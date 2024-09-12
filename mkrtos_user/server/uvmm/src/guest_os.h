#pragma once
#include <u_thread.h>
#include <pthread.h>
#include "u_types.h"
#include "uvmm_devtree.h"
#include "uvmm_host_irqs.h"

struct guest_os;
typedef struct guest_os guest_os_t;
struct vmm_dev;
typedef struct vmm_dev vmm_dev_t;
struct vmm_devtree_id;
typedef struct vmm_devtree_id vmm_devtree_id_t;
typedef struct guest_vcpu
{
    char *name;
    obj_handler_t vcpu_th; //!< cpu的hd
    guest_os_t *guest;
    int vcpu_id; //!< cpu id
} guest_vcpu_t;

typedef int (*vmm_dev_read_uint8_cb)(vmm_dev_t *dev, paddr_t offset, uint8_t *dst);
typedef int (*vmm_dev_read_uint16_cb)(vmm_dev_t *dev, paddr_t offset, uint16_t *dst);
typedef int (*vmm_dev_read_uint32_cb)(vmm_dev_t *dev, paddr_t offset, uint32_t *dst);
typedef int (*vmm_dev_read_uint64_cb)(vmm_dev_t *dev, paddr_t offset, uint64_t *dst);
typedef int (*vmm_dev_write_uint8_cb)(vmm_dev_t *dev, paddr_t offset, uint8_t dst);
typedef int (*vmm_dev_write_uint16_cb)(vmm_dev_t *dev, paddr_t offset, uint16_t dst);
typedef int (*vmm_dev_write_uint32_cb)(vmm_dev_t *dev, paddr_t offset, uint32_t dst);
typedef int (*vmm_dev_write_uint64_cb)(vmm_dev_t *dev, paddr_t offset, uint64_t dst);
typedef int (*vmm_dev_reset_cb)(vmm_dev_t *dev);
typedef int (*vmm_dev_probe)(guest_os_t *guest,
                             vmm_dev_t *edev,
                             const vmm_devtree_id_t *eid);
typedef int (*vmm_dev_remove)(vmm_dev_t *edev);

typedef struct vmm_simul
{
    char *name;

    vmm_dev_read_uint8_cb read_u8;
    vmm_dev_read_uint16_cb read_u16;
    vmm_dev_read_uint32_cb read_u32;
    vmm_dev_read_uint64_cb read_u64;
    vmm_dev_write_uint8_cb write_u8;
    vmm_dev_write_uint16_cb write_u16;
    vmm_dev_write_uint32_cb write_u32;
    vmm_dev_write_uint64_cb write_u64;
    vmm_dev_reset_cb reset;
    vmm_dev_probe probe;
    vmm_dev_remove remove;
    vmm_devtree_id_t *match_tab;
} vmm_simul_t;
/**
 * @brief 设备模拟的结构体
 *
 */
typedef struct vmm_dev
{
    guest_os_t *guest;
    vmm_simul_t *dev_operations;
    vmm_devtree_node_t *devtree_node;
    void *priv;
    // int ref_cnt;
} vmm_dev_t;

#define VMM_DEV_LIST 128 //!< 支持的最大设备数量
#define GUEST_OS_NAME_MAX_LEN 128
typedef struct guest_os
{
    char name[GUEST_OS_NAME_MAX_LEN];
    guest_vcpu_t vcpu;               //!< TODO:暂时只支持一个cpu
    pthread_t ipa_exeception_hd;     // 异常处理线程，每个cpu应该有一个
    obj_handler_t ipa_execption_ipc; // 异常处理线程的hd，每个cpu应该有一个

    void *msg_buf; //!< 消息传递的buf
    int prio;      //!< 优先级
    void *os_mem;  //!< os内存

    vmm_dev_t *devs[VMM_DEV_LIST]; //!< 匹配成功的列表
} guest_os_t;

int guest_os_create(guest_os_t *gos, char *cfg,
                    addr_t st_addr, void *entry, void *bin_end, size_t mem_size);
int guest_os_add_dev(guest_os_t *guest, vmm_dev_t *dev);
vmm_dev_t *guest_os_find_region(guest_os_t *guest, paddr_t addr);

static inline guest_vcpu_t *uvmm_scheduler_current_vcpu(void)
{
    guest_vcpu_t *vcpu = (guest_vcpu_t *)(thread_get_cur_ipc_msg()->user[3]);

    return vcpu;
}
