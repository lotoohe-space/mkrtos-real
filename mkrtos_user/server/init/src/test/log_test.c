#include "u_types.h"
#include "u_prot.h"
#include "u_log.h"
#include <stdio.h>
void ulog_test(void)
{
    uint8_t data[10];

    ulog_write_str(LOG_PROT, "Init task running..\n");

    int len = ulog_read_bytes(LOG_PROT, data, sizeof(data) - 1);
    if (len > 0)
    {
        data[len] = 0;
        printf("%s\n", data);
    }
}