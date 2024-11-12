/**
 * @file u_app_loader.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "u_types.h"
#include "u_prot.h"
#include "u_app.h"
#include "u_factory.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_thread.h"
#include "u_ipc.h"
#include "cpiofs.h"
#include "u_env.h"
#include "u_sys.h"
#include <assert.h>
#include <string.h>
#include <elf.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
/**
 * @brief 向栈中存放数据
 *
 * @param stack
 * @param val
 * @return umword_t
 */
static umword_t app_stack_push(umword_t *stack, umword_t val)
{
    *stack = val;
    stack++;
    return (umword_t)stack;
}

/**
 * @brief 放入一个无符号整型
 *
 * @param task_obj
 * @param stack
 * @param val
 */
static void *app_stack_push_umword(obj_handler_t task_obj, umword_t **stack, umword_t val)
{
    ipc_msg_t *msg = thread_get_cur_ipc_msg();
    (*stack)--;

    msg->msg_buf[0] = val;
    task_copy_data(task_obj, *stack, sizeof(umword_t));

    return *stack;
}
/**
 * @brief 放入一个字符串
 *
 * @param task_obj
 * @param stack
 * @param str
 */
static void *app_stack_push_str(obj_handler_t task_obj, umword_t **stack, const char *str)
{
    ipc_msg_t *msg = thread_get_cur_ipc_msg();
    int len = strlen(str) + 1;
    char *cp_data = (char *)msg->msg_buf;

    len = MIN(len, ARRAY_SIZE(msg->msg_buf) * sizeof(umword_t));
    *stack -= ALIGN(len, sizeof(void *)) / sizeof(void *);

    memcpy(cp_data, str, len);
    cp_data[len - 1] = 0;

    task_copy_data(task_obj, *stack, len);

    return *stack;
}
/**
 * @brief 压入数组
 *
 * @param task_obj
 * @param stack
 * @param arr
 * @param len
 * @return void*
 */
static void *app_stack_push_array(obj_handler_t task_obj, umword_t **stack, uint8_t *arr, size_t len)
{
    ipc_msg_t *msg = thread_get_cur_ipc_msg();
    char *cp_data = (char *)msg->msg_buf;

    len = MIN(len, ARRAY_SIZE(msg->msg_buf) * sizeof(umword_t));
    *stack -= ALIGN(len, sizeof(void *)) / sizeof(void *);

    memcpy(cp_data, arr, len);
    cp_data[len - 1] = 0;

    task_copy_data(task_obj, *stack, len);

    return *stack;
}

/**
 * @brief 加载并执行一个app
 *
 * @param name app的名字
 * @return int
 */
