#include "uvmm_dev_man.h"
#include "guest_os.h"
#include "uvmm_devtree.h"
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdio.h>
#define UVMM_SIMUL_DEVS_LEN 256

extern char *_dev_simul_start[];
extern char *_dev_simul_end[];

static vmm_simul_t *uvmm_simul_devs[UVMM_SIMUL_DEVS_LEN]; // 模拟设备的函数列表

void uvmm_dev_simul_init(void)
{
    umword_t start_addr = (umword_t)&_dev_simul_start;
    umword_t end_addr = (umword_t)&_dev_simul_end;
    uvmm_dev_simul_struct_t *dev_simul = (uvmm_dev_simul_struct_t *)start_addr;

    for (; dev_simul < (uvmm_dev_simul_struct_t *)end_addr; dev_simul++)
    {
        if (dev_simul->_init)
        {
            if (dev_simul->_init() < 0)
            {
                printf("%s init fail.\n", dev_simul->init_func_name);
            }
        }
    }
}

int uvmm_register_emulator_dev(vmm_simul_t *dev_operation)
{
    for (int i = 0; i < UVMM_SIMUL_DEVS_LEN; i++)
    {
        if (uvmm_simul_devs[i] == NULL)
        {
            uvmm_simul_devs[i] = dev_operation;
            return 0;
        }
    }
    return -1;
}
void uvmm_unregister_emulator_dev(vmm_simul_t *dev_operation)
{
    /*TODO:*/
    assert(0);
}

int uvmm_devs_match(guest_os_t *guest, vmm_devtree_node_t *devtree_node)
{
    int ret = -1;
    assert(devtree_node);
    vmm_dev_t *simul_dev;

    for (int i = 0; i < UVMM_SIMUL_DEVS_LEN; i++)
    {
        if (!uvmm_simul_devs[i])
        {
            continue;
        }
        vmm_devtree_id_t *devtree_id = uvmm_simul_devs[i]->match_tab;
        if (simul_dev == NULL)
        {
            simul_dev = calloc(1, sizeof(vmm_dev_t));
            if (!simul_dev)
            {
                return -ENOMEM;
            }
        }
        simul_dev->devtree_node = devtree_node;
        for (; devtree_id->compatible[0]; devtree_id++)
        {
            if (strcmp(devtree_id->compatible, devtree_node->compatible) == 0)
            {
                simul_dev->dev_operations = uvmm_simul_devs[i];
                simul_dev->guest = guest;
                // 匹配成功
                if (uvmm_simul_devs[i]->probe(guest, simul_dev, devtree_id) < 0)
                {
                    continue;
                }

                // 设备添加到guest_os
                ret = guest_os_add_dev(guest, simul_dev);
                if (ret < 0)
                {
                    free(simul_dev);
                }
                assert(ret >= 0);
                simul_dev = NULL;
                break;
            }
        }
    }
    return 0;
}
