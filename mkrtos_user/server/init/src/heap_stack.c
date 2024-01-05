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
#define HEAP_SIZE ((1024 + 256) * 3 + 1024)
#define STACK_SIZE (1024 + 256)

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

__attribute__((used)) HEAP_ATTR static char heap[HEAP_SIZE];
__attribute__((used)) STACK_ATTR static char stack[STACK_SIZE];
