

#include "lwip/sockets.h"
#include "net_svr.h"
#include "rpc_prot.h"
#include "u_sig.h"
#include "lwipopts.h"
#include "u_mutex.h"
#include <u_hd_man.h>

typedef struct fd_man
{
    pid_t pid;
    int fd;
    uint8_t used;
} fd_man_t;

static fd_man_t fd_man_list[MEMP_NUM_UDP_PCB + MEMP_NUM_TCP_PCB + MEMP_NUM_TCP_PCB_LISTEN];
static u_mutex_t fd_man_lock;

void fd_man_reg(pid_t pid, int fd)
{
    u_mutex_lock(&fd_man_lock, 0, NULL);
    for (int i = 0; i < ARRAY_SIZE(fd_man_list); i++)
    {
        if (fd_man_list[i].used == 0)
        {
            fd_man_list[i].pid = pid;
            fd_man_list[i].fd = fd;
            fd_man_list[i].used = 1;
            int w_ret = pm_sig_watch(pid, 0 /*TODO:现在只有kill */);
            if (w_ret < 0)
            {
                printf("net wath pid %d err.\n", w_ret);
            }
            break;
        }
    }
    u_mutex_unlock(&fd_man_lock);
}
void fd_man_unreg(int fd)
{
    u_mutex_lock(&fd_man_lock, 0, NULL);
    for (int i = 0; i < ARRAY_SIZE(fd_man_list); i++)
    {
        if (fd_man_list[i].used == 1 && fd_man_list[i].fd == fd)
        {
            fd_man_list[i].used = 0;
            break;
        }
    }
    u_mutex_unlock(&fd_man_lock);
}
void fd_man_close_without_pid(pid_t pid)
{
    u_mutex_lock(&fd_man_lock, 0, NULL);
    for (int i = 0; i < ARRAY_SIZE(fd_man_list); i++)
    {
        if (fd_man_list[i].used == 1 && fd_man_list[i].pid == pid)
        {
            printf("net close fd:%d\n", fd_man_list[i].fd);
            lwip_close(fd_man_list[i].fd);
            fd_man_list[i].used = 0;
        }
    }
    u_mutex_unlock(&fd_man_lock);
    printf("net close--\n");
}
static int fs_sig_call_back(pid_t pid, umword_t sig_val)
{
    switch (sig_val)
    {
    case KILL_SIG:
        fd_man_close_without_pid(pid);
        break;
    }
    return 0;
}

static int net_svr_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    int fd;
    pid_t pid = thread_get_src_pid();

    fd = lwip_accept(s, addr, addrlen);
    if (fd >= 0)
    {
        fd_man_reg(pid, fd);
    }
    return fd;
}
static int net_svr_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    return lwip_bind(s, name, namelen);
}
static int net_svr_shutdown(int s, int how)
{
    int ret;
    
    ret = lwip_shutdown(s, how);
    fd_man_unreg(s);
    return ret;
}
static int net_svr_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
    return lwip_getpeername(s, name, namelen);
}
static int net_svr_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    return lwip_getsockname(s, name, namelen);
}
static int net_svr_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    return lwip_getsockopt(s, level, optname, optval, optlen);
}
static int net_svr_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    return lwip_setsockopt(s, level, optname, optval, optlen);
}
static int net_svr_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    return lwip_connect(s, name, namelen);
}
static int net_svr_listen(int s, int backlog)
{
    return lwip_listen(s, backlog);
}
static ssize_t net_svr_recv(int s, void *mem, size_t len, int flags)
{
    return lwip_recv(s, mem, len, flags);
}
static ssize_t net_svr_recvfrom(int s, void *mem, size_t len, int flags,
                                struct sockaddr *from, socklen_t *fromlen)
{

    return lwip_recvfrom(s, mem, len, flags, from, fromlen);
}
#if 0
static ssize_t net_svr_recvmsg(int s, struct msghdr *message, int flags)
{

    return lwip_recvmsg(s, message, flags);
}
#endif
static ssize_t net_svr_send(int s, const void *dataptr, size_t size, int flags)
{

    return lwip_send(s, dataptr, size, flags);
}
#if 0
ssize_t net_svr_sendmsg(int s, const struct msghdr *message, int flags)
{

    return lwip_sendmsg(s, message, flags);
}
#endif
static ssize_t net_svr_sendto(int s, const void *dataptr, size_t size, int flags,
                              const struct sockaddr *to, socklen_t tolen)
{
    return lwip_sendto(s, dataptr, size, flags, to, tolen);
}
static int net_svr_socket(int domain, int type, int protocol)
{
    int fd;
    pid_t pid = thread_get_src_pid();

    fd = lwip_socket(domain, type, protocol);
    if (fd >= 0)
    {
        fd_man_reg(pid, fd);
    }
    return fd;
}
static net_operations_t net_op = {
    .accept = net_svr_accept,
    .bind = net_svr_bind,
    .shutdown = net_svr_shutdown,
    .getpeername = net_svr_getpeername,
    .getsockname = net_svr_getsockname,
    .getsockopt = net_svr_getsockopt,
    .setsockopt = net_svr_setsockopt,
    .connect = net_svr_connect,
    .listen = net_svr_listen,
    .recv = net_svr_recv,
    .recvfrom = net_svr_recvfrom,
    .send = net_svr_send,
    .sendto = net_svr_sendto,
    .socket = net_svr_socket,
};
static net_t net;
void net_svr_init(void)
{
    msg_tag_t tag;

    net_init(&net, &net_op);
    meta_reg_svr_obj(&net.svr, NET_PROT);

    u_mutex_init(&fd_man_lock, handler_alloc());

    pm_sig_func_set(fs_sig_call_back);
}
