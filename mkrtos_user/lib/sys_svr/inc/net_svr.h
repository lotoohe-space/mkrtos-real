#pragma once

#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_types.h"
#include <sys/types.h>
typedef struct net_operations
{
    int (*accept)(int s, struct sockaddr *addr, socklen_t *addrlen);
    int (*bind)(int s, const struct sockaddr *name, socklen_t namelen);
    int (*shutdown)(int s, int how);
    int (*getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockopt)(int s, int level, int optname, void *optval, socklen_t *optlen);
    int (*setsockopt)(int s, int level, int optname, const void *optval, socklen_t optlen);
    // int (*close)(int s);
    int (*connect)(int s, const struct sockaddr *name, socklen_t namelen);
    int (*listen)(int s, int backlog);
    ssize_t (*recv)(int s, void *mem, size_t len, int flags);
    // ssize_t (*read)(int s, void *mem, size_t len);
    // ssize_t (*readv)(int s, const struct iovec *iov, int iovcnt); 在客户端实现
    ssize_t (*recvfrom)(int s, void *mem, size_t len, int flags,
                        struct sockaddr *from, socklen_t *fromlen);
    // ssize_t (*recvmsg)(int s, struct msghdr *message, int flags);
    ssize_t (*send)(int s, const void *dataptr, size_t size, int flags);
    // ssize_t (*sendmsg)(int s, const struct msghdr *message, int flags);
    ssize_t (*sendto)(int s, const void *dataptr, size_t size, int flags,
                      const struct sockaddr *to, socklen_t tolen);
    int (*socket)(int domain, int type, int protocol);
    // ssize_t (*write)(int s, const void *dataptr, size_t size); 文件协议中实现
    // ssize_t (*writev)(int s, const struct iovec *iov, int iovcnt);文件协议中实现
    // int (*select)(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
    //               struct timeval *timeout);文件协议中实现
    // int (*poll)(struct pollfd *fds, nfds_t nfds, int timeout);文件协议中实现
    // int (*ioctl)(int s, long cmd, void *argp);文件协议中实现
    // int (*fcntl)(int s, int cmd, int val);文件协议中实现
    // const char *(*inet_ntop)(int af, const void *src, char *dst, socklen_t size);C库中实现
    // int (*inet_pton)(int af, const char *src, void *dst);C库中实现
} net_operations_t;

typedef struct net
{
    rpc_svr_obj_t svr;
    const net_operations_t *op;
} net_t;
#if 0
int net_svr_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int net_svr_bind(int s, const struct sockaddr *name, socklen_t namelen);
int net_svr_shutdown(int s, int how);
int net_svr_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int net_svr_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int net_svr_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int net_svr_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int net_svr_connect(int s, const struct sockaddr *name, socklen_t namelen);
int net_svr_listen(int s, int backlog);
ssize_t net_svr_recv(int s, void *mem, size_t len, int flags);
ssize_t net_svr_recvfrom(int s, void *mem, size_t len, int flags,
                         struct sockaddr *from, socklen_t *fromlen);
ssize_t net_svr_recvmsg(int s, struct msghdr *message, int flags);
ssize_t net_svr_send(int s, const void *dataptr, size_t size, int flags);
ssize_t net_svr_sendmsg(int s, const struct msghdr *message, int flags);
ssize_t net_svr_sendto(int s, const void *dataptr, size_t size, int flags,
                       const struct sockaddr *to, socklen_t tolen);
int net_svr_socket(int domain, int type, int protocol);
#endif
void net_init(net_t *net, const net_operations_t *op);
