

#include "lwip/sockets.h"
#include "net_svr.h"
#include "rpc_prot.h"
static int net_svr_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    return lwip_accept(s, addr, addrlen);
}
static int net_svr_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    return lwip_bind(s, name, namelen);
}
static int net_svr_shutdown(int s, int how)
{
    return lwip_shutdown(s, how);
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
    return lwip_socket(domain, type, protocol);
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
}
