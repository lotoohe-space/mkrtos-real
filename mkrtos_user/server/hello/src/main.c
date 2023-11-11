
// #include <stdio.h>
#include "u_log.h"
#include "u_env.h"



int main(int argc, char *args[])
{
    // printf("Hello world.\n");
    ulog_write_str(u_get_global_env()->log_hd, "Hello world.\n");
    return 0;
}
