
#include <u_types.h>
#include <string.h>
#include "u_hd_man.h"
#define NAMESPACE_PATH_LEN 32
#define NAMESAPCE_NR 32

typedef struct namespace_entry
{
    char path[NAMESPACE_PATH_LEN];
    obj_handler_t hd;
} namespace_entry_t;

typedef struct namespace
{
    namespace_entry_t ne_list[NAMESAPCE_NR];
}
namespace_t;

static namespace_t ns;

static int ns_alloc(const char *path, obj_handler_t hd)
{
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        if (ns.ne_list[i].hd == HANDLER_INVALID)
        {
            ns.ne_list[i].hd = hd;
            strncpy(ns.ne_list[i].path, path, NAMESPACE_PATH_LEN);
            ns.ne_list[i].path[NAMESPACE_PATH_LEN - 1] = 0;
            return i;
        }
    }
    return -1;
}

int ns_register(const char *path, obj_handler_t hd)
{
    if (ns_alloc(path, hd) < 0)
    {
        return -1;
    }
    return 0;
}
int ns_query(const char *path, obj_handler_t *hd)
{
    for (int i = 0; i < NAMESAPCE_NR; i++)
    {
        if (ns.ne_list[i].hd == HANDLER_INVALID)
        {
            if (strncmp(ns.ne_list[i].path, path, NAMESPACE_PATH_LEN) == 0)
            {
                *hd = ns.ne_list[i].hd;
                return 0;
            }
        }
    }
    return -1;
}
