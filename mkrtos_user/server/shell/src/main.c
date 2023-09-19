
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "test.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
void malloc_test(void)
{
    void *mem = malloc(1024);
    assert(mem);
    void *mem1 = malloc(1024);
    assert(mem1);
    free(mem);
    free(mem1);
    mem = malloc(4 * 1024);
    assert(mem);
    free(mem);
    mem1 = malloc(1024);
    assert(mem1);
    free(mem1);
}
void ns_test(void)
{
    char *buf;
    ipc_msg_t *ipc_msg;
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));
    assert(msg_tag_get_val(tag) >= 0);

    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    ipc_msg->msg_buf[0] = 0;
    ipc_msg->msg_buf[1] = strlen("shell") + 1;
    ipc_msg->map_buf[0] = vpage_create_raw3(0, 0, tmp_ipc_hd).raw;
    strcpy((char *)(&ipc_msg->msg_buf[2]), "shell");
    tag = ipc_call(u_get_global_env()->ns_hd, msg_tag_init4(0, 2 + ROUND_UP(strlen("shell"), WORD_BYTES), 1, 0), ipc_timeout_create2(0, 0));
    printf("msg %d\n", tag.msg_buf_len);
    handler_free_umap(tmp_ipc_hd);
}
int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    ulog_write_str(u_get_global_env()->log_hd, "MKRTOS:\n");
#if 0
    malloc_test();
    irq_test();
#endif
    ns_test();
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(u_get_global_env()->log_hd, "Error.\n");
    return 0;
}
