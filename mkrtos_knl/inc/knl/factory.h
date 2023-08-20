#pragma once

#include "ram_limit.h"
#include "kobject.h"
typedef struct factory
{
    kobject_t kobj;
    ram_limit_t limit;
} factory_t;

typedef kobject_t *(*factory_func)(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                   umword_t arg2, umword_t arg3);

void factory_register(factory_func func, int inx);
factory_t *root_factory_get(void);
