#include "u_types.h"
#include "u_hd_man.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_arch.h"
#include "u_ipc.h"
#include "u_factory.h"
#include <sys/types.h>
#include <assert.h>
static obj_handler_t hd = HANDLER_INVALID;
void u_sleep_init(void)
{
    msg_tag_t tag;

    if (hd != HANDLER_INVALID)
    {
        return;
    }
    hd = handler_alloc();
    assert(hd != HANDLER_INVALID);
    tag = factory_create_ipc(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, hd));
    assert(msg_tag_get_val(tag) >= 0);
}

void u_sleep_ms(size_t ms)
{
    ipc_call(hd, msg_tag_init4(0, 0, 0, 0), ipc_timeout_create2(ms / (1000 / SYS_SCHE_HZ), 0));
}
