#include "u_types.h"
#include "u_hd_man.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_arch.h"
#include "u_ipc.h"
#include "u_factory.h"
#include "u_thread.h"
#include <sys/types.h>
#include <assert.h>
static obj_handler_t hd = HANDLER_INVALID;

void u_sleep_ms(size_t ms)
{
    thread_ipc_wait(ipc_timeout_create2(0, ms / (1000 / CONFIG_SYS_SCHE_HZ)), NULL);
}
