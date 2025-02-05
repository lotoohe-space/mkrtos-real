#include "u_types.h"

#define APP_MAGIC "MKRTOS."

typedef struct app_info
{
    const char d[32];
    const char magic[8];
    union
    {
        struct exec_head_info
        {
            unsigned long ram_size;
            unsigned long heap_offset;
            unsigned long stack_offset;
            unsigned long heap_size;
            unsigned long stack_size;
            unsigned long data_offset;
            unsigned long bss_offset;
            unsigned long got_start;
            unsigned long got_end;
            unsigned long rel_start;
            unsigned long rel_end;
            unsigned long text_start;
            unsigned long data_start;
            unsigned long dyn_start;
        } i;
        // const char d1[256];
    };
    const char dot_text[];
} app_info_t;

static inline app_info_t *app_info_get(void *addr)
{
    app_info_t *app = (app_info_t *)((unsigned long)addr & (~0x3UL));
    const char *magic = APP_MAGIC;
    for (int i = 0; i < sizeof(app->magic) - 1; i++)
    {
        if (app->magic[i] != magic[i])
        {
            return NULL;
        }
    }
    return app;
}