int app_load(const char *name, uenv_t *cur_env, pid_t *pid, char *argv[], int arg_cn, char *envp[], int envp_cn, obj_handler_t *p_sem_hd)
{
    msg_tag_t tag;
    sys_info_t sys_info;
    assert(p_sem_hd);

    tag = sys_read_info(SYS_PROT, &sys_info, 0);
    if (msg_tag_get_val(tag) < 0)
    {
        return -ENOENT;
    }
    int type;
    umword_t addr;
    int ret = cpio_find_file((umword_t)sys_info.bootfs_start_addr, (umword_t)(-1), name, NULL, &type, &addr);

    if (ret < 0 || (ret >= 0 && type == 1))
    {
        return -ENOENT;
    }

    app_info_t *app = app_info_get((void *)addr);

    if (app == NULL)
    {
        printf("app format is error.\n");
        return -1;
    }
    printf("%s addr is [0x%x]\n", name, app);
    umword_t ram_base;
    obj_handler_t hd_task = handler_alloc();
    obj_handler_t hd_thread = handler_alloc();
    obj_handler_t hd_sem = handler_alloc();

    if (hd_task == HANDLER_INVALID)
    {
        goto end;
    }
    if (hd_thread == HANDLER_INVALID)
    {
        goto end;
    }
    if (hd_sem == HANDLER_INVALID)
    {
        goto end;
    }
    tag = factory_create_task(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_task));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_thread));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = facotry_create_sema(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_sem), 0, (uint32_t)(-1));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_alloc_ram_base(hd_task, app->i.ram_size, &ram_base);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, hd_task, TASK_PROT, 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, hd_sem, SEMA_PROT, 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, cur_env->ns_hd, LOG_PROT, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, SYS_PROT, SYS_PROT, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, FUTEX_PROT, FUTEX_PROT, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, hd_thread, THREAD_MAIN, 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, FACTORY_PROT, FACTORY_PROT, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, VMA_PROT, VMA_PROT, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_map(hd_task, cur_env->ns_hd, cur_env->ns_hd, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = thread_bind_task(hd_thread, hd_task);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = thread_msg_buf_set(hd_thread, (void *)(ram_base + app->i.ram_size));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_set_pid(hd_task, hd_task); //!< 设置进程的pid就是进程hd号码
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    if (pid)
    {
        *pid = hd_task;
    }
    void *sp_addr = (char *)ram_base + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;
    printf("stack:0x%x size:%d.\n", sp_addr, app->i.stack_size);
    umword_t *usp_top = (umword_t *)((umword_t)((umword_t)sp_addr_top - 8) & ~0x7UL);
    uenv_t uenv = {
        .log_hd = cur_env->ns_hd,
        .ns_hd = cur_env->ns_hd,
        .rev1 = HANDLER_INVALID,
        .rev2 = HANDLER_INVALID,
    };
    umword_t *app_env;
    char *cp_args;
    char *cp_envp;

    app_env = app_stack_push_array(hd_task, &usp_top, (uint8_t *)(&uenv), sizeof(uenv));
    for (int i = 0; i < arg_cn; i++)
    {
        cp_args = app_stack_push_str(hd_task, &usp_top, argv[i]);
        if ((ALIGN(strlen(argv[i]) + 1, sizeof(void *)) / sizeof(void *)) % 2)
        {
            app_stack_push_umword(hd_task, &usp_top, 0);
        }
        printf("app_load 1 cp_args:%p\n", cp_args);
    }
    for (int i = 0; i < envp_cn; i++)
    {
        cp_envp = app_stack_push_str(hd_task, &usp_top, envp[i]);
        if ((ALIGN(strlen(argv[i]) + 1, sizeof(void *)) / sizeof(void *)) % 2)
        {
            app_stack_push_umword(hd_task, &usp_top, 0);
        }
    }

    app_stack_push_umword(hd_task, &usp_top, 0);
    if ((arg_cn + envp_cn) & 0x1) // 参数是奇数是，多添加一个
    {
        app_stack_push_umword(hd_task, &usp_top, 0);
    }

    app_stack_push_umword(hd_task, &usp_top, (umword_t)app_env);
    app_stack_push_umword(hd_task, &usp_top, 0xfe);

    app_stack_push_umword(hd_task, &usp_top, MK_PAGE_SIZE);
    app_stack_push_umword(hd_task, &usp_top, (umword_t)AT_PAGESZ);

    app_stack_push_umword(hd_task, &usp_top, 0);
    for (int i = 0; i < envp_cn; i++)
    {
        app_stack_push_umword(hd_task, &usp_top, (umword_t)cp_envp);
        cp_envp += ALIGN(strlen(envp[i]), sizeof(void *));
        if ((ALIGN(strlen(envp[i]) + 1, sizeof(void *)) / sizeof(void *)) % 2)
        {
            cp_envp += sizeof(void *);
        }
    }
    if (arg_cn)
    {
        app_stack_push_umword(hd_task, &usp_top, 0);
        for (int i = 0; i < arg_cn; i++)
        {
            if (i != 0)
            {
                if ((ALIGN(strlen(argv[i]) + 1, sizeof(void *)) / sizeof(void *)) % 2)
                {
                    cp_args += sizeof(void *);
                }
            }
            printf("app_load 2 cp_args:%p\n", cp_args);
            app_stack_push_umword(hd_task, &usp_top, (umword_t)cp_args);
            cp_args += ALIGN(strlen(argv[i]) + 1, sizeof(void *));
        }
    }
    app_stack_push_umword(hd_task, &usp_top, arg_cn);

    printf("pid:%d stack:%p\n", hd_task, usp_top);
    tag = thread_exec_regs(hd_thread, (umword_t)addr, (umword_t)usp_top,
                           ram_base, 0);
    assert(msg_tag_get_prot(tag) >= 0);

    *p_sem_hd = hd_sem;
    /*启动线程运行*/
    tag = thread_run(hd_thread, 2);
    assert(msg_tag_get_prot(tag) >= 0);
    task_unmap(TASK_THIS, vpage_create_raw3(0, 0, hd_thread));
    handler_free(hd_thread);
    return 0;
end_del_obj:
    if (hd_thread != HANDLER_INVALID)
    {
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_thread));
    }
    if (hd_task != HANDLER_INVALID)
    {
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_task));
    }
    if (hd_sem != HANDLER_INVALID)
    {
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_sem));
    }
end:
    if (hd_task != HANDLER_INVALID)
    {
        handler_free(hd_task);
    }
    if (hd_thread != HANDLER_INVALID)
    {
        handler_free(hd_thread);
    }
    if (hd_sem != HANDLER_INVALID)
    {
        handler_free(hd_sem);
    }
    return -ENOMEM;
}
