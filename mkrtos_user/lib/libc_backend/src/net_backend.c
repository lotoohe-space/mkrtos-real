

#include "syscall_backend.h"
#include <sys/socket.h>
#include <net_cli.h>

int be_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    return net_accept(s, addr, addrlen);
}
int be_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    return net_bind(s, name, namelen);
}
int be_shutdown(int s, int how)
{
    return net_shutdown(s, how);
}
int be_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
    return net_getpeername(s, name, namelen);
}
int be_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    return net_getsockname(s, name, namelen);
}
int be_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    return net_getsockopt(s, level, optname, optval, optlen);
}
int be_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    return net_setsockopt(s, level, optname, optval, optlen);
}
int be_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    return net_connect(s, name, namelen);
}
int be_listen(int s, int backlog)
{
    return net_listen(s, backlog);
}
ssize_t be_recv(int s, void *mem, size_t len, int flags)
{
    return net_recv(s, mem, len, flags);
}
ssize_t be_recvfrom(int s, void *mem, size_t len, int flags,
                    struct sockaddr *from, socklen_t *fromlen)
{
    return net_recvfrom(s, mem, len, flags, from, fromlen);
}
// ssize_t (*recvmsg)(int s, struct msghdr *message, int flags);
ssize_t be_send(int s, const void *dataptr, size_t size, int flags)
{
    return net_send(s, dataptr, size, flags);
}
// ssize_t (*sendmsg)(int s, const struct msghdr *message, int flags);
ssize_t be_sendto(int s, const void *dataptr, size_t size, int flags,
                  const struct sockaddr *to, socklen_t tolen)
{
    return net_sendto(s, dataptr, size, flags, to, tolen);
}
int be_socket(int domain, int type, int protocol)
{
    return net_socket(domain, type, protocol);
}