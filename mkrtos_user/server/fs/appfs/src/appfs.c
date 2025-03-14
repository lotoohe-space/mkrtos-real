#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "appfs.h"

#define BYPTE_PER_PTR (sizeof(uint32_t)) //!< 先统一4字节处理方式
#define BITS_PER_BYTE (8)
#define BITS_PER_PTR (sizeof(uint32_t) * BITS_PER_BYTE)

int appfs_init(fs_info_t *fs)
{
    int ret;
    assert(fs);
    fs->cb.hw_init_fs_for_block(fs);
    fs_info_t *fs_info = (fs_info_t *)fs->mem_addr;

    // ret = fs->cb.hw_read_block(fs, 0, fs->buf, fs->save.block_size);
    // if (ret >= 0)
    // {
    //     memcpy(fs, fs->buf, sizeof(fs_info_t));
    // }
    fs->save = fs_info->save;
    return ret;
}
static int mem_set_bits(uint32_t *buf, int start, int end)
{
    assert(buf);
    assert(start >= 0);
    assert(end >= 0);
    assert(start <= end);

    for (int i = start; i <= end; i++)
    {
        int block_inx = i / (BITS_PER_PTR);
        int block_bit = i % (BITS_PER_PTR);
        buf[block_inx] |= (1UL << block_bit);
    }

    return 0;
}
static int mem_clear_bits(uint32_t *buf, int start, int end)
{
    assert(buf);
    assert(start >= 0);
    assert(end >= 0);
    assert(start <= end);

    for (int i = start; i <= end; i++)
    {
        int block_inx = i / (BITS_PER_PTR);
        int block_bit = i % (BITS_PER_PTR);
        buf[block_inx] &= ~(1UL << block_bit);
    }

    return 0;
}
static int appfs_get_available_block_nr(fs_info_t *info)
{
    assert(info);
    int ret;
    int j = 0;
    int block_nr = 0;
    int total_nr = 0;

    for (int i = info->save.blockinfo_inx; i < (info->save.blockinfo_nr + info->save.blockinfo_inx) && j < info->save.block_nr; i++, j++)
    {
        ret = info->cb.hw_read_block(info, i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        uint32_t *buf_ptr = (uint32_t *)info->buf;
        for (int a = 0; a < info->save.block_size / BYPTE_PER_PTR; a++)
        {
            for (int m = 0; m < BITS_PER_PTR; m++)
            {
                total_nr++;
                if ((buf_ptr[a] & (1UL << m)))
                {
                    block_nr++;
                }
                if (total_nr >= info->save.block_nr)
                {
                    goto end;
                }
            }
        }
    }
end:
    return block_nr;
}
int appfs_get_available_size(fs_info_t *info)
{
    assert(info);

    return appfs_get_available_block_nr(info) * info->save.block_size;
}
/**
 * 设置块中的某些位为1
 */
static int appfs_blockinfo_bits_set(fs_info_t *info, int st, int end, int is_set)
{
    assert(info);
    int block_inx = -1;
    int block_last_inx = -1;
    int ret = 0;
    for (int i = st; i <= end; i++)
    {
        block_inx = i / (info->save.block_size / BYPTE_PER_PTR) / BITS_PER_PTR;

        if (block_last_inx != -1 && block_last_inx != block_inx)
        {
            ret = info->cb.hw_write_and_erase_block(info, info->save.blockinfo_inx + block_last_inx, info->buf, info->save.block_size);
            if (ret < 0)
            {
                return ret;
            }
        }

        int block_bit = i % (info->save.block_size * BITS_PER_BYTE);

        if (block_last_inx == -1 || block_last_inx != block_inx)
        {
            ret = info->cb.hw_read_block(info, info->save.blockinfo_inx + block_inx, info->buf, info->save.block_size);
            if (ret < 0)
            {
                return ret;
            }
        }
        if (is_set)
        {
            info->buf[block_bit / BITS_PER_PTR] |= (1UL << (block_bit % BITS_PER_PTR));
        }
        else
        {
            info->buf[block_bit / BITS_PER_PTR] &= ~(1UL << (block_bit % BITS_PER_PTR));
        }
        block_last_inx = block_inx;
    }
    if (end - st >= 0)
    {
        // 更新最后一个块
        ret = info->cb.hw_write_and_erase_block(info, info->save.blockinfo_inx + block_last_inx, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}

/**
 * @brief 格式化文件系统
 *
 */
int appfs_format(fs_info_t *fs, int file_nr)
{
    assert(fs);
    assert(file_nr > 0);

    int ret = 0;
    fs->cb.hw_init_fs_for_block(fs);

    // 擦除所有块
    for (int i = 0; i < fs->save.block_nr; i++)
    {
        ret = fs->cb.hw_erase_block(fs, i);
        if (ret < 0)
        {
            return ret;
        }
    }
    strcpy(fs->save.magic, APPFS_MAGIC);
    fs->save.version = APPFS_VERSION;
    // 初始化文件系统信息
    fs->save.blockinfo_inx = 1;
    fs->save.blockinfo_nr = ROUND_UP(fs->save.block_nr, fs->save.block_size * 8);

    fs->save.dirinfo_inx = fs->save.blockinfo_inx + fs->save.blockinfo_nr;
    fs->save.dirinfo_nr = ROUND_UP(file_nr * sizeof(dir_info_t), fs->save.block_size);

    memset((fs_info_t *)fs->mem_addr, 0, fs->save.block_size);
    for (int i = fs->save.dirinfo_inx; i < fs->save.dirinfo_inx + fs->save.dirinfo_nr; i++)
    {
        ret = fs->cb.hw_write_and_erase_block(fs, i, fs->buf, fs->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
    }
    ret = appfs_blockinfo_bits_set(fs, 0, fs->save.blockinfo_nr + fs->save.dirinfo_nr + 1, 0);
    if (ret < 0)
    {
        return ret;
    }

    ret = fs->cb.hw_write_and_erase_block(fs, 0, fs, MIN(sizeof(fs_info_t), fs->save.block_size));
    return ret;
}
/**
 * @brief 查找连续的count个块
 *
 * @param info
 * @param st
 * @param end
 * @param count
 * @return int
 */
static inline int calculate_block_index(int i, int j, int m, fs_info_t *info)
{
    return i * (info->save.block_size * BITS_PER_BYTE) + j * BITS_PER_PTR + m;
}

/**
 * @brief 查找连续的count个块并设置为已使用
 * 1. 从blockinfo_inx开始查找
 * 2. 查找到count个连续的块
 * 3. 设置这些块为已使用
 * 4. 返回起始块号和结束块号
 * @param info
 * @param st
 * @param end
 * @param count
 *
 */
int appfs_find_set_blocks(fs_info_t *info, int *st, int *end, int count)
{
    assert(st);
    assert(end);
    assert(info);
    int ret = 0;
    int state_flag = 0; //!< 0:未找到 1:找到起始块 2:找到结束块
    int count_cur = 0;  //!< 当前计数

    for (int i = 0; i < info->save.blockinfo_nr; i++)
    {
        ret = info->cb.hw_read_block(info, info->save.blockinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        for (int j = 0; j < info->save.block_size / BYPTE_PER_PTR; j++)
        {
            for (int m = 0; m < BITS_PER_PTR; m++)
            {
                if ((info->buf[j] & (1UL << m)))
                {
                    switch (state_flag)
                    {
                    case 0:
                        *st = calculate_block_index(i, j, m, info); //!< 计算起始块号
                        state_flag = 1;
                        count_cur++;
                        break;
                    case 1:
                        count_cur++;
                        if (count_cur >= count)
                        {
                            *end = calculate_block_index(i, j, m, info); //!< 计算结束块号
                            state_flag = 2;
                            goto __next;
                        }
                        break;
                    }
                }
                else
                {
                    *st = 0;
                    *end = 0;
                    count_cur = 0;
                    state_flag = 0;
                }
            }
        }
    }
    if (count_cur < count)
    {
        printf("appfs mem is low.\n");
        return -1;
    }
__next:
    ret = appfs_blockinfo_bits_set(info, *st, *end, 0);
#if 0
    if (ret >= 0)
    {
        printf("st:%d end %d.\n", *st, *end);
    }
#endif
    return ret;
}
static int appfs_check_file_exist(fs_info_t *info, const char *name)
{
    assert(info);
    assert(name);

    int ret = 0;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (strcmp(dir_info->name, name) == 0)
            {
                return -1;
            }
        }
    }

    return 0;
}
/**
 * @brief 创建文件
 * 1. 检查文件是否存在
 * 2. 查找连续的块并设置为已使用
 * 3. 设置文件信息
 * 4. 更新目录信息
 * @param info
 * @param path
 * @param size
 */
int appfs_create_file(fs_info_t *info, const char *path, int size)
{
    assert(info);
    assert(path);

    int ret = -1;

    ret = appfs_check_file_exist(info, path);
    if (ret < 0)
    {
        return -EEXIST;
    }

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        assert(info->cb.hw_read_block);
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (strcmp(dir_info->name, path) == 0)
            {
                return -1;
            }
            if (dir_info->block_inx == 0)
            {
                int st = 0;
                int end = 0;

                if (size != 0)
                {
                    // 查找连续的块并设置为已使用
                    ret = appfs_find_set_blocks(info, &st, &end, ROUND_UP(size, info->save.block_size));
                    if (ret < 0)
                    {
                        return ret;
                    }
                }
                // 更新目录信息
                ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
                if (ret < 0)
                {
                    return ret;
                }
                // 设置文件信息
                dir_info->block_inx = st == 0 ? -1 : st;
                dir_info->size = size;
                strncpy(dir_info->name, path, APPFS_FILE_NAME_MAX);
                dir_info->name[APPFS_FILE_NAME_MAX - 1] = '\0';

                ret = info->cb.hw_write_and_erase_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
                if (ret < 0)
                {
                    return ret;
                }
                return 0;
            }
        }
    }

    return ret;
}
/**
 * @brief 写文件
 * 1. 查找文件
 * 2. 设置文件块为未使用
 * 3. 更新目录信息
 *
 * @param info
 * @param path
 * @param offset
 * @return int  0:成功 -1:失败
 */
int appfs_write_file(fs_info_t *info, const char *name, void *data, int size, int offset)
{
    assert(info);
    assert(data);

    int ret = 0;
    int write_len = 0;

    const dir_info_t *dir_info = appfs_find_file_by_name(info, name);

    if (!dir_info)
    {
        return -ENOENT;
    }
    // 计算块号
    int block_inx = dir_info->block_inx;
    int block_offset_inx = ROUND_DOWN(offset, info->save.block_size);
    int block_nr = ROUND_UP(offset + size, info->save.block_size);

    block_nr = MIN(block_nr, ROUND_UP(dir_info->size - offset, info->save.block_size));

    for (int k = 0; k < block_nr && write_len < dir_info->size; k++)
    {
        // 写入数据
        int write_size = MIN(size, info->save.block_size - (offset % info->save.block_size));

        write_size = MIN(dir_info->size - offset, write_size);

        if (write_size != info->save.block_size)
        {
            ret = info->cb.hw_read_block(info, block_inx + k + block_offset_inx, info->buf, info->save.block_size);
            if (ret < 0)
            {
                return ret;
            }
        }

        // 从data + k * info->save.block_size拷贝write_size大小的数据到buf
        memcpy((uint8_t *)(info->buf) + (offset % info->save.block_size), data + write_len, write_size);
        // 写入块
        ret = info->cb.hw_write_and_erase_block(info, block_inx + k + block_offset_inx, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        offset = 0;
        write_len += write_size;
        size -= write_size;
    }
    return write_len;
}
int appfs_read_file(fs_info_t *info, const char *name, void *data, int size, int offset)
{
    assert(info);
    assert(data);

    int ret = 0;
    int read_len = 0;

    const dir_info_t *dir_info = appfs_find_file_by_name(info, name);

    if (!dir_info)
    {
        return -ENOENT;
    }
    // 计算块号
    int block_inx = dir_info->block_inx;
    int block_offset_inx = ROUND_DOWN(offset, info->save.block_size);
    int block_nr = ROUND_UP(offset + size, info->save.block_size);

    block_nr = MIN(block_nr, ROUND_UP(dir_info->size - offset, info->save.block_size));

    for (int k = 0; k < block_nr && read_len < dir_info->size; k++)
    {
        // 写入数据
        int read_size = MIN(size, info->save.block_size - (offset % info->save.block_size));

        read_size = MIN(dir_info->size - offset, read_size);
        ret = info->cb.hw_read_block(info, block_inx + k + block_offset_inx, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }

        memcpy(data + read_len, (uint8_t *)(info->buf) + (offset % info->save.block_size), read_size);
        offset = 0;
        read_len += read_size;
        size -= read_size;
    }
    return read_len;
}
static int appfs_write_dirinfo(fs_info_t *info, dir_info_t *in_dir_info)
{
    assert(info);
    assert(in_dir_info);
    int ret = -1;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (strcmp(dir_info->name, in_dir_info->name) == 0)
            {

                *dir_info = *in_dir_info;
                ret = info->cb.hw_write_and_erase_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
                if (ret < 0)
                {
                    return ret;
                }
                ret = 0;
                goto end;
            }
        }
    }
end:
    return ret;
}
/**
 * @brief 删除目录信息
 * 1. 通过文件名查找文件
 * 2. 删除目录信息
 * @param info
 * @param name
 * @return int 0:成功 -1:失败
 */
