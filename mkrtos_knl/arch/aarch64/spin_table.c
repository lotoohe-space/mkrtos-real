

// #include <config.h>
#include <mmu.h>
#include <config.h>
#include <util.h>
#include <arch.h>
#define DATA_BOOT_SECTION ".data.boot"
#define TEXT_BOOT_SECTION ".text.boot"

extern mword_t cpu_jump_addr[SYS_CPU_NUM];
extern mword_t per_cpu_stack_addr[SYS_CPU_NUM];

// 每个cpu的栈指针
SECTION(DATA_BOOT_SECTION)
__ALIGN__(PAGE_SIZE)
mword_t per_cpu_stack[SYS_CPU_NUM][THREAD_BLOCK_SIZE / MWORD_BYTES];

void *get_cpu_stack(int cpu_inx)
{
    return (void *)per_cpu_stack[cpu_inx];
}

static inline SECTION(TEXT_BOOT_SECTION) uint64_t get_st_currentel(void)
{
    unsigned long _val;

    asm volatile("mrs %0, CurrentEL"
                 : "=r"(_val));
    return _val;
}
static SECTION(TEXT_BOOT_SECTION) inline int boot_get_current_cpu_id(void)
{
    return read_sysreg(mpidr_el1) & 0XFFUL;
}

extern void cpu_kernel_init(void);
void SECTION(TEXT_BOOT_SECTION) other_cpu_boot(void)
{
    // int cpu_id;

    // cpu_id = boot_get_current_cpu_id();
    // per_cpu_boot_mapping(&per_cpu_pkdir[cpu_id - 1]);
    per_cpu_boot_mapping(FALSE);

    mword_t elx = get_st_currentel();

    // kprint("cpuid %d run el%d.\n", cpu_id, elx);
    cpu_kernel_init();
    while (1)
        ;
}
void SECTION(TEXT_BOOT_SECTION) cpu_start_to(int cpu_id)
{
    // knl_pdir_init(&per_cpu_pkdir[cpu_id - 1], per_cpu_boot_kpdir[cpu_id - 1]);
    per_cpu_stack_addr[cpu_id] = ((addr_t)per_cpu_stack[cpu_id]) + THREAD_BLOCK_SIZE - MWORD_BYTES - PT_REGS_SIZE;
    cpu_jump_addr[cpu_id] = (mword_t)other_cpu_boot;
}
