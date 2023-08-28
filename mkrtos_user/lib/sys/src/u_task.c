#include "u_types.h"
#include "u_prot.h"

enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
};

msg_tag_t task_unmap(obj_handler_t task_han, obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");

    syscall(task_han, msg_tag_init3(TASK_OBJ_UNMAP, 1, TASK_PROT).raw,
            obj,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);

    return tag;
}
