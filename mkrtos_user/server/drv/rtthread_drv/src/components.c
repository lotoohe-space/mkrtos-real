/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-09-20     Bernard      Change the name to components.c
 *                             And all components related header files.
 * 2012-12-23     Bernard      fix the pthread initialization issue.
 * 2013-06-23     Bernard      Add the init_call for components initialization.
 * 2013-07-05     Bernard      Remove initialization feature for MS VC++ compiler
 * 2015-02-06     Bernard      Remove the MS VC++ support and move to the kernel
 * 2015-05-04     Bernard      Rename it to components.c because compiling issue
 *                             in some IDEs.
 * 2015-07-29     Arda.Fu      Add support to use RT_USING_USER_MAIN with IAR
 * 2018-11-22     Jesven       Add secondary cpu boot up
 * 2023-09-15     xqyjlj       perf rt_hw_interrupt_disable/enable
 */

#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_USER_MAIN
#ifndef RT_MAIN_THREAD_STACK_SIZE
#define RT_MAIN_THREAD_STACK_SIZE 2048
#endif /* RT_MAIN_THREAD_STACK_SIZE */
#ifndef RT_MAIN_THREAD_PRIORITY
#define RT_MAIN_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX / 3)
#endif /* RT_MAIN_THREAD_PRIORITY */
#endif /* RT_USING_USER_MAIN */

#ifdef RT_USING_COMPONENTS_INIT
/*
 * Components Initialization will initialize some driver and components as following
 * order:
 * rti_start         --> 0
 * BOARD_EXPORT      --> 1
 * rti_board_end     --> 1.end
 *
 * DEVICE_EXPORT     --> 2
 * COMPONENT_EXPORT  --> 3
 * FS_EXPORT         --> 4
 * ENV_EXPORT        --> 5
 * APP_EXPORT        --> 6
 *
 * rti_end           --> 6.end
 *
 * These automatically initialization, the driver or component initial function must
 * be defined with:
 * INIT_BOARD_EXPORT(fn);
 * INIT_DEVICE_EXPORT(fn);
 * ...
 * INIT_APP_EXPORT(fn);
 * etc.
 */
static int rti_start(void)
{
    return 0;
}
INIT_EXPORT(rti_start, "0");

static int rti_board_start(void)
{
    return 0;
}
INIT_EXPORT(rti_board_start, "0.end");

static int rti_board_end(void)
{
    return 0;
}
INIT_EXPORT(rti_board_end, "1.end");

static int rti_end(void)
{
    return 0;
}
INIT_EXPORT(rti_end, "6.end");

/**
 * @brief  Onboard components initialization. In this function, the board-level
 *         initialization function will be called to complete the initialization
 *         of the on-board peripherals.
 */
void rt_components_board_init(void)
{
#ifdef RT_DEBUGING_AUTO_INIT
    int result;
    const struct rt_init_desc *desc;
    for (desc = &__rt_init_desc_rti_board_start; desc < &__rt_init_desc_rti_board_end; desc++)
    {
        rt_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        rt_kprintf(":%d done\n", result);
    }
#else
#ifdef MKRTOS
    extern void *app_start_addr;
    unsigned long start_addr = ((unsigned long)app_start_addr) & (~3UL);
#endif
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__rt_init_rti_board_start; fn_ptr < &__rt_init_rti_board_end; fn_ptr++)
    {
#ifdef MKRTOS
        unsigned long fn_addr = (((unsigned long)(*fn_ptr) + (unsigned long)start_addr) | 0x1UL);
        ((init_fn_t)(fn_addr))();
#else
        (*fn_ptr)();
#endif
    }
#endif /* RT_DEBUGING_AUTO_INIT */
}

/**
 * @brief  RT-Thread Components Initialization.
 */
void rt_components_init(void)
{
#ifdef RT_DEBUGING_AUTO_INIT
    int result;
    const struct rt_init_desc *desc;

    rt_kprintf("do components initialization.\n");
    for (desc = &__rt_init_desc_rti_board_end; desc < &__rt_init_desc_rti_end; desc++)
    {
        rt_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        rt_kprintf(":%d done\n", result);
    }
#else
#ifdef MKRTOS
    extern void *app_start_addr;
    unsigned long start_addr = ((unsigned long)app_start_addr) & (~3UL);
#endif
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__rt_init_rti_board_end; fn_ptr < &__rt_init_rti_end; fn_ptr++)
    {
#ifdef MKRTOS
        unsigned long fn_addr = (((unsigned long)(*fn_ptr) + (unsigned long)start_addr) | 0x1UL);
        ((init_fn_t)(fn_addr))();
#else
        (*fn_ptr)();
#endif
    }
#endif /* RT_DEBUGING_AUTO_INIT */
}
#endif /* RT_USING_COMPONENTS_INIT */