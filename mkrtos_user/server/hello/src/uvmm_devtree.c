
#include "uvmm_devtree.h"
#include "guest_os.h"
#include "uvmm_dev_man.h"
#include "u_vmam.h"
#include <u_types.h>
#include <cJSON.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
static cJSON *root_cjson;
int uvmm_devtree_scan(guest_os_t *guest, const char *data)
{
    cJSON *cjson;
    cJSON *guest_os_name;
    int ret = 0;

    cjson = cJSON_Parse(data);
    if (cjson == NULL)
    {
        return -EINVAL;
    }
    guest_os_name = cJSON_GetObjectItem(cjson, "guest_os_name");
    if (guest_os_name)
    {
        strlcpy(guest->name, guest_os_name->valuestring, GUEST_OS_NAME_MAX_LEN);
    }
    else
    {
        strlcpy(guest->name, "guest os", GUEST_OS_NAME_MAX_LEN);
    }
    cJSON *simul_tab = cJSON_GetObjectItem(cjson, "simul_dev");
    if (simul_tab == NULL)
    {
        ret = -ENOENT;
        goto end;
    }
    int simul_tab_size = cJSON_GetArraySize(simul_tab);

    for (int i = 0; i < simul_tab_size; i++)
    {
        cJSON *dev_node = cJSON_GetArrayItem(simul_tab, i);
        if (!dev_node)
        {
            break;
        }
        cJSON *compat = cJSON_GetObjectItem(dev_node, "compatible");
        if (!compat)
        {
            break;
        }
        cJSON *reg_addr_node = cJSON_GetObjectItem(dev_node, "reg_addr");
        cJSON *reg_size_node = cJSON_GetObjectItem(dev_node, "reg_size");

        vmm_devtree_node_t *devtree_node = calloc(1, sizeof(vmm_devtree_node_t));

        if (!devtree_node)
        {
            ret = -ENOMEM;
            goto end;
        }
        if (reg_addr_node && reg_size_node)
        {
            umword_t reg_addr;
            umword_t reg_size;
            char *endptr;

            reg_addr = strtol(reg_addr_node->valuestring, &endptr, 0);
            reg_size = strtol(reg_size_node->valuestring, &endptr, 0);

            msg_tag_t tag = u_vmam_alloc(VMA_PROT,
                                         vma_addr_create(VPAGE_PROT_N_ACCESS_FLAG | VPAGE_PROT_UNCACHE,
                                                         VMA_ADDR_RESV, reg_addr),
                                         reg_size, reg_addr, NULL);
            if (msg_tag_get_val(tag) < 0)
            {
                printf("%s [0x%x - 0x%x] reg map fail", compat->valuestring, reg_addr, reg_addr + reg_size - 1);
                ret = -EFAULT;
                free(devtree_node);
                goto end;
            }
            devtree_node->addr = reg_addr;
            devtree_node->size = reg_size;
        }
        strlcpy(devtree_node->compatible, compat->valuestring, VMM_FIELD_COMPAT_SIZE);
        devtree_node->json_node = dev_node;
        int match_ret;

        match_ret = uvmm_devs_match(guest, devtree_node);
        if (match_ret)
        {
            printf("%s device match error.\n", compat->valuestring);
            free(devtree_node);
            goto end;
        }
    }
    root_cjson = cjson;
    goto end_ok;
end:
    cJSON_free(cjson);
end_ok:
    return 0;
}
