#include <u_util.h>
#if !IS_ENABLED(CONFIG_MMU)
/**
 * @file heap_stack.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifdef MKRTOS_TEST_MODE
#define HEAP_SIZE (256 * 1024)
#define STACK_SIZE (2 * 1024) //(1024 + 256)
#else
#define HEAP_SIZE (2 * 1024)
#define STACK_SIZE (4 * 1024) //(1024 + 256)
#endif

#if defined(__CC_ARM)
#define HEAP_ATTR SECTION("HEAP") __attribute__((zero_init))
#define STACK_ATTR SECTION("STACK") __attribute__((zero_init))
#elif defined(__GNUC__)
#define HEAP_ATTR __attribute__((__section__(".bss.heap")))
#define STACK_ATTR __attribute__((__section__(".bss.stack")))
#elif defined(__IAR_SYSTEMS_ICC__)
#define HEAP_ATTR
#define STACK_ATTR
#endif

__attribute__((used)) HEAP_ATTR static char _____heap_____[HEAP_SIZE];
__attribute__((used)) STACK_ATTR static char _____stack_____[STACK_SIZE];
#endif
