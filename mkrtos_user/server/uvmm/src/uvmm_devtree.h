#pragma once
#include "u_types.h"
#include <cJSON.h>
struct guest_os;
typedef struct guest_os guest_os_t;
// 启动时候扫描设备树，然后和驱动列表中的驱动进行一一匹配
// 匹配成功则进行probe，probe则给guest_os添加新的模拟设备.

#define VMM_FIELD_SHORT_NAME_SIZE 64
#define VMM_FIELD_TYPE_SIZE 64
#define VMM_FIELD_COMPAT_SIZE 64
typedef struct vmm_devtree_id
{
    char name[VMM_FIELD_SHORT_NAME_SIZE];
    char type[VMM_FIELD_TYPE_SIZE];
    char compatible[VMM_FIELD_COMPAT_SIZE];
    const void *data;
} vmm_devtree_id_t;

typedef struct vmm_devtree_node
{
    char compatible[VMM_FIELD_COMPAT_SIZE]; //!< compatible用于设备匹配
    addr_t addr;                            //!< 设备模拟的寄存器地址
    size_t size;                            //!< 寄存器的大小，PAGE_SIZE对齐
    cJSON *json_node;
} vmm_devtree_node_t;

int uvmm_devtree_scan(guest_os_t *guest, const char *data);
