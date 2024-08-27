/**
 * @file fd_map.c
 * @author your name (1358745329@qq.com)
 * @brief fd映射层
 * @version 0.1
 * @date 2024-08-08
 *
 * @copyright Copyright (c) ATShining 2024
 *
 */
#include <u_types.h>
#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <pthread.h>
#include <fd_map.h>
#include <string.h>

#define FD_MAP_TOTAL (CONFIG_FD_MAP_ROW_CN * CONFIG_FD_MAP_ROW_NR)

typedef struct fd_map_row
{
    fd_map_entry_t entry[CONFIG_FD_MAP_ROW_CN];
} fd_map_row_t;

typedef struct fd_map
{
    fd_map_row_t *row[CONFIG_FD_MAP_ROW_NR];
    pthread_spinlock_t lock;
    uint16_t free_fd;
} fd_map_t;

static fd_map_t fd_map;
/**
 * @brief 分配一个可用的fd
 *
 * @param svr_fd
 * @param priv_fd
 * @param type
 * @return int
 */
int fd_map_alloc(uint32_t svr_fd, uint32_t priv_fd, enum fd_type type)
{
    int alloc_fd = 0;

again_alloc:
    pthread_spin_lock(&fd_map.lock);
    if (fd_map.free_fd >= FD_MAP_TOTAL)
    {
        // 没有可用的fd了，尝试循环查找
        for (int i = 0; i < CONFIG_FD_MAP_ROW_NR; i++)
        {
            for (int j = 0; j < CONFIG_FD_MAP_ROW_CN; j++)
            {
                if (fd_map.row[i]->entry[j].flags == 0)
                {
                    alloc_fd = i * CONFIG_FD_MAP_ROW_CN + j;
                    goto next;
                }
            }
        }
        pthread_spin_unlock(&fd_map.lock);
        // 没有可用的了
        return -EAGAIN;
    }
    else
    {
        alloc_fd = fd_map.free_fd;
    }
next:;
    int row_inx = alloc_fd / CONFIG_FD_MAP_ROW_CN;
    int inx = alloc_fd % CONFIG_FD_MAP_ROW_CN;

    assert(row_inx < CONFIG_FD_MAP_ROW_NR);
    if (fd_map.row[row_inx] == NULL)
    {
        fd_map.row[row_inx] = malloc(sizeof(fd_map_row_t));
        if (fd_map.row[row_inx] == NULL)
        {
            pthread_spin_unlock(&fd_map.lock);
            return -EAGAIN;
        }
        memset(fd_map.row[row_inx], 0, sizeof(fd_map_row_t));
    }
    assert(fd_map.row[row_inx]->entry[inx].flags == 0);
    fd_map.row[row_inx]->entry[inx].flags = 1;
    fd_map.row[row_inx]->entry[inx].svr_fd = svr_fd;
    fd_map.row[row_inx]->entry[inx].priv_fd = priv_fd;
    fd_map.row[row_inx]->entry[inx].type = type;
    fd_map.free_fd++;
    pthread_spin_unlock(&fd_map.lock);

    return alloc_fd;
}
/**
 * @brief 获取fd
 *
 * @param fd
 * @param new_entry
 * @return int
 */
int fd_map_get(int fd, fd_map_entry_t *new_entry)
{
    assert(new_entry);
    if (fd >= FD_MAP_TOTAL)
    {
        return -1;
    }
    int row_inx = fd / CONFIG_FD_MAP_ROW_CN;
    int inx = fd % CONFIG_FD_MAP_ROW_CN;

    if (fd_map.row[row_inx] == NULL)
    {
        return -1;
    }

    *new_entry = fd_map.row[row_inx]->entry[inx];
    return 0;
}
/**
 * @brief 更新fd
 *
 * @param fd
 * @param new_entry
 * @return int
 */
int fd_map_update(int fd, fd_map_entry_t *new_entry)
{
    if (fd >= FD_MAP_TOTAL)
    {
        return -1;
    }
    int row_inx = fd / CONFIG_FD_MAP_ROW_CN;
    int inx = fd % CONFIG_FD_MAP_ROW_CN;

    pthread_spin_lock(&fd_map.lock);
    int flags = fd_map.row[row_inx]->entry[inx].flags;

    fd_map.row[row_inx]->entry[inx] = *new_entry;
    fd_map.row[row_inx]->entry[inx].flags = flags;
    pthread_spin_unlock(&fd_map.lock);
}
/**
 * @brief 释放fd
 *
 * @param fd
 * @param ret_entry
 * @return int
 */
int fd_map_free(int fd, fd_map_entry_t *ret_entry)
{
    if (fd >= FD_MAP_TOTAL)
    {
        return -1;
    }
    int row_inx = fd / CONFIG_FD_MAP_ROW_CN;
    int inx = fd % CONFIG_FD_MAP_ROW_CN;

    pthread_spin_lock(&fd_map.lock);
    if (fd_map.row[row_inx]->entry[inx].flags == 1)
    {
        if (ret_entry)
        {
            *ret_entry = fd_map.row[row_inx]->entry[inx];
        }
        fd_map.row[row_inx]->entry[inx].flags = 0;
        if (fd_map.free_fd > fd)
        {
            fd_map.free_fd = fd;
        }
        pthread_spin_unlock(&fd_map.lock);
    }
    else
    {
        pthread_spin_unlock(&fd_map.lock);
        return -1;
    }
    return 0;
}
