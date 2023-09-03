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
#include <assert.h>
#include <string.h>
/**
 * @brief 加载一个app，并启动
 *
 */
void app_test(void)
{
    msg_tag_t tag;
    umword_t addr = cpio_find_file((umword_t)0x8020000, (umword_t)0x8040000, "shell");
    assert(addr);

    app_info_t *app = (app_info_t *)addr;
    printf("app addr is 0x%x\n", app);
    umword_t ram_base;
    obj_handler_t hd_task = handler_alloc();
    obj_handler_t hd_thread = handler_alloc();
    obj_handler_t hd_ipc = handler_alloc();

    assert(hd_task != HANDLER_INVALID);
    assert(hd_thread != HANDLER_INVALID);

    tag = factory_create_task(FACTORY_PROT, hd_task);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_thread(FACTORY_PROT, hd_thread);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = factory_create_ipc(FACTORY_PROT, hd_ipc);
    assert(msg_tag_get_prot(tag) >= 0);
    printf("ipc hd is %d\n", hd_ipc);

    tag = task_alloc_ram_base(hd_task, app->i.ram_size, &ram_base);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, LOG_PROT, LOG_PROT);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, hd_ipc, hd_ipc);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = task_map(hd_task, hd_thread, THREAD_MAIN);
    assert(msg_tag_get_prot(tag) >= 0);
    void *sp_addr = (char *)ram_base + app->i.stack_offset - app->i.data_offset;
    void *sp_addr_top = (char *)sp_addr + app->i.stack_size;
    tag = thread_exec_regs(hd_thread, (umword_t)addr, (umword_t)sp_addr_top, ram_base);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(hd_thread, hd_task);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(hd_thread, ram_base + app->i.ram_size);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run(hd_thread);
    assert(msg_tag_get_prot(tag) >= 0);

    char *buf;
    umword_t len;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    strcpy(buf, "hello shell.\n");
    ipc_send(hd_ipc, strlen(buf), 0);
    printf("test ok\n");
}
