/**
 * @file ref.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ref.h"
#include <assert.h>
int ref_counter_dec_and_release(ref_counter_t *ref, kobject_t *kobj)
{
    assert(kobj);
    int ret = ref_counter_dec(ref);

    if (ret == 1)
    {
        if (kobj->stage_2_func)
        {
            kobj->stage_2_func(kobj);
        }
    }
    return ret;
}
