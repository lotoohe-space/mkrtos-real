

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include "u_util.h"
#include "u_arch.h"
#include "u_irq_sender.h"
#include <string.h>
enum log_op
{
    WRITE_DATA,
    READ_DATA,
    SET_FLAGS
};
enum irq_sender_op
{
    BIND_IRQ,   //!< 绑定一个中断号
    UNBIND_IRQ, //!< 解除绑定
    WAIT_IRQ,   //!< 等待中断触发
    ACK_IRQ,    //!< 中断确认
};
MK_SYSCALL
msg_tag_t uirq_bind(obj_handler_t obj_inx, umword_t irq_no, u_irq_prio_t prio_sub_pre)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(BIND_IRQ, IRQ_PROT, obj_inx).raw,
               0,
               irq_no,
               prio_sub_pre.raw,
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
MK_SYSCALL
msg_tag_t uirq_wait(obj_handler_t obj_inx, int flags)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(WAIT_IRQ, IRQ_PROT, obj_inx).raw,
               0,
               flags,
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
MK_SYSCALL
msg_tag_t uirq_ack(obj_handler_t obj_inx, umword_t irq_no)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(ACK_IRQ, IRQ_PROT, obj_inx).raw,
               0,
               irq_no,
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
