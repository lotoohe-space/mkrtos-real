
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "u_ns.h"
#include "test.h"
#include "u_rpc.h"
#include "ns_cli.h"
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
#if 0
    int ret;
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));
    assert(msg_tag_get_val(tag) >= 0);
    ret = cli_ns_register("shell", tmp_ipc_hd);
    assert(ret >= 0);
    ret = cli_ns_query("shell", &tmp_ipc_hd);
    assert(ret >= 0);
    ulog_write_str(u_get_global_env()->log_hd, "ns test success.\n");
#endif
    obj_handler_t tmp_ipc_hd;

    tmp_ipc_hd = handler_alloc();
    assert(tmp_ipc_hd != HANDLER_INVALID);
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(0, 0, tmp_ipc_hd));

    assert(ns_register("shell", tmp_ipc_hd) >= 0);
    obj_handler_t rcv_ipc_hd;

    assert(ns_query("shell", &rcv_ipc_hd) >= 0);

    handler_free_umap(rcv_ipc_hd);
}
#include "fs_cli.h"
void fs_test(void)
{
    char tmp[4] = "123";
    int fd = fs_open("/test", 0, 0x1 | 0x2 | 0x8);
    assert(fd >= 0);
    int wlen = fs_write(fd, tmp, 4);
    assert(wlen == 4);
    int ret = fs_lseek(fd, 0, SEEK_SET);
    assert(ret >= 0);
    int rlen = fs_read(fd, tmp, 4);
    assert(rlen == 4);
    assert(strcmp(tmp, "123") == 0);
    fs_close(fd);
}
int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    ulog_write_str(u_get_global_env()->log_hd, "MKRTOS:\n");
#if 0
    malloc_test();
    irq_test();
#endif
    rpc_test();
    ns_test();
    fs_test();
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, TASK_THIS));
    ulog_write_str(u_get_global_env()->log_hd, "Error.\n");
    return 0;
}
