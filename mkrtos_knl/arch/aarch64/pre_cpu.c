
#include <pre_cpu.h>
#include <types.h>
#include <buddy.h>
#include <config.h>
#include <string.h>
#include <arch.h>
#include <assert.h>
#include <asm_config.h>
#if CONFIG_SMP
extern char _pre_cpu_data_start[];
extern char _pre_cpu_data_end[];

static void *pre_cpu_data;
static mword_t pre_cpu_data_size;

void pre_cpu_init(void)
{
    mword_t data_size = (addr_t)_pre_cpu_data_end - (addr_t)_pre_cpu_data_start;
    pre_cpu_data_size = ALIGN(data_size, MWORD_BYTES);

    printk("pre_cpu data's size is %d bytes.\n", data_size);\
    printk("pre_cpu data's alloc size is %d bytes.\n", pre_cpu_data_size);

    pre_cpu_data = buddy_alloc(buddy_get_alloter(), 
        pre_cpu_data_size * SYS_CPU_NUM);
    assert(pre_cpu_data);
    memset(pre_cpu_data, 0, pre_cpu_data_size * SYS_CPU_NUM);
}
#else
void pre_cpu_init(void)
{
}
#endif

void *pre_cpu_get_var_cpu(int cpu_inx, void *mem_addr)
{
    #if CONFIG_SMP
    mword_t offset = (addr_t)mem_addr - (addr_t)_pre_cpu_data_start;

    return (void *)((addr_t)pre_cpu_data + 
        offset + cpu_inx * pre_cpu_data_size);
    #else
    return mem_addr;
    #endif
}
void *pre_cpu_get_current_cpu_var(void *mem_addr)
{
    return pre_cpu_get_var_cpu(arch_get_current_cpu_id(), mem_addr);
}
