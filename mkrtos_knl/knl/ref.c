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
