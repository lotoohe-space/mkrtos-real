
#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "u_ns.h"
#include "u_rpc.h"
#include "u_sleep.h"
#include "drv.h"
#include "led.h"
#include "test.h"
#include "spl06.h"
#include "sysinfo.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *args[])
{
    drv_init();
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    ulog_write_str(u_get_global_env()->log_hd, "app start..\n");
    relay_test();
    while (1)
    {
        user_spl0601_get();
        u_sleep_ms(500);
        printf("temp:%d press:%d\n", (int)(sys_info.board_temp), (int)(sys_info.pressure));
        toogle_led_0();
    }
    return 0;
}
