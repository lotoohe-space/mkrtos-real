
#define HEAP_SIZE  1024*4
#define STACK_SIZE 1024*1

#if defined(__CC_ARM)
#define HEAP_ATTR SECTION("HEAP") __attribute__((zero_init)) 
#define STACK_ATTR SECTION("STACK") __attribute__((zero_init))
#elif defined(__GNUC__)
#define HEAP_ATTR
#define STACK_ATTR
#elif defined(__IAR_SYSTEMS_ICC__)
#define HEAP_ATTR
#define STACK_ATTR
#endif

__attribute__((used)) HEAP_ATTR static char heap[HEAP_SIZE];
__attribute__((used)) STACK_ATTR static char stack[STACK_SIZE];
