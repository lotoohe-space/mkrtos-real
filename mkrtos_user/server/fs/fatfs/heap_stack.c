#include <u_util.h>
#include <u_types.h>


#define HEAP_SIZE 512
#define STACK_SIZE 1024 * 2

#define HEAP_ATTR __attribute__((__section__(".bss.heap")))
#define STACK_ATTR __attribute__((__section__(".bss.stack")))

__attribute__((used)) HEAP_ATTR static char _____heap_____[HEAP_SIZE];
__attribute__((used)) STACK_ATTR static char _____stack_____[STACK_SIZE];
__attribute__((used)) umword_t _____mm_bitmap_____[ROUND(HEAP_SIZE, MK_PAGE_SIZE) / (sizeof(umword_t) * 8) + 1];
