
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_env.h"
#include "test.h"
#include <assert.h>
#include <stdio.h>
#include <malloc.h>

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

int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    ulog_write_str(u_get_global_env()->log_hd, "MKRTOS:\n");
    malloc_test();
    char *buf;
    ipc_msg_t *ipc_msg;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    ipc_msg = (ipc_msg_t *)buf;
    ipc_msg->msg_buf[0] = 0x112233;
    ipc_msg->msg_buf[1] = 0x223344;
    msg_tag_t tag = ipc_call(u_get_global_env()->ns_hd, msg_tag_init4(0, 2, 0, 0), ipc_timeout_create2(0, 0));
    printf("msg %d\n", tag.msg_buf_len);
    irq_test();
    // ipc_wait(12, 0);
    // ipc_reply(12, msg_tag_init4(0, 0, 0, 0));
    // umword_t len;
    // thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    // printf(buf);
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(u_get_global_env()->log_hd, "Error.\n");
    return 0;
}
