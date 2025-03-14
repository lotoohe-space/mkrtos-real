
#include <string.h>
#include <assert.h>
#include "appfs_tiny.h"
/**
 * 通过文件名查找文件
 */
const dir_info_t *appfs_find_file_by_name(fs_info_t *info, const char *name)
{
    assert(info);
    assert(name);

    int ret = 0;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        if (info->save.dirinfo_inx + i >= info->save.block_nr)
        {
            return NULL;
        }
        const void *buf = (void *)((info->save.dirinfo_inx + i) * info->save.block_size + (unsigned long)info);

        if (ret < 0)
        {
            return NULL;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            const dir_info_t *dir_info = &((dir_info_t *)buf)[j];
            if (strcmp(dir_info->name, name) == 0)
            {
                return (const dir_info_t *)((char *)info + (info->save.dirinfo_inx + i) * info->save.block_size + j * sizeof(dir_info_t));
            }
        }
    }

    return NULL;
}
fs_info_t *appfs_get_form_addr(void *addr)
{
    assert(addr);
    fs_info_t *fs = addr;
    const char *magic_str = APPFS_MAGIC;

    for (int i = 0; magic_str[i]; i++)
    {
        if (fs->save.magic[0] != magic_str[0])
        {
            return NULL;
        }
    }
    return fs;
}
umword_t appfs_find_file_addr_by_name(fs_info_t *info, const char *name, size_t *size)
{
    const dir_info_t *dir_info;

    dir_info = appfs_find_file_by_name(info, name);

    if (dir_info == NULL)
    {
        return 0;
    }
    if (size)
    {
        *size = dir_info->size;
    }
    return (umword_t)appfs_get_file_addr(info, dir_info);
}