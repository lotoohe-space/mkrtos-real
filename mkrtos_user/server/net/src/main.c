
#include "cons_cli.h"
#include "lwiperf.h"
#include "netconf.h"
#include "u_prot.h"
#include "u_sleep.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "fs_rpc.h"
#include "net_rpc.h"
#include "net_test.h"
#include "u_fast_ipc.h"
#include <blk_drv_cli.h>
#include <ns_cli.h>
#include <u_factory.h>
#include <u_hd_man.h>
#include <u_sema.h>
#include <u_share_mem.h>
#include <u_task.h>
static umword_t addr;
static umword_t size;
obj_handler_t net_drv_hd = HANDLER_INVALID;

#define STACK_COM_ITME_SIZE (2 * 1024 /*sizeof(struct pthread) + TP_OFFSET*/)
#define STACK_NUM 4
ATTR_ALIGN(8)
static uint8_t stack_coms[STACK_COM_ITME_SIZE * STACK_NUM];
static uint8_t msg_buf_coms[MSG_BUG_LEN * STACK_NUM];
static obj_handler_t com_th_obj[STACK_NUM];
static void fast_ipc_init(void)
{
    for (int i = 0; i < STACK_NUM; i++)
    {
        com_th_obj[i] = handler_alloc();
        assert(com_th_obj[i] != HANDLER_INVALID);
    }
    u_fast_ipc_init(stack_coms,
                    msg_buf_coms, STACK_NUM, STACK_COM_ITME_SIZE, com_th_obj);
}
int main(int args, char *argv[])
{
    int ret;
    msg_tag_t tag;
    obj_handler_t hd;
    obj_handler_t sem_hd;
    obj_handler_t shm_hd;
    int count_net_link = 0;
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_net");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_net");
    printf("net startup..\n");
    fast_ipc_init();
    ret = rpc_meta_init(TASK_THIS, &hd);
    if (ret < 0)
    {
        printf("rpc meta init failed\n");
        return -1;
    }
again:
    ret = ns_query_svr("/eth", &net_drv_hd, 0x1);
    if (ret < 0)
    {
        // 0代表根节点
        u_sleep_ms(50);
        count_net_link++;
        if (count_net_link < 20)
        {
            goto again;
        }
        net_drv_hd = HANDLER_INVALID;
    }
    cons_write_str("net init..\n");
    net_hw_init();
    net_svr_init();
    fs_svr_init();
    cons_write_str("net start success..\n");
    ip_addr_t perf_server_ip;

    IP_ADDR4(&perf_server_ip, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    lwiperf_start_tcp_server(&perf_server_ip, 9527, NULL, NULL);

    if (net_drv_hd != HANDLER_INVALID)
    {
        if (blk_drv_cli_map(net_drv_hd, &sem_hd) < 0)
        {
            printf("net drv sem map error.\n");
            return -1;
        }

        shm_hd = handler_alloc();
        if (shm_hd == HANDLER_INVALID)
        {
            printf("handler alloc failed.\n");
            return -1;
        }
        tag = facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd),
                                       SHARE_MEM_CNT_BUDDY_CNT, 2048);
        if (msg_tag_get_val(tag) < 0)
        {
            printf("share mem create failed.\n");
            return -1;
        }
        tag = share_mem_map(shm_hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &addr, &size);
        if (msg_tag_get_val(tag) < 0)
        {
            printf("share mem map failed.\n");
            return -1;
        }
    }

    ret = ns_register("/sys/net", hd, 0);
    if (ret < 0)
    {
        printf("ns reg failed.\n");
        return -1;
    }
    cons_write_str("net mount success\n");
    // net_test();
    while (1)
    {
        int ret;

        if (net_drv_hd != HANDLER_INVALID)
        {
            if (msg_tag_get_prot(u_sema_down(sem_hd, 0, NULL)) < 0)
            {
                printf("error.\n");
            }
            ret = blk_drv_cli_read(net_drv_hd, shm_hd, 0, 0);
            if (ret > 0)
            {
                lwip_pkt_handle_raw((uint8_t *)addr, ret);
            }
        }
        else
        {
            u_sleep_ms(0);
        }
    }
    return 0;
}
