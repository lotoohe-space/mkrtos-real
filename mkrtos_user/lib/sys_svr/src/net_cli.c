#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "ns_types.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_prot.h"
#include "u_hd_man.h"
#include "ns_cli.h"
#include "u_rpc.h"
#include <stdio.h>
#include "net_cli.h"

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

RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_ACCEPT, accept,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addrlen)
int net_accept(sd_t s, struct sockaddr *addr, socklen_t *addrlen)
{
    if (addr == NULL)
    {
        return -EINVAL;
    }
    if (addrlen == NULL)
    {
        return -EINVAL;
    }
    assert(addr);
    assert(addrlen);
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_socketaddr_t_t rpc_addr = {
        .data = *addr,
    };
    rpc_socklen_t_t rpc_addrlen = {
        .data = *addrlen,
    };
    msg_tag_t tag;

    tag = net_t_accept_call(hd, &rpc_s, &rpc_addr, &rpc_addrlen);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *addr = rpc_addr.data;
    *addrlen = rpc_addrlen.data;
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_BIND, bind,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, namelen)
int net_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    if (name == NULL)
    {
        return -EINVAL;
    }
    assert(name);
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_socketaddr_t_t rpc_addr = {
        .data = *name,
    };
    rpc_socklen_t_t rpc_addrlen = {
        .data = namelen,
    };
    msg_tag_t tag;

    tag = net_t_bind_call(hd, &rpc_s, &rpc_addr, &rpc_addrlen);
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL2(TRUE, net_t, NET_PROT, NET_SHUTDOWN, shutdown,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, how)
int net_shutdown(int s, int how)
{
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_int_t rpc_how = {
        .data = how,
    };
    msg_tag_t tag;

    tag = net_t_shutdown_call(hd, &rpc_s, &rpc_how);
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_GETPEERNAME, getpeername,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, addr,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, namelen)
int net_getpeername(int s, struct sockaddr *name, socklen_t *namelen)
{
    if (name == NULL)
    {
        return -EINVAL;
    }
    if (namelen == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_socketaddr_t_t rpc_name = {
        .data = *name,
    };
    rpc_socklen_t_t rpc_namelen = {
        .data = *namelen,
    };
    msg_tag_t tag;

    tag = net_t_getpeername_call(hd, &rpc_s, &rpc_name, &rpc_namelen);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *name = rpc_name.data;
    *namelen = rpc_namelen.data;
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_GETSOCKNAME, getsockname,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, name,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, namelen)
int net_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    if (name == NULL)
    {
        return -EINVAL;
    }
    if (namelen == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_socketaddr_t_t rpc_name = {
        .data = *name,
    };
    rpc_socklen_t_t rpc_namelen = {
        .data = *namelen,
    };
    msg_tag_t tag;

    tag = net_t_getsockname_call(hd, &rpc_s, &rpc_name, &rpc_namelen);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *name = rpc_name.data;
    *namelen = rpc_namelen.data;
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL5(TRUE, net_t, NET_PROT, NET_GETSOCKOPT, getsockopt,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, level,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, optname,
                     rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optval,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optlen)
int net_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    if (optval == NULL)
    {
        return -EINVAL;
    }
    if (optlen == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_int_t rpc_level = {
        .data = level,
    };
    rpc_int_t rpc_optname = {
        .data = optname,
    };
    rpc_ref_array_uint32_t_uint8_t_64_t rpc_buf = {
        .data = optval,
        .len = sizeof(rpc_buf.data),
    };
    rpc_socklen_t_t rpc_optlen = {
        .data = *optlen,
    };
    msg_tag_t tag;

    tag = net_t_getsockopt_call(hd, &rpc_s, &rpc_level, &rpc_optname, &rpc_buf, &rpc_optlen);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    *optlen = rpc_optlen.data;
    // memcpy(optval, &rpc_buf.data[0], MIN(*optlen, rpc_buf.data));
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL5(TRUE, net_t, NET_PROT, NET_SETSOCKOPT, setsockopt,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, level,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, optname,
                     rpc_ref_array_uint32_t_uint8_t_64_t, rpc_array_uint32_t_uint8_t_64_t, RPC_DIR_INOUT, RPC_TYPE_DATA, optval,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, optlen)
int net_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    if (optval == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_int_t rpc_level = {
        .data = level,
    };
    rpc_int_t rpc_optname = {
        .data = optname,
    };
    rpc_ref_array_uint32_t_uint8_t_64_t rpc_buf = {
        .data = (void *)optval,
        .len = MIN(optlen, sizeof(rpc_buf.data)),
    };
    rpc_socklen_t_t rpc_optlen = {
        .data = optlen,
    };
    msg_tag_t tag;

    tag = net_t_setsockopt_call(hd, &rpc_s, &rpc_level, &rpc_optname, &rpc_buf, &rpc_optlen);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_CONNECT, connect,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_IN, RPC_TYPE_DATA, name,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_IN, RPC_TYPE_DATA, namelen)
int net_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    if (name == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_socketaddr_t_t rpc_name = {
        .data = *name,
    };
    rpc_socklen_t_t rpc_namelen = {
        .data = namelen,
    };
    msg_tag_t tag;

    tag = net_t_connect_call(hd, &rpc_s, &rpc_name, &rpc_namelen);
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL2(TRUE, net_t, NET_PROT, NET_LISTEN, listen,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, backlog)
int net_listen(int s, int backlog)
{
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;
    rpc_int_t rpc_s = {
        .data = fd,
    };
    rpc_int_t rpc_backlog = {
        .data = backlog,
    };
    msg_tag_t tag;

    tag = net_t_listen_call(hd, &rpc_s, &rpc_backlog);
    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL4(TRUE, net_t, NET_PROT, NET_RECV, recv,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                     rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
int net_recv(int s, void *mem, size_t len, int flags)
{
    if (mem == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };

    rpc_int_t rpc_flags = {
        .data = flags,
    };
    msg_tag_t tag;

    int rlen = 0;
    int r_once_len = 0;
    while (rlen < len)
    {

        rpc_ref_array_uint32_t_uint8_t_128_t rpc_mem = {
            .data = mem + rlen,
            // .len = MIN(len, sizeof(rpc_mem.data)),
        };
        r_once_len = MIN(sizeof(rpc_mem.data), len - rlen);
        rpc_mem.len = r_once_len;
        rpc_size_t_t rpc_len = {
            .data = r_once_len,
        };
        msg_tag_t tag = net_t_recv_call(hd, &rpc_s, &rpc_mem, &rpc_len, &rpc_flags);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != r_once_len)
        {
            break;
        }
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL6(TRUE, net_t, NET_PROT, NET_RECVFROM, recvfrom,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_INOUT, RPC_TYPE_DATA, mem,
                     rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, from,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, fromlen)
int net_recvfrom(int s, void *mem, size_t len, int flags,
                 struct sockaddr *from, socklen_t *fromlen)
{
    if (mem == NULL || from == NULL || fromlen == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };

    rpc_int_t rpc_flags = {
        .data = flags,
    };

    rpc_socketaddr_t_t rpc_form = {
        .data = *from,
    };
    rpc_socklen_t_t rpc_fromlen = {
        .data = *fromlen,
    };
    msg_tag_t tag;

    int rlen = 0;
    int r_once_len = 0;
    while (rlen < len)
    {

        rpc_ref_array_uint32_t_uint8_t_128_t rpc_mem = {
            .data = mem + rlen,
            // .len = MIN(len, sizeof(rpc_mem.data)),
        };
        r_once_len = MIN(sizeof(rpc_mem.data), len - rlen);
        rpc_mem.len = r_once_len;
        rpc_size_t_t rpc_len = {
            .data = r_once_len,
        };
        msg_tag_t tag = net_t_recvfrom_call(hd, &rpc_s, &rpc_mem, &rpc_len, &rpc_flags, &rpc_form, &rpc_fromlen);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != r_once_len)
        {
            break;
        }
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL4(TRUE, net_t, NET_PROT, NET_SEND, send,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                     rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags)
int net_send(int s, const void *dataptr, size_t size, int flags)
{
    if (dataptr == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };

    rpc_int_t rpc_flags = {
        .data = flags,
    };

    msg_tag_t tag;

    int rlen = 0;
    int w_once_len = 0;
    while (rlen < size)
    {

        rpc_ref_array_uint32_t_uint8_t_128_t rpc_mem = {
            .data = (void *)dataptr + rlen,
            // .len = MIN(len, sizeof(rpc_mem.data)),
        };
        w_once_len = MIN(sizeof(rpc_mem.data), size - rlen);
        rpc_mem.len = w_once_len;
        rpc_size_t_t rpc_len = {
            .data = w_once_len,
        };
        msg_tag_t tag = net_t_send_call(hd, &rpc_s, &rpc_mem, &rpc_len, &rpc_flags);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != w_once_len)
        {
            break;
        }
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL6(TRUE, net_t, NET_PROT, NET_SENDTO, sendto,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_ref_array_uint32_t_uint8_t_128_t, rpc_array_uint32_t_uint8_t_128_t, RPC_DIR_IN, RPC_TYPE_DATA, mem,
                     rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                     rpc_socketaddr_t_t, rpc_socketaddr_t_t, RPC_DIR_IN, RPC_TYPE_DATA, to,
                     rpc_socklen_t_t, rpc_socklen_t_t, RPC_DIR_INOUT, RPC_TYPE_DATA, tolen)
int net_sendto(int s, const void *dataptr, size_t size, int flags,
               const struct sockaddr *to, socklen_t tolen)
{
    if (dataptr == NULL || to == NULL)
    {
        return -EINVAL;
    }
    obj_handler_t hd = mk_sd_init_raw(s).hd;
    int fd = mk_sd_init_raw(s).fd;

    rpc_int_t rpc_s = {
        .data = fd,
    };

    rpc_int_t rpc_flags = {
        .data = flags,
    };
    rpc_socketaddr_t_t rpc_to = {
        .data = *to,
    };
    rpc_socklen_t_t rpc_tolen = {
        .data = tolen,
    };
    msg_tag_t tag;

    int rlen = 0;
    int w_once_len = 0;
    while (rlen < size)
    {
        rpc_ref_array_uint32_t_uint8_t_128_t rpc_mem = {
            .data = (void *)(dataptr + rlen),
            // .len = MIN(len, sizeof(rpc_mem.data)),
        };
        w_once_len = MIN(sizeof(rpc_mem.data), size - rlen);
        rpc_mem.len = w_once_len;
        rpc_size_t_t rpc_len = {
            .data = w_once_len,
        };
        msg_tag_t tag = net_t_sendto_call(hd, &rpc_s, &rpc_mem, &rpc_len, &rpc_flags, &rpc_to, &rpc_tolen);

        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        rlen += msg_tag_get_val(tag);
        if (msg_tag_get_val(tag) != w_once_len)
        {
            break;
        }
    }

    return msg_tag_get_val(tag);
}
RPC_GENERATION_CALL3(TRUE, net_t, NET_PROT, NET_SOCKET, socket,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, s,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, type,
                     rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, protocol)
int net_socket(int domain, int type, int protocol)
{
    int ret;
    obj_handler_t hd;
    switch (domain)
    {
    case AF_INET:
    case AF_INET6:
        ret = ns_query("/net" /*FIXME:更改为宏*/, &hd, 1);
        break;
    default:
        return -ENOSYS;
    }
    if (ret < 0)
    {
        return ret;
    }
    rpc_int_t rpc_domain = {
        .data = domain,
    };
    rpc_int_t rpc_type = {
        .data = type,
    };
    rpc_int_t rpc_protocol = {
        .data = protocol,
    };
    msg_tag_t tag;

    tag = net_t_socket_call(hd, &rpc_domain, &rpc_type, &rpc_protocol);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    return mk_sd_init2(hd, msg_tag_get_val(tag)).raw;
}