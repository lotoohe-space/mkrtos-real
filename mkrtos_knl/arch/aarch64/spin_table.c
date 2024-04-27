

// #include <config.h>
#include <mmu.h>
#include <config.h>
#include <util.h>
#include <arch.h>
#define DATA_BOOT_SECTION ".data.boot"
#define TEXT_BOOT_SECTION ".text.boot"

extern mword_t cpu_jump_addr[SYS_CPU_NUM];
extern mword_t per_cpu_stack_addr[SYS_CPU_NUM];

void SECTION(TEXT_BOOT_SECTION) cpu_start_to(int cpu_id, void *stack, void *(fn)(void))
{
    per_cpu_stack_addr[cpu_id] = ((addr_t)stack);
    cpu_jump_addr[cpu_id] = (mword_t)fn;
}