static int appfs_delete_dirinfo_by_name(fs_info_t *info, const char *name)
{
    assert(info);
    assert(name);

    int ret = 0;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (strcmp(dir_info->name, name) == 0)
            {
                memset(dir_info, 0, sizeof(dir_info_t));
                ret = info->cb.hw_write_and_erase_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
                if (ret < 0)
                {
                    return ret;
                }
                return 0;
            }
        }
    }

    return 0;
}
const dir_info_t *appfs_dir_info_get_next(fs_info_t *info, const dir_info_t *cur)
{
    assert(info);
    assert(cur);
    int ret = 0;

    for (const dir_info_t *cur_dir = (const dir_info_t *)cur + 1;
         (unsigned long)cur_dir < info->mem_addr + (info->save.dirinfo_inx + info->save.dirinfo_nr) * info->save.block_size;
         cur_dir++)
    {
        if (cur_dir->block_inx == 0)
        {
            continue;
        }
        return cur_dir;
    }
    return NULL;
}
const dir_info_t *appfs_dir_info_get_first(fs_info_t *info)
{
    assert(info);
    for (const dir_info_t *cur_dir = (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx * info->save.block_size));
         (unsigned long)cur_dir < info->mem_addr + (info->save.dirinfo_inx + info->save.dirinfo_nr) * info->save.block_size;
         cur_dir++)
    {
        if (cur_dir->block_inx == 0)
        {
            continue;
        }
        return cur_dir;
    }
    return NULL;
}
const dir_info_t *appfs_dir_info_get_index(fs_info_t *info, int index)
{
    assert(info);
    int i = 0;
    for (const dir_info_t *cur_dir = (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx * info->save.block_size));
         (unsigned long)cur_dir < info->mem_addr + (info->save.dirinfo_inx + info->save.dirinfo_nr) * info->save.block_size;
         cur_dir++)
    {
        if (cur_dir->block_inx == 0)
        {
            continue;
        }
        if (i == index)
        {
            return cur_dir;
        }
        i++;
    }
    return NULL;
}
const int appfs_get_dir_info_size(fs_info_t *info)
{
    assert(info);
    int i = 0;
    for (const dir_info_t *cur_dir = (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx * info->save.block_size));
         (unsigned long)cur_dir < info->mem_addr + (info->save.dirinfo_inx + info->save.dirinfo_nr) * info->save.block_size;
         cur_dir++)
    {
        if (cur_dir->block_inx == 0)
        {
            continue;
        }
        i++;
    }
    return i;
}
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
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return NULL;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            const dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (strcmp(dir_info->name, name) == 0)
            {
                return (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx + i) * info->save.block_size + j * sizeof(dir_info_t));
            }
        }
    }

    return NULL;
}
/**
 * 通过尾块查找文件
 * 1. 遍历目录信息
 * 2. 通过尾块查找文件
 * 3. 返回文件信息
 * @param info
 * @param tail_block_inx
 */
