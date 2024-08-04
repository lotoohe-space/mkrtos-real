#pragma once

#include "guest_os.h"
#include "uvmm_devtree.h"
#include "u_util.h"
typedef struct uvmm_dev_simul_struct
{
    const char *author;
    const char *descr;
    const char *init_func_name;
    const char *exit_func_name;
    int (*_init)(void);
    void (*_exit)(void);
} uvmm_dev_simul_struct_t;

#define UVMM_DECLARE_MODULE(_descr, _author, prio, __init, __exit)                                        \
    static USED SECTION(".dev_simul_section." #prio) uvmm_dev_simul_struct_t dev_simul_##__init##__exit = { \
        .author = _author,                                                                                \
        .descr = _descr,                                                                                  \
        ._init = __init,                                                                                  \
        ._exit = __exit,                                                                                  \
        .init_func_name = #__init,                                                                        \
        .exit_func_name = #__exit,                                                                        \
    };

void uvmm_dev_simul_init(void);
int uvmm_devs_match(guest_os_t *guest, vmm_devtree_node_t *devtree_node);
int uvmm_register_emulator_dev(vmm_simul_t *dev_operation);
void uvmm_unregister_emulator_dev(vmm_simul_t *dev_operation);
