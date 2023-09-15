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
#include <assert.h>
#include <string.h>
#include <elf.h>
#include <stdio.h>
umword_t app_stack_push(umword_t *stack, umword_t val)
{
    *stack = val;
    stack++;
    return (umword_t)stack;
}
#define TEST_APP_NAME "shell"
/**
 * @brief 加载一个app，并启动
 *
 */
void app_test(void)
{
    msg_tag_t tag;
    umword_t addr = cpio_find_file((umword_t)0x8020000, (umword_t)0x8040000, TEST_APP_NAME);
    assert(addr);

    app_info_t *app = (app_info_t *)addr;
    printf("app addr is 0x%x\n", app);
    umword_t ram_base;
    obj_handler_t hd_task = handler_alloc();
    obj_handler_t hd_thread = handler_alloc();
    obj_handler_t hd_ipc = handler_alloc();

    assert(hd_task != HANDLER_INVALID);
    assert(hd_thread != HANDLER_INVALID);

    tag = factory_create_task(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_task));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_thread));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd_ipc));
    assert(msg_tag_get_prot(tag) >= 0);
    printf("ipc hd is %d\n", hd_ipc);

    tag = task_alloc_ram_base(hd_task, app->i.ram_size, &ram_base);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, hd_task, TASK_PROT, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, LOG_PROT, LOG_PROT, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, hd_ipc, hd_ipc, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, hd_thread, THREAD_MAIN, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, FACTORY_PROT, FACTORY_PROT, 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, MM_PROT, MM_PROT, 0);
    assert(msg_tag_get_prot(tag) >= 0);

    tag = thread_msg_buf_set(hd_thread, (void *)(ram_base + app->i.ram_size));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(hd_thread, hd_task);
    assert(msg_tag_get_prot(tag) >= 0);
    ipc_bind(hd_ipc, hd_thread, 0);

    void *sp_addr = (char *)ram_base + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;

    umword_t usp_top = ((umword_t)((umword_t)sp_addr_top - 8) & ~0x7UL) - MSG_BUG_LEN;

    /**处理传参*/
    umword_t *buf;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    umword_t *buf_bk = buf;

    // 传递的参数放到最后64字节，可以存放16-1个words.
    buf = (umword_t *)app_stack_push(buf, 1);                      //!< argc 24
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + 64); //!< argv[0]
    buf = (umword_t *)app_stack_push(buf, 0);                      //!< NULL
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + 96); //!< env[0...N]
    buf = (umword_t *)app_stack_push(buf, 0);                      //!< NULL

    buf = (umword_t *)app_stack_push(buf, (umword_t)AT_PAGESZ);         //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, MK_PAGE_SIZE);                //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, (umword_t)usp_top + 96 + 16); //!< auxvt[0...N]
    buf = (umword_t *)app_stack_push(buf, 0xfe);                        //!< auxvt[0...N]

    buf = (umword_t *)app_stack_push(buf, 0); //!< 0
    buf = (umword_t *)app_stack_push(buf, 0); //!< 0
    buf = (umword_t *)app_stack_push(buf, 0); //!< 0
    buf = (umword_t *)app_stack_push(buf, 0); //!< 0
    buf = (umword_t *)app_stack_push(buf, 0); //!< 0
    buf = (umword_t *)app_stack_push(buf, 0); //!< 0

    printf("argc addr is 0x%x\n", buf);
    memcpy((char *)buf_bk + 64, TEST_APP_NAME, strlen(TEST_APP_NAME) + 1);
    memcpy((char *)buf_bk + 96, "PATH=/", strlen("PATH=/") + 1);
    uenv_t *uenv = (uenv_t *)((char *)buf_bk + 96 + 16);
    uenv->log_hd = LOG_PROT;

    tag = thread_exec_regs(hd_thread, (umword_t)addr, (umword_t)sp_addr_top, ram_base, 1);
    assert(msg_tag_get_prot(tag) >= 0);

    /*启动线程运行*/
    tag = thread_run(hd_thread, 2);
    assert(msg_tag_get_prot(tag) >= 0);

    umword_t len;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    strcpy((char *)buf, "hello shell.\n");
    ipc_call(hd_ipc, msg_tag_init4(0, ROUND_UP(strlen((char *)buf), WORD_BYTES), 0, 0), ipc_timeout_create2(0, 0));
    printf("test ok\n");
}
