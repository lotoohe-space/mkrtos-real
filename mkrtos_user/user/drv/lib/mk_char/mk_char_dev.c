

#include "mk_char_dev.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
static mk_char_dev_t mk_char_dev_list[MK_CHAR_DEV_NUM];
int mk_char_dev_register(mk_dev_t *dev, mk_file_ops_t *ops)
{
    int i;

    for (i = 0; i < MK_CHAR_DEV_NUM; i++)
    {
        if (mk_char_dev_list[i].ops == NULL)
        {
            mk_char_dev_list[i].ops = ops;
            mk_char_dev_list[i].dev = dev;
            dev->ref++;
            return 0;
        }
    }
    return -ENOMEM;
}
mk_char_dev_t *mk_char_get_first(void)
{
    int i;

    for (i = 0; i < MK_CHAR_DEV_NUM; i++)
    {
        if (mk_char_dev_list[i].ops)
        {
            return mk_char_dev_list + i;
        }
    }
    return NULL;
}
mk_char_dev_t *mk_char_get_iter_first(mk_char_dev_iter_t *iter)
{
    int i;
    assert(iter);
    iter->first = NULL;
    iter->cur = NULL;
    iter->cur_inx = 0;

    for (i = 0; i < MK_CHAR_DEV_NUM; i++)
    {
        if (mk_char_dev_list[i].ops)
        {
            iter->first = mk_char_dev_list + i;
            iter->cur = mk_char_dev_list + i;
            iter->cur_inx = i;
            return mk_char_dev_list + i;
        }
    }
    return NULL;
}
mk_char_dev_t *mk_char_get_iter_next(mk_char_dev_iter_t *iter)
{
    int i;

    assert(iter);
    for (i = iter->cur_inx + 1; i < MK_CHAR_DEV_NUM; i++)
    {
        if (mk_char_dev_list[i].ops && i != iter->cur_inx)
        {
            iter->first = mk_char_dev_list + i;
            iter->cur = mk_char_dev_list + i;
            iter->cur_inx = i;
            return mk_char_dev_list + i;
        }
    }
    return NULL;
}
mk_char_dev_t *mk_char_find_char_dev(const char *name)
{
    int i;

    for (i = 0; i < MK_CHAR_DEV_NUM; i++)
    {
        if (mk_char_dev_list[i].ops == NULL)
        {
            continue;
        }
        char *tmp_name;

        tmp_name = mk_dev_get_dev_name(mk_char_dev_list[i].dev);
        if (strcmp(tmp_name, name) == 0)
        {
            return mk_char_dev_list + i;
        }
    }
    return NULL;
}
