#pragma once
#include <mk_dev.h>
#include <u_types.h>
#include <sys/types.h>
#define MK_CHAR_DEV_NUM 4

typedef struct mk_file_ops
{
    int (*open)(mk_dev_t *dev);
    int (*write)(mk_dev_t *dev, void *buf, size_t size, off_t *offset);
    int (*read)(mk_dev_t *dev, void *buf, size_t size, off_t *offset);
    int (*ioctl)(mk_dev_t *dev, int cmd, umword_t args);
    int (*release)(mk_dev_t *dev);
} mk_file_ops_t;

typedef struct mk_char_dev
{
    mk_file_ops_t *ops;
    mk_dev_t *dev;
} mk_char_dev_t;

typedef struct mk_char_dev_iter
{
    mk_char_dev_t *first;
    mk_char_dev_t *cur;
    int cur_inx;
} mk_char_dev_iter_t;

int mk_char_dev_register(mk_dev_t *dev, mk_file_ops_t *ops);
mk_char_dev_t *mk_char_find_char_dev(const char *name);
mk_char_dev_t *mk_char_get_first(void);
mk_char_dev_t *mk_char_get_iter_first(mk_char_dev_iter_t *iter);
mk_char_dev_t *mk_char_get_iter_next(mk_char_dev_iter_t *iter);
