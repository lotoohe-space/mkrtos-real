#pragma once
#include <stddef.h>
#include <u_types.h>
#include <u_rpc.h>

int net_accept(sd_t s, struct sockaddr *addr, socklen_t *addrlen);
int net_bind(int s, const struct sockaddr *name, socklen_t namelen);
int net_shutdown(int s, int how);
int net_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int net_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int net_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int net_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int net_connect(int s, const struct sockaddr *name, socklen_t namelen);
int net_listen(int s, int backlog);
int net_recv(int s, void *mem, size_t len, int flags);
int net_recvfrom(int s, void *mem, size_t len, int flags,
                 struct sockaddr *from, socklen_t *fromlen);
int net_send(int s, const void *dataptr, size_t size, int flags);
int net_sendto(int s, const void *dataptr, size_t size, int flags,
               const struct sockaddr *to, socklen_t tolen);
int net_socket(int domain, int type, int protocol);
