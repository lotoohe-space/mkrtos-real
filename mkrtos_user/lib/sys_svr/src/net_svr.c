#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>
#include "net_svr.h"
#include "rpc_prot.h"

typedef struct sockaddr socketaddr_t;
typedef struct msghdr msghdr_t;
typedef struct timeval timeval_t;
typedef struct pollfd pollfd_t;
RPC_TYPE_DEF_ALL(socketaddr_t)
RPC_TYPE_DEF_ALL(socklen_t)
RPC_TYPE_DEF_ALL(msghdr_t)
RPC_TYPE_DEF_ALL(fd_set)
RPC_TYPE_DEF_ALL(timeval_t)
RPC_TYPE_DEF_ALL(pollfd_t)
RPC_TYPE_DEF_ALL(nfds_t)

// int (*accept)(int s, struct sockaddr *addr, socklen_t *addrlen);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_ACCEPT, accept,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addrlen)
{
    int ret;
    if (!obj->op || !obj->op->accept)
    {
        return -ENOSYS;
    }
    ret = obj->op->accept(s->data, &addr->data, &addrlen->data);

    return ret;
}

// int (*bind)(int s, const struct sockaddr *name, socklen_t namelen);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_BIND, bind,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, namelen)
{
    int ret;
    if (!obj->op || !obj->op->bind)
    {
        return -ENOSYS;
    }
    ret = obj->op->bind(s->data, &addr->data, namelen->data);
    return ret;
}
// int (*shutdown)(int s, int how);
RPC_GENERATION_OP_DISPATCH2(net_t, NET_PROT, NET_SHUTDOWN, shutdown,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, how)
{
    int ret;
    if (!obj->op || !obj->op->shutdown)
    {
        return -ENOSYS;
    }
    ret = obj->op->shutdown(s->data, how->data);
    return ret;
}
// int (*getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_GETPEERNAME, getpeername,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, namelen)
{
    int ret;
    if (!obj->op || !obj->op->getpeername)
    {
        return -ENOSYS;
    }
    ret = obj->op->getpeername(s->data, &addr->data, &namelen->data);
    return ret;
}
// int (*getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_GETSOCKNAME, getsockname,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, name,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, namelen)
{
    int ret;
    if (!obj->op || !obj->op->getsockname)
    {
        return -ENOSYS;
    }
    ret = obj->op->getsockname(s->data, &name->data, &namelen->data);
    return ret;
}
// int (*getsockopt)(int s, int level, int optname, void *optval, socklen_t *optlen);
RPC_GENERATION_OP_DISPATCH5(net_t, NET_PROT, NET_GETSOCKOPT, getsockopt,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, level,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, optname,
                            rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optval,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optlen)
{
    int ret;
    if (!obj->op || !obj->op->getsockopt)
    {
        return -ENOSYS;
    }
    ret = obj->op->getsockopt(s->data, level->data, optname->data, (void *)(optval->data), &optlen->data);
    if (ret < 0)
    {
        return -errno;
    }
    return ret;
}
// int (*setsockopt)(int s, int level, int optname, const void *optval, socklen_t optlen);
RPC_GENERATION_OP_DISPATCH5(net_t, NET_PROT, NET_SETSOCKOPT, setsockopt,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, level,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, optname,
                            rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optval,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, optlen)
{
    int ret;
    if (!obj->op || !obj->op->setsockopt)
    {
        return -ENOSYS;
    }
    ret = obj->op->setsockopt(s->data, level->data, optname->data, (const void *)(optval->data), optlen->data);
    if (ret < 0)
    {
        return -errno;
    }
    return ret;
}
// int (*close)(int s); //在fs协议中实现

// int (*connect)(int s, const struct sockaddr *name, socklen_t namelen);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_CONNECT, connect,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_IN, RPC_TYPE_DATA, name,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, namelen)
{
    int ret;
    if (!obj->op || !obj->op->connect)
    {
        return -ENOSYS;
    }
    ret = obj->op->connect(s->data, &name->data, namelen->data);
    return ret;
}
// int (*listen)(int s, int backlog);
RPC_GENERATION_OP_DISPATCH2(net_t, NET_PROT, NET_LISTEN, listen,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, backlog)
{
    int ret;
    if (!obj->op || !obj->op->listen)
    {
        return -ENOSYS;
    }
    ret = obj->op->listen(s->data, backlog->data);
    return ret;
}
// ssize_t (*recv)(int s, void *mem, size_t len, int flags);
RPC_GENERATION_OP_DISPATCH4(net_t, NET_PROT, NET_RECV, recv,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                            rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int ret;
    if (!obj->op || !obj->op->recv)
    {
        return -ENOSYS;
    }
    ret = obj->op->recv(s->data, mem->data, len->data, flags->data);
    return ret;
}
// ssize_t (*read)(int s, void *mem, size_t len); 实现在fs协议
// ssize_t (*readv)(int s, const struct iovec *iov, int iovcnt); 在客户端实现

