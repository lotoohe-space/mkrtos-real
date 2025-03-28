
#include "syscall_backend.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "u_prot.h"
#include "u_factory.h"
#include "u_share_mem.h"
#include "u_hd_man.h"
#include "u_vmam.h"
#include "u_task.h"
#include "ns_cli.h"
#include <fcntl.h>
#include <sys/ipc.h>
#include "fs_backend.h"
#include <fd_map.h>
#include <limits.h>
static int _be_shm_open_svr(const char *shm_name)
{
    obj_handler_t shm_hd;
    // char new_src_path[NAME_MAX];
    // const char *cur_path = fs_backend_cur_path();

    // u_rel_path_to_abs(cur_path, shm_name, new_src_path);
    if (ns_query_svr(shm_name, &shm_hd) < 0)
    {
        return -ENOENT;
    }
    int user_fd = fd_map_alloc(0, mk_sd_init2(shm_hd, 0).raw, FD_SHM);

    if (user_fd < 0)
    {
        handler_del_umap(shm_hd);
    }
    return user_fd;
}
static int _be_shm_close_svr(int fd)
{
    fd_map_entry_t u_fd;
    
    int ret = fd_map_get(fd, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    obj_handler_t hd = mk_sd_init_raw(u_fd.priv_fd).hd;
    // int fd = mk_sd_init_raw(u_fd.priv_fd).fd;
    
    handler_del_umap(hd);
    return 0;
}

static int _be_shm_open(const char *shm_name, int flag, mode_t mode, size_t size)
{
    msg_tag_t tag;
    obj_handler_t shm_hd;
    int shm_fd;
    int ret;
    shm_fd = _be_shm_open_svr(shm_name);

    if (shm_fd >= 0)
    {
        if ((flag & O_CREAT) && (flag & O_EXCL))
        {
            _be_shm_close_svr(shm_fd);
            return -EEXIST;
        }
        return shm_fd;
    }
    if (shm_fd < 0)
    {
        shm_hd = handler_alloc();
        if (shm_hd == HANDLER_INVALID)
        {
            return -ENOMEM;
        }
        tag = u_facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd),
                                    SHARE_MEM_CNT_BUDDY_CNT, size);
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        ret = ns_register(shm_name, shm_hd, mode);
        if (ret < 0)
        {
            handler_del_umap(shm_hd);
            return ret;
        }
        handler_free_umap(shm_hd);
        shm_fd = _be_shm_open_svr(shm_name);
        if (shm_fd < 0)
        {
            return shm_fd;
        }
    }
    return shm_fd;
}

int be_shmget(key_t key, size_t size, int flag)
{
    char shm_name[NAME_MAX]="/dev/shm/";
    int new_flag = 0;

    snprintf(shm_name + 9, NAME_MAX - 9, "%d", key);

    if (new_flag&IPC_CREAT)
    {
        new_flag|=O_CREAT;
    }
    if (new_flag&IPC_EXCL)
    {
        new_flag|=O_EXCL;
    }

    return _be_shm_open(shm_name, new_flag, flag & 0777,size);
}
void *be_shmat(int id, const void *addr, int flag)
{
    fd_map_entry_t u_fd;
    msg_tag_t tag;
    umword_t shm_addr;
    int ret;
    
    ret = fd_map_get(id, &u_fd);
    if (ret < 0)
    {
        return (void *)(-EBADF);
    }
    obj_handler_t hd = mk_sd_init_raw(u_fd.priv_fd).hd;
    // int fd = mk_sd_init_raw(u_fd.priv_fd).fd;

    /*FIXME: 缓存addr与fd的关系，在unmap时解除映射*/
    tag = u_share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, 0, (umword_t)addr), &shm_addr, NULL);
    if (msg_tag_get_prot(tag) < 0)
    {
        return (void *)((umword_t)msg_tag_get_prot(tag));
    }
    return (void *)shm_addr;
}
int be_shmdt(const void *addr)
{
    /*TODO:在用户态吧addr与fd的映射关系存起来*/
    return -ENOSYS;
}
int be_shmctl(int id, int cmd, struct shmid_ds *buf)
{
    fd_map_entry_t u_fd;
    int ret;
    
    ret = fd_map_free(id, &u_fd);
    if (ret < 0)
    {
        return -EBADF;
    }
    obj_handler_t hd = mk_sd_init_raw(u_fd.priv_fd).hd;
    // int fd = mk_sd_init_raw(u_fd.priv_fd).fd;

    switch(cmd & (0xff))
    {
        case IPC_STAT:/*TODO:impl me*/
        break;
        case IPC_SET:/*TODO:impl me*/
        break;
        case IPC_RMID:/*TODO:impl me*/
        handler_del_umap(hd);
        ret = 0;
        break;
        default:
        ret = -ENOSYS;
        break;
    }
    return ret;
}

int be_shm_open(const char *name, int flag, mode_t mode)
{
    return _be_shm_open(name, flag, mode, PAGE_SIZE/*default is PAGE_SIZE, fix form ftruncate interface.*/);
}
int be_inner_shm_ftruncate(sd_t fd, size_t size)
{
    msg_tag_t tag;
    obj_handler_t hd = mk_sd_init_raw(fd).hd;
    // int fd = mk_sd_init_raw(u_fd.priv_fd).fd;
    tag = u_share_mem_resize(hd, size);
    return msg_tag_get_val(tag);
}
#if 0
int be_shm_unlink(const char *path)
{
    return -ENOSYS;
}
#endif
long be_shm_mmap(void *start,
                 size_t len,
                 long prot,
                 long flags,
                 sd_t sd,
                 long _offset)
{
    msg_tag_t tag;
    umword_t shm_addr;
    obj_handler_t hd = mk_sd_init_raw(sd).raw;

    // int fd = mk_sd_init_raw(u_fd.priv_fd).fd;
    /*FIXME: 缓存addr与fd的关系，在unmap时解除映射*/
    tag = u_share_mem_map(hd, vma_addr_create(VPAGE_PROT_RW, 0, (umword_t)start), &shm_addr, NULL);
    if (msg_tag_get_prot(tag) < 0)
    {
        return (long)msg_tag_get_prot(tag);
    }
    return (long)shm_addr;
}
