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
            umword_t ram_size;
            umword_t heap_offset;
            umword_t stack_offset;
            umword_t heap_size;
            umword_t stack_size;
            umword_t data_offset;
            umword_t bss_offset;
            umword_t got_start;
            umword_t got_end;
            umword_t rel_start;
            umword_t rel_end;
            umword_t text_start;
        } i;
        const char d1[128];
    };
    const char dot_text[];
} app_info_t;

static inline app_info_t *app_info_get(void *addr)
{
    app_info_t *app = (app_info_t *)((umword_t)addr & (~0x3UL));
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