// ssize_t (*recvfrom)(int s, void *mem, size_t len, int flags,
//                     struct sockaddr *from, socklen_t *fromlen);
RPC_GENERATION_OP_DISPATCH6(net_t, NET_PROT, NET_RECVFROM, recvfrom,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_INOUT, RPC_TYPE_DATA, mem,
                            rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, from,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, fromlen)
{
    int ret;
    if (!obj->op || !obj->op->recvfrom)
    {
        return -ENOSYS;
    }
    ret = obj->op->recvfrom(s->data, mem->data, len->data, flags->data, &from->data, &fromlen->data);
    return ret;
}
// ssize_t (*recvmsg)(int s, struct msghdr *message, int flags);
#if 0
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_RECVMSG, recvmsg,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                   rpc_msghdr_t_t, rpc_msghdr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, message,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int ret;
    if (!obj->op || !obj->op->recvmsg)
    {
        return -ENOSYS;
    }
    ret = obj->op->recvmsg(s->data, &message->data, flags->data);
    return ret;
}
#endif
// ssize_t (*send)(int s, const void *dataptr, size_t size, int flags);
RPC_GENERATION_OP_DISPATCH4(net_t, NET_PROT, NET_SEND, send,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                            rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int ret;
    if (!obj->op || !obj->op->send)
    {
        return -ENOSYS;
    }
    ret = obj->op->send(s->data, mem->data, size->data, flags->data);
    return ret;
}
// ssize_t (*sendmsg)(int s, const struct msghdr *message, int flags);
#if 0
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_SENDMSG, sendmsg,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                   rpc_msghdr_t_t, rpc_msghdr_t_t, RPC_DIR_IN, RPC_TYPE_DATA, message,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
{
    int ret;
    if (!obj->op || !obj->op->sendmsg)
    {
        return -ENOSYS;
    }
    ret = obj->op->sendmsg(s->data, &message->data, flags->data);
    return ret;
}
#endif
// ssize_t (*sendto)(int s, const void *dataptr, size_t size, int flags,
//                     const struct sockaddr *to, socklen_t tolen);
RPC_GENERATION_OP_DISPATCH6(net_t, NET_PROT, NET_SENDTO, sendto,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                            rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                            rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_IN, RPC_TYPE_DATA, to,
                            rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, tolen)
{
    int ret;
    if (!obj->op || !obj->op->sendto)
    {
        return -ENOSYS;
    }
    ret = obj->op->sendto(s->data, mem->data, size->data, flags->data, &to->data, tolen->data);
    return ret;
}
// int (*socket)(int domain, int type, int protocol);
RPC_GENERATION_OP_DISPATCH3(net_t, NET_PROT, NET_SOCKET, socket,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type,
                            rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, protocol)
{
    int ret;
    if (!obj->op || !obj->op->socket)
    {
        return -ENOSYS;
    }
    ret = obj->op->socket(s->data, type->data, protocol->data);
    return ret;
}
// int (*select)(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
//                 struct timeval *timeout); 文件协议中实现
// int (*poll)(struct pollfd *fds, nfds_t nfds, int timeout);文件协议中实现
// int (*ioctl)(int s, long cmd, void *argp);文件协议中实现
// int (*fcntl)(int s, int cmd, int val);文件协议中实现
// const char *(*inet_ntop)(int af, const void *src, char *dst, socklen_t size); C库中默认有实现
// int (*inet_pton)(int af, const char *src, void *dst);C库中默认有实现

/*dispatch*/
RPC_DISPATCH14(net_t, NET_PROT, typeof(NET_ACCEPT), NET_ACCEPT, accept, NET_BIND, bind,
               NET_SHUTDOWN, shutdown, NET_GETPEERNAME, getpeername, NET_GETSOCKNAME, getsockname,
               NET_GETSOCKOPT, getsockopt, NET_SETSOCKOPT, setsockopt, NET_CONNECT, connect, NET_LISTEN,
               listen, NET_RECV, recv, NET_RECVFROM, recvfrom, NET_SEND, send,
               NET_SENDTO, sendto, NET_SOCKET, socket)
void net_init(net_t *net, const net_operations_t *op)
{
    net->op = op;
    rpc_svr_obj_init(&net->svr, rpc_net_t_dispatch, NET_PROT);
}