static const dir_info_t *appfs_find_file_by_tail_block(fs_info_t *info, int tail_block_inx)
{
    assert(info);

    int ret = 0;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return NULL;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            int file_tail_block_inx = dir_info->block_inx + ROUND_UP(dir_info->size, info->save.block_size);
            if (file_tail_block_inx == tail_block_inx)
            {
                return (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx + i) * info->save.block_size + j * sizeof(dir_info_t));
            }
        }
    }

    return NULL;
}
/**
 * 通过头块查找文件
 */
static const dir_info_t *appfs_find_file_by_head_block(fs_info_t *info, int head_block_inx)
{
    assert(info);

    int ret = 0;

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return NULL;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            const dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (dir_info->block_inx == head_block_inx)
            {
                return (const dir_info_t *)(info->mem_addr + (info->save.dirinfo_inx + i) * info->save.block_size + j * sizeof(dir_info_t));
            }
        }
    }

    return NULL;
}
/**
 * 将文件移动到指定的块上
 */
static const dir_info_t *appfs_move_file_to_block(fs_info_t *info, int file_block_inx, int move_to_block_inx)
{
    assert(info);

    const dir_info_t *dir_info = NULL;
    int ret;

    // printf("move file %d to %d.\n", file_block_inx, move_to_block_inx);

    // 通过尾块查找文件
    dir_info = appfs_find_file_by_head_block(info, file_block_inx);
    if (dir_info == NULL)
    {
        printf("file is not found!\n");
        return NULL;
    }
    for (int i = move_to_block_inx;
         i < move_to_block_inx + ROUND_UP(dir_info->size, info->save.block_size); i++)
    {
        // 读取块
        ret = info->cb.hw_read_block(info, dir_info->block_inx + i - move_to_block_inx,
                                     info->buf, info->save.block_size);
        if (ret < 0)
        {
            /*TODO:错误处理*/
            return NULL;
        }
        // 写入块
        ret = info->cb.hw_write_and_erase_block(info, i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            /*TODO:错误处理*/
            return NULL;
        }
    }
    ret = appfs_blockinfo_bits_set(info, dir_info->block_inx, dir_info->block_inx + ROUND_UP(dir_info->size, info->save.block_size) - 1, 1);
    if (ret < 0)
    {
        return NULL;
    }
    ret = appfs_blockinfo_bits_set(info, move_to_block_inx, move_to_block_inx + ROUND_UP(dir_info->size, info->save.block_size) - 1, 0);
    if (ret < 0)
    {
        return NULL;
    }
    // 更新dirinfo

    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return NULL;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info_tmp = &((dir_info_t *)info->buf)[j];
            if (dir_info_tmp->block_inx == file_block_inx)
            {
                dir_info_tmp->block_inx = move_to_block_inx;
                ret = info->cb.hw_write_and_erase_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
                if (ret < 0)
                {
                    return NULL;
                }
                goto end;
            }
        }
    }
