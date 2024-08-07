#include "u_types.h"
#include "u_prot.h"
#include "u_arch.h"

enum SEMA_OP
{
    SEMA_UP,
    SEMA_DOWN,
};
msg_tag_t u_sema_up(obj_handler_t obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(SEMA_UP, SEMA_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
msg_tag_t u_sema_down(obj_handler_t obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(SEMA_DOWN, SEMA_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
