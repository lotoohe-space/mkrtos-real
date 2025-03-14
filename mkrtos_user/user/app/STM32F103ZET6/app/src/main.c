
#include "u_log.h"
#include "u_prot.h"

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
#include "uart4.h"
#include "rs485.h"
#include "wk2xx.h"
#include "auto_close.h"
#include "u_str.h"
#include "e180-zg120.h"
#include "MDM_RTU_APP.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *args[])
{
    printf("argc:%d args[0]:%s\n", argc, args[0]);
    drv_init();
    u_sleep_ms(100);
    ulog_write_str(u_get_global_env()->log_hd, "app start..\n");
    // relay_test();

    // if (mod_send_data(0x10de, 1, 0, 0, "hello world", strlen("hello world")) >= 0)
    // {
    //     printf("send ok..\n");
    // }
    while (1)
    {
        MDM_RTU_Loop();
        user_spl0601_get();
        temp_cal();
        UpdateUI();
        usart2_loop();
        io_ctrl_loop();
        e180_loop();
    }
    return 0;
}
