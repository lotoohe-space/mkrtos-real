#include <u_types.h>

#define HEAP_SIZE (10*1024)
#define STACK_SIZE (1024 * 2)

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
__attribute__((used)) umword_t _____mm_bitmap_____[ROUND(HEAP_SIZE, MK_PAGE_SIZE) / (sizeof(umword_t) * 8) + 1];
