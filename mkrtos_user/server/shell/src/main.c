
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
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
    ulog_write_str(LOG_PROT, "MKRTOS:\n");
    malloc_test();
    ipc_wait(12, 0);
    ipc_reply(12, msg_tag_init4(0, 0, 0, 0));
    char *buf;
    umword_t len;
    thread_msg_buf_get(THREAD_MAIN, (umword_t *)(&buf), NULL);
    printf(buf);
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(LOG_PROT, "Error.\n");
    return 0;
}
