#include "types.h"
#include "app.h"

app_info_t *app_info_get(void *addr)
{
    return (app_info_t *)addr;
}
