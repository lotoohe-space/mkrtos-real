#include "u_types.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_util.h"
#include "u_arch.h"

enum ipc_op
{
    IPC_BIND,
    IPC_DO = 6,
};
msg_tag_t ipc_bind(obj_handler_t obj, obj_handler_t th_obj, umword_t user_obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(IPC_BIND, IPC_PROT, obj).raw,
               th_obj,
               user_obj,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     :
                     : ARCH_REG_0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
