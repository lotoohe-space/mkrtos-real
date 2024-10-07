
#pragma once

#define MEM_NODES_NUM 4
#define FLASH_NODES_NUM 4
#define FLASH_LAYER_NUM 4
typedef struct mem_node
{
    addr_t addr;       //!< 开始地址
    size_t size;       //!< 大小
    int speed;         //!< 内存速度
    bool_t is_sys_mem; //!< 系统内存
} mem_node_t;
typedef struct mem_list
{
    mem_node_t mem_list[MEM_NODES_NUM];
    int mem_num;
} mem_list_t;
typedef struct flash_list
{
    mem_node_t flash_list[FLASH_NODES_NUM];
    int flash_num;
} flash_list_t;

typedef struct flash_layer
{
    addr_t st_addr;
    size_t size;
    const char *name;
} flash_layer_t;

typedef struct flash_layer_list
{
    flash_layer_t flash_layer_list[FLASH_LAYER_NUM];
    int flash_layer_num;
} flash_layer_list_t;
#define BOOTSTARP_LAYER_0 0
#define DTBO_LAYER_1 1
#define KNL_LAYER_2 2
#define BOOTFS_LAYER_3 3
typedef struct boot_info
{
    mem_list_t mem;
    flash_list_t flash;
    flash_layer_list_t flash_layer;
} boot_info_t;

