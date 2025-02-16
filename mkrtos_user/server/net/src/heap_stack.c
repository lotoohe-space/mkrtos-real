#include <u_util.h>
#if !IS_ENABLED(CONFIG_MMU)
#define HEAP_SIZE (16 * 1024)
#define STACK_SIZE (2048)

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