end:
    return dir_info;
}

/**
 * 删除文件
 */
int appfs_delete_file(fs_info_t *info, const char *name)
{
    assert(info);
    assert(name);
    int ret = 0;
    const dir_info_t *dir_info = NULL;

    // 通过文件名查找文件
    dir_info = appfs_find_file_by_name(info, name);
    if (dir_info == NULL)
    {
        printf("file is not found!\n");
        return -1;
    }
    if (dir_info->ref != 0)
    {
        printf("delete filed, ref not is 0.\n");
        return -1;
    }

    // 删除已有的文件
    ret = appfs_blockinfo_bits_set(info, dir_info->block_inx, dir_info->block_inx + ROUND_UP(dir_info->size, info->save.block_size) - 1, 1);
    if (ret < 0)
    {
        return ret;
    }

    ret = appfs_delete_dirinfo_by_name(info, name);
    if (ret < 0)
    {
        printf("delete dirinfo failed!\n");
        return ret;
    }
    return 0;
}
/**
 * 重设大小，会导致所有的数据消失
 * @param info
 * @param dir_info
 * @param size
 * @return <0 fail, >=0 旧大小
 */
int appfs_file_resize_raw(fs_info_t *info, const dir_info_t *dir_info, int size)
{
    int ret;
    int old_size = dir_info->size;
    int new_size = size;

    if (new_size == old_size)
    {
        return old_size;
    }

    ret = appfs_blockinfo_bits_set(info, dir_info->block_inx, dir_info->block_inx + ROUND_UP(dir_info->size, info->save.block_size) - 1, 1);
    if (ret < 0)
    {
        return ret;
    }

    dir_info_t new_info = *dir_info;

    if (dir_info->size != 0)
    {
        new_info.size = 0; // 大小设置为0
        new_info.block_inx = 0;
        ret = appfs_write_dirinfo(info, &new_info);
        if (ret < 0)
        {
            return ret;
        }
    }

    ret = appfs_arrange_files(info);
    if (ret < 0)
    {
        return ret;
    }
    int st;
    int end;

    // 查找连续的块并设置为已使用
    ret = appfs_find_set_blocks(info, &st, &end, ROUND_UP(new_size, info->save.block_size));
    if (ret < 0)
    {
        return ret;
    }
    new_info.block_inx = st;
    new_info.size = new_size;
    ret = appfs_write_dirinfo(info, &new_info);
    if (ret < 0)
    {
        return ret;
    }
    return ret;
}
/**
 * 重新设置文件大小
 * 如果文件大小发生变化，则会清空所有的文件的所有数据
 */
