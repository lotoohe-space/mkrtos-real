

#include "syscall_backend.h"
#include <sys/socket.h>
#include <net_cli.h>
#include "fd_map.h"
int be_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    int fd = net_accept(u_fd.priv_fd, addr, addrlen);

    if (fd < 0)
    {
        return fd;
    }
    int user_fd = fd_map_alloc(0, fd, FD_FS);

    if (user_fd < 0)
    {
        be_close(user_fd);
    }
    return user_fd;
}
int be_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_bind(u_fd.priv_fd, name, namelen);
}
int be_shutdown(int s, int how)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_shutdown(u_fd.priv_fd, how);
}
int be_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_getpeername(u_fd.priv_fd, name, namelen);
}
int be_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_getsockname(u_fd.priv_fd, name, namelen);
}
int be_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_getsockopt(u_fd.priv_fd, level, optname, optval, optlen);
}
int be_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_setsockopt(u_fd.priv_fd, level, optname, optval, optlen);
}
int be_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_connect(u_fd.priv_fd, name, namelen);
}
int be_listen(int s, int backlog)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_listen(u_fd.priv_fd, backlog);
}
ssize_t be_recv(int s, void *mem, size_t len, int flags)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_recv(u_fd.priv_fd, mem, len, flags);
}
ssize_t be_recvfrom(int s, void *mem, size_t len, int flags,
                    struct sockaddr *from, socklen_t *fromlen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_recvfrom(u_fd.priv_fd, mem, len, flags, from, fromlen);
}
// ssize_t (*recvmsg)(int s, struct msghdr *message, int flags);
ssize_t be_send(int s, const void *dataptr, size_t size, int flags)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_send(u_fd.priv_fd, dataptr, size, flags);
}
// ssize_t (*sendmsg)(int s, const struct msghdr *message, int flags);
ssize_t be_sendto(int s, const void *dataptr, size_t size, int flags,
                  const struct sockaddr *to, socklen_t tolen)
{
    fd_map_entry_t u_fd;
    int ret = fd_map_get(s, &u_fd);

    if (ret < 0)
    {
        return -EBADF;
    }
    return net_sendto(u_fd.priv_fd, dataptr, size, flags, to, tolen);
}
int be_socket(int domain, int type, int protocol)
{
    int fd;
    
    fd = net_socket(domain, type, protocol);
    if (fd < 0)
    {
        return fd;
    }
    int user_fd = fd_map_alloc(0, fd, FD_FS);

    if (user_fd < 0)
    {
        be_close(user_fd);
    }
    return user_fd;
}