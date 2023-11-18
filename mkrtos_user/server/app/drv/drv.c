
#include "u_types.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_sleep.h"
#include "u_drv.h"

#include "led.h"
#include "relay.h"
#include "soft_iic.h"
#include "spl06.h"
#include "adc.h"
#include "usart3.h"
#include "usart2.h"
#include "uart4.h"
#include "uart5.h"
#include "rs485.h"
#include "input_ck.h"
#include "wk2xx_hw.h"
#include "hmi_lcd_process.h"
#include "music_control.h"
#include "MDM_RTU_APP.h"
#include "e180-zg120.h"
#include "timer.h"
#include "sysinfo.h"
#include <assert.h>

void music2_send_bytes(u8 *bytes, int len)
{
    /*在下面调用bsp的发送函数*/
    wk_TxChars(2, len, bytes);
    // 等待发送完成
    wk_wait_tx_done(2);
}

void drv_init(void)
{
    led_init();
    relay_init();
    IIC_Init();
    adc_init();
    queue_reset();
    init_usart2(115200);
    init_usart3(115200);
    init_uart4(115200);
    ext_input_check();
    wk2xx_hw_init();
    rs485_init();
    Wk_DeInit(1);
    Wk_DeInit(2);
    Wk_DeInit(3);
    Wk_DeInit(4);
    Wk_Init(1);
    Wk_Init(2);
    Wk_Init(3);
    Wk_Init(4);
    Wk_SetBaud(1, B115200);
    Wk_SetBaud(2, B115200);
    Wk_SetBaud(3, B115200);
    Wk_SetBaud(4, B9600);

    spl0601_init();
    LCD_init();

    if (sys_info_read() < 0)
    {
        sys_info_save();
        bluetooth_set_CP(0, "02");
        bluetooth_set_CP(1, "02");
    }

    bluetooth_init_cfg(0, usart2_send_bytes);
    bluetooth_init_cfg(1, music2_send_bytes);

    MDM_RTU_APPInit();

    // timer_init();

    mod_reset(0, 0xffff, 0);
    mod_set_node_type(COD_TYPE);
    mod_reset(0, 0xffff, 0);
    mod_start_cfg_net(2);

    local_read_net_node_0x22_all();
}