int appfs_file_resize(fs_info_t *info, const char *name, int size)
{
    assert(info);

    int ret = 0;
    const dir_info_t *dir_info = NULL;

    // 通过文件名查找文件
    dir_info = appfs_find_file_by_name(info, name);
    if (dir_info == NULL)
    {
        printf("file is not found!\n");
        return -1;
    }

    return appfs_file_resize_raw(info, dir_info, size);
}
/**
 * 整理文件，将所有文件整齐排列
 * 1. 遍历所有块
 * 2. 查找空闲块
 * 3. 将文件移动到空闲块
 * @param info 文件系统信息
 * @return int 0:成功 -1:失败
 */
int appfs_arrange_files(fs_info_t *info)
{
    int last_idle_block_inx = -1;
    int status = 0;
    assert(info);
    for (int i = 0; i < info->save.blockinfo_nr; i++)
    {
        int ret;
    again:
        ret = info->cb.hw_read_block(info, info->save.blockinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        // 遍历每个块
        for (int j = 0; j < info->save.block_size / BYPTE_PER_PTR; j++)
        {
            // 遍历每个bit
            for (int m = 0; m < BITS_PER_PTR; m++)
            {
                // 找到使用的块
                if ((info->buf[j] & (1UL << m)) == 0)
                {
                    int block_inx = i * (info->save.block_size * BITS_PER_BYTE) + j * BITS_PER_PTR + m;

                    if (last_idle_block_inx != -1)
                    {
                        // 遇到中间有空闲块，需要调整使得中间没有空闲块
                        const dir_info_t *dir_info = appfs_move_file_to_block(info, block_inx, last_idle_block_inx);

                        if (dir_info == NULL)
                        {
                            printf("move file to block failed!\n");
                            return -1;
                        }
                        last_idle_block_inx = -1;
                        goto again;
                    }
                }
                else
                {
                    if (last_idle_block_inx == -1)
                    {
                        last_idle_block_inx = i * (info->save.block_size / BYPTE_PER_PTR) * BITS_PER_PTR + j * BITS_PER_PTR + m;
                    }
                }
            }
        }
    }

    return 0;
}
/**
 * @brief 打印文件信息
 */
int appfs_print_files(fs_info_t *info)
{
    assert(info);
    int ret = 0;
    for (int i = 0; i < info->save.dirinfo_nr; i++)
    {
        // 读取目录信息
        ret = info->cb.hw_read_block(info, info->save.dirinfo_inx + i, info->buf, info->save.block_size);
        if (ret < 0)
        {
            return ret;
        }
        // 遍历目录信息
        for (int j = 0; j < info->save.block_size / sizeof(dir_info_t); j++)
        {
            dir_info_t *dir_info = &((dir_info_t *)info->buf)[j];
            if (dir_info->block_inx == 0)
            {
                continue;
            }
            printf("name:%s, block_inx:%d, size:%d\n", dir_info->name, dir_info->block_inx, dir_info->size);
        }
    }
    return 0;
}
