
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *args[])
{
    // printf("shell>\n");
    ulog_write_str(LOG_PROT, "MKRTOS:\n");
    while (1)
        ;
    return 0;
}
void _start_c_init(void)
{
    main(0, 0);
    while(1);
}
