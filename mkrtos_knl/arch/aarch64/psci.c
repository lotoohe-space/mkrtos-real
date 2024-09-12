
#include <types.h>
#include <printk.h>
#include <psci.h>
enum
{
    PSCI_FUNC_VERSION = 0,
    PSCI_FUNC_CPU_SUSPEND,
    PSCI_FUNC_CPU_OFF,
    PSCI_FUNC_CPU_ON,
    PSCI_FUNC_AFFINITY_INFO,
    PSCI_FUNC_MIGRATE,
    PSCI_FUNC_MIGRATE_INFO_TYPE,
    PSCI_FUNC_MIGRATE_INFO_UP_CPU,
    PSCI_FUNC_SYSTEM_OFF,
    PSCI_FUNC_SYSTEM_RESET,
    PSCI_FUNC_FEATURES,
    PSCI_FUNC_CPU_FREEZE,
    PSCI_FUNC_CPU_DEFAULT_SUSPEND,
    PSCI_FUNC_NODE_HW_STATE,
    PSCI_FUNC_SYSTEM_SUSPEND,
    PSCI_FUNC_SET_SUSPEND_MODE,
    PSCI_FUNC_STAT_RESIDENCY,
    PSCI_FUNC_STAT_COUNT,

    PSCI_FUNC_BASE_SMC32 = 0x84000000,
    PSCI_FUNC_BASE_SMC64 = 0xC4000000,
};
static int v1;
static umword_t psci_func(umword_t func_id)
{
    switch (func_id)
    {
    case PSCI_FUNC_VERSION:
    case PSCI_FUNC_CPU_OFF:
    case PSCI_FUNC_MIGRATE_INFO_TYPE:
    case PSCI_FUNC_SYSTEM_OFF:
    case PSCI_FUNC_SYSTEM_RESET:
    case PSCI_FUNC_FEATURES:
    case PSCI_FUNC_CPU_FREEZE:
    case PSCI_FUNC_SET_SUSPEND_MODE:
        return PSCI_FUNC_BASE_SMC32 + func_id;
    default:
        return ((sizeof(long) == 8) ? PSCI_FUNC_BASE_SMC64 : PSCI_FUNC_BASE_SMC32) + func_id;
    }
}

 __attribute__((optimize(0))) static inline void psci_call(
    umword_t func_id,
    umword_t res[4],
    umword_t arg0,
    umword_t arg1,
    umword_t arg2,
    umword_t arg3,
    umword_t arg4,
    umword_t arg5,
    umword_t arg6)
{
    register umword_t r0 asm("x0") = psci_func(func_id);
    register umword_t r1 asm("x1") = arg0;
    register umword_t r2 asm("x2") = arg1;
    register umword_t r3 asm("x3") = arg2;
    register umword_t r4 asm("x4") = arg3;
    register umword_t r5 asm("x5") = arg4;
    register umword_t r6 asm("x6") = arg5;
    register umword_t r7 asm("x7") = arg6;

    // asm volatile("smc #0");
    arm_smc(r0, r1, r2, r3, r4, r5, r6, r7);

    res[0] = r0;
    res[1] = r1;
    res[2] = r2;
    res[3] = r3;
}
void psci_init(void)
{
    umword_t res[4];

    printk("psci detecting..\n");
    psci_call(
        PSCI_FUNC_VERSION, res, 0, 0, 0,
        0, 0, 0, 0);
    v1 = (res[0] >> 16) >= 1;

    if (v1)
    {
        psci_call(PSCI_FUNC_FEATURES, res,
                  psci_func(PSCI_FUNC_CPU_SUSPEND),
                  0, 0, 0, 0, 0, 0);
        if (res[0] & (1UL << 31))
        {
            printk("psci CPU_SUSPEND not supported (%d)\n", (int)res[0]);
        }
        else
        {
            printk("psci CPU_SUSPEND format %s, %s support OS-initiated mode\n",
                   res[0] & 0x2 ? "extended" : "original v0.2",
                   res[0] & 0x1 ? "" : "not");
        }
    }
    psci_call(PSCI_FUNC_MIGRATE_INFO_TYPE, res,
              0, 0, 0, 0, 0, 0, 0);
    if (res[0] == 0 || res[0] == 1)
    {
        printk("psci TOS:single core, %s migration capable\n",
               res[0] ? "not " : "");
    }
    else
    {
        printk("psci TOS:not present or not required\n");
    }
}
umword_t psci_cpu_on(umword_t cpuid, umword_t entry_point)
{
    umword_t res[4];

    psci_call(PSCI_FUNC_CPU_ON, res, cpuid, entry_point,
              0, 0, 0, 0, 0);
    return res[0];
}
void psci_system_reset(void)
{
    umword_t res[4];

    psci_call(PSCI_FUNC_SYSTEM_RESET, res,
              0, 0, 0, 0, 0, 0, 0);
    printk("psci system-reset failed.\n");
}
void psic_system_off(void)
{
    umword_t res[4];

    psci_call(PSCI_FUNC_SYSTEM_OFF, res,
              0, 0, 0, 0, 0, 0, 0);
    printk("psci system_off failed.\n");
}
