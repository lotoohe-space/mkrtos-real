
#include "u_types.h"
#include "u_prot.h"
#include "u_app.h"
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
 * @brief 加载并执行一个app
 *
 * @param name app的名字
 * @return int
 */
int app_load(const char *name, uenv_t *cur_env)
{
    msg_tag_t tag;
    sys_info_t sys_info;

    tag = sys_read_info(SYS_PROT, &sys_info);
    if (msg_tag_get_val(tag) < 0)
    {
        return -ENOENT;
    }
    umword_t addr = cpio_find_file((umword_t)sys_info.bootfs_start_addr, (umword_t)(-1), name, NULL);

    if (!addr)
    {
        return -ENOENT;
    }

    app_info_t *app = app_info_get((void *)addr);

    if (app == NULL)
    {
        printf("app format is error.\n");
        return -1;
    }
    printf("%s addr is 0x%x\n", name, app);
    umword_t ram_base;
    obj_handler_t hd_task = handler_alloc();
    obj_handler_t hd_thread = handler_alloc();

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
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_thread));
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
    tag = task_map(hd_task, cur_env->ns_hd, cur_env->ns_hd, KOBJ_DELETE_RIGHT);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = thread_msg_buf_set(hd_thread, (void *)(ram_base + app->i.ram_size));
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = thread_bind_task(hd_thread, hd_task);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    tag = task_set_pid(hd_task, hd_task); //!< 设置进程的pid就是进程hd号码
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_del_obj;
    }
    void *sp_addr = (char *)ram_base + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;

    umword_t usp_top = ((umword_t)((umword_t)sp_addr_top - 8) & ~0x7UL) - MSG_BUG_LEN;

    /**处理传参*/
    umword_t *buf;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    umword_t *buf_bk = buf;
#define ARG_WORD_NR 10
    buf = (umword_t *)app_stack_push(buf, 1);                                        //!< argc 24
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + ARG_WORD_NR * 4);      //!< argv[0]
    buf = (umword_t *)app_stack_push(buf, 0);                                        //!< NULL
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + ARG_WORD_NR * 4 + 16); //!< env[0...N]
    buf = (umword_t *)app_stack_push(buf, 0);                                        //!< NULL

    buf = (umword_t *)app_stack_push(buf, (umword_t)AT_PAGESZ);                           //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, MK_PAGE_SIZE);                                  //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, 0xfe);                                          //!< auxvt[0...N] mkrtos_env
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + ARG_WORD_NR * 4 + 16 + 16); //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, 0);                                             //!< NULL

    // set args & env.
    memcpy((char *)buf_bk + ARG_WORD_NR * 4, name, strlen(name) + 1);
    memcpy((char *)buf_bk + ARG_WORD_NR * 4 + 16, "PATH=/", strlen("PATH=/") + 1);

    // set user env.
    uenv_t *uenv = (uenv_t *)((char *)buf_bk + ARG_WORD_NR * 4 + 16 + 16);
    uenv->log_hd = cur_env->ns_hd;
    uenv->ns_hd = cur_env->ns_hd;
    uenv->rev1 = HANDLER_INVALID;
    uenv->rev2 = HANDLER_INVALID;

    tag = thread_exec_regs(hd_thread, (umword_t)addr, (umword_t)sp_addr_top - sizeof(void *), ram_base, 1);
    assert(msg_tag_get_prot(tag) >= 0);

    /*启动线程运行*/
    tag = thread_run(hd_thread, 2);
    assert(msg_tag_get_prot(tag) >= 0);
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
