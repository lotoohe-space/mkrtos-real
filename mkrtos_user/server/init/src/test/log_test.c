#include "u_types.h"
#include "u_prot.h"

void ulog_test(void)
{
    ulog_write_str(LOG_PROT, "Init task running..\n");
    ulog_write_str(LOG_PROT, "todo..\n");
}