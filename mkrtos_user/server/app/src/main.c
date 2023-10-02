
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
#include "hmi_lcd_process.h"
#include "temp_cal.h"
#include "usart3.h"
#include "usart2.h"
#include "rs485.h"
#include "wk2xx.h"
#include "auto_close.h"
#include <assert.h>
#include <stdio.h>
int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    drv_init();
    u_sleep_ms(100);
    sys_info_read();
    // sys_info.devID = 12;
    // sys_info_save();
    // sys_info.devID = 0;
    // assert(sys_info.devID == 12);
    ulog_write_str(u_get_global_env()->log_hd, "app start..\n");
    relay_test();
    while (1)
    {
        user_spl0601_get();
        temps_cal();
        UpdateUI();
        usart2_loop();
        io_ctrl_loop();
        // u_sleep_ms(10);
        // printf("temp:%f press:%f\n", (sys_info.board_temp), (sys_info.pressure));
        // for (int i = 0; i < 4; i++)
        // {
        //     printf("tmp%d:%f\n", i, sys_info.temp[i]);
        // }
        // usart3_send_string("usart3..\n");
    }
    return 0;
}
