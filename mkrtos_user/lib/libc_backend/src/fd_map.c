
#include <u_types.h>
#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <pthread.h>
typedef struct fd_map_entry
{
    uint16_t svr_fd;
    uint16_t priv_fd;
    uint8_t flags;
} fd_map_entry_t;

#define FD_MAP_ROW_CN 16
#define FD_MAP_ROW_NR 16

#define FD_MAP_TOTAL (FD_MAP_ROW_CN * FD_MAP_ROW_NR)

typedef struct fd_map_row
{
    fd_map_entry_t entry[FD_MAP_ROW_CN];
} fd_map_row_t;

typedef struct fd_map
{
    fd_map_row_t *row[FD_MAP_ROW_NR];
    pthread_spinlock_t lock;
    uint16_t free_fd;
} fd_map_t;

static fd_map_t fd_map;

int fd_map_alloc(uint16_t svr_fd, uint16_t priv_fd)
{
    int alloc_fd = 0;

again_alloc:
    pthread_spin_lock(&fd_map.lock);
    if (fd_map.free_fd >= FD_MAP_TOTAL)
    {
        // 没有可用的fd了，尝试循环查找
        for (int i = 0; i < FD_MAP_ROW_NR; i++)
        {
            for (int j = 0; j < FD_MAP_ROW_CN; j++)
            {
                if (fd_map.row[i]->entry[j].flags == 0)
                {
                    alloc_fd = i * FD_MAP_ROW_CN + j;
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
    int row_inx = alloc_fd / FD_MAP_ROW_CN;
    int inx = alloc_fd % FD_MAP_ROW_CN;

    assert(row_inx < FD_MAP_ROW_NR);
    if (fd_map.row[row_inx] == NULL)
    {
        fd_map.row[row_inx] = malloc(sizeof(fd_map_row_t));
        if (fd_map.row[row_inx] == NULL)
        {
            pthread_spin_unlock(&fd_map.lock);
            return -EAGAIN;
        }
    }
    assert(fd_map.row[row_inx]->entry[inx].flags == 0);
    fd_map.row[row_inx]->entry[inx].flags = 1;
    fd_map.row[row_inx]->entry[inx].svr_fd = svr_fd;
    fd_map.row[row_inx]->entry[inx].priv_fd = priv_fd;
    pthread_spin_unlock(&fd_map.lock);

    return alloc_fd;
}

int fd_map_update(int fd, fd_map_entry_t *new_entry)
{
    if (fd >= FD_MAP_TOTAL)
    {
        return -1;
    }
    int row_inx = fd / FD_MAP_ROW_CN;
    int inx = fd % FD_MAP_ROW_CN;

    pthread_spin_lock(&fd_map.lock);
    int flags = fd_map.row[row_inx]->entry[inx].flags;

    fd_map.row[row_inx]->entry[inx] = *new_entry;
    fd_map.row[row_inx]->entry[inx].flags = flags;
    pthread_spin_unlock(&fd_map.lock);
}

int fd_map_free(int fd, fd_map_entry_t *ret_entry)
{
    if (fd >= FD_MAP_TOTAL)
    {
        return -1;
    }
    int row_inx = fd / FD_MAP_ROW_CN;
    int inx = fd % FD_MAP_ROW_CN;

    pthread_spin_lock(&fd_map.lock);
    if (fd_map.row[row_inx]->entry[inx].flags == 1)
    {
        if (ret_entry)
        {
            *ret_entry = fd_map.row[row_inx]->entry[inx];
        }
        fd_map.row[row_inx]->entry[inx].flags = 0;
        pthread_spin_unlock(&fd_map.lock);
    }
    else
    {
        pthread_spin_unlock(&fd_map.lock);
        return -1;
    }
    return 0;
}
