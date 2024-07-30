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
#include "u_arch.h"
#include "u_factory.h"
#include "u_mm.h"
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
#include <u_elf64.h>
#include <u_vmam.h>
#include <stdlib.h>
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
    (*stack)--;
    *(*stack) = val;

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
    int len = strlen(str) + 1;

    *stack -= ALIGN(len, sizeof(void *)) / sizeof(void *);
    for (int i = 0; i < len; i++)
    {
        ((char *)(*stack))[i] = str[i];
    }

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
    *stack -= ALIGN(len, sizeof(void *)) / sizeof(void *);
    memcpy(*stack, arr, len);
    return *stack;
}
static int thread_set_msg_buf(obj_handler_t hd_task, obj_handler_t hd_thread)
{
    addr_t mem;
    msg_tag_t tag;

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, 0, 0), PAGE_SIZE, 0, (addr_t *)(&mem));
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    memset((void *)mem, 0, PAGE_SIZE);
    tag = u_vmam_grant(VMA_PROT, hd_task, (addr_t)mem, CONFIG_MSG_BUF_VADDR, PAGE_SIZE);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    // 设置msgbuff
    tag = thread_msg_buf_set(hd_thread, (void *)(CONFIG_MSG_BUF_VADDR));
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return 0;
}
/**
 * @brief 加载并执行一个app
 *
 * @param name app的名字
 * @return int
 */
int app_load(const char *name, uenv_t *cur_env, pid_t *pid, char *argv[], int arg_cn, char *envp[], int envp_cn)
{
    msg_tag_t tag;
    sys_info_t sys_info;
    int ret;

    tag = sys_read_info(SYS_PROT, &sys_info, 0);
    if (msg_tag_get_val(tag) < 0)
    {
        return -ENOENT;
    }
    int type;
    umword_t addr;
    umword_t size;

    ret = cpio_find_file((umword_t)sys_info.bootfs_start_addr, (umword_t)(-1), name, &size, &type, &addr);

    if (ret < 0 || (ret >= 0 && type == 1))
    {
        return -ENOENT;
    }

    obj_handler_t hd_task = handler_alloc();
    obj_handler_t hd_thread = handler_alloc();
    addr_t entry_addr;

    if (hd_task == HANDLER_INVALID)
    {
        goto end;
    }
    if (hd_thread == HANDLER_INVALID)
    {
        goto end;
    }

    tag = factory_create_task(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_task));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }

    ret = elf_load(addr, size, &entry_addr, hd_task);
    if (ret < 0)
    {
        goto end_del_obj;
    }

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_thread));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    // tag = task_alloc_ram_base(hd_task, app->i.ram_size, &ram_base);
    // if (msg_tag_get_prot(tag) < 0)
    // {
    //     goto end_del_obj;
    // }
    tag = task_map(hd_task, hd_task, TASK_PROT, 0);
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
    tag = task_map(hd_task, MM_PROT, MM_PROT, KOBJ_DELETE_RIGHT);
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
    ret = thread_set_msg_buf(hd_task, hd_thread);
    if (ret < 0)
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
    void *sp_addr;
    void *sp_addr_top;

#define MAIN_THREAD_STACK_SIZE ((1 << CONFIG_PAGE_SHIFT) * 8)

    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, 0),
                       MAIN_THREAD_STACK_SIZE, 0, (addr_t *)(&sp_addr));
    if (msg_tag_get_val(tag) < 0)
    {
        goto end_del_obj;
    }
    sp_addr_top = (char *)sp_addr + MAIN_THREAD_STACK_SIZE;

    printf("stack:0x%x size:%d.\n", sp_addr, MAIN_THREAD_STACK_SIZE);
    umword_t *usp_top = (umword_t *)((umword_t)((umword_t)sp_addr_top - sizeof(void *)) & ~0x7UL);
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
    }
    for (int i = 0; i < envp_cn; i++)
    {
        cp_envp = app_stack_push_str(hd_task, &usp_top, envp[i]);
    }

    app_stack_push_umword(hd_task, &usp_top, 0);

    app_stack_push_umword(hd_task, &usp_top, (umword_t)app_env - (addr_t)sp_addr + 0x8000000);
    app_stack_push_umword(hd_task, &usp_top, 0xfe);

    app_stack_push_umword(hd_task, &usp_top, MK_PAGE_SIZE);
    app_stack_push_umword(hd_task, &usp_top, (umword_t)AT_PAGESZ);

    app_stack_push_umword(hd_task, &usp_top, 0);
    for (int i = 0; i < envp_cn; i++)
    {
        app_stack_push_umword(hd_task, &usp_top, (umword_t)cp_envp - (addr_t)sp_addr + 0x8000000);
        cp_envp += ALIGN(strlen(envp[i]), sizeof(void *));
    }
    if (arg_cn)
    {
        app_stack_push_umword(hd_task, &usp_top, 0);
        for (int i = 0; i < arg_cn; i++)
        {
            app_stack_push_umword(hd_task, &usp_top, (umword_t)cp_args - (addr_t)sp_addr + 0x8000000);
            cp_args += ALIGN(strlen(argv[i]) + 1, sizeof(void *));
        }
    }
    app_stack_push_umword(hd_task, &usp_top, arg_cn);

    printf("pid:%d stack:%p\n", hd_task, usp_top);
    tag = thread_exec_regs(hd_thread, (umword_t)entry_addr, (umword_t)usp_top - (addr_t)sp_addr + 0x8000000, 0, 0);
    assert(msg_tag_get_prot(tag) >= 0);

    tag = u_vmam_grant(VMA_PROT, hd_task, (addr_t)sp_addr, 0x8000000, MAIN_THREAD_STACK_SIZE);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto free_data;
    }

    /*启动线程运行*/
    tag = thread_run_cpu(hd_thread, 2, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    task_unmap(TASK_THIS, vpage_create_raw3(0, 0, hd_thread));
    handler_free(hd_thread);
    return 0;

free_data:
    if (sp_addr)
    {
        u_vmam_free(VMA_PROT, (addr_t)sp_addr, MAIN_THREAD_STACK_SIZE);
    }
end_del_obj:
    if (hd_thread != HANDLER_INVALID)
    {
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_thread));
    }
    if (hd_task != HANDLER_INVALID)
    {
        task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_task));
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
    return -ENOMEM;
}
