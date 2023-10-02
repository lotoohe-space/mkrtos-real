
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
#include "input_ck.h"
#include "wk2xx_hw.h"
#include "hmi_lcd_process.h"
#include <assert.h>

void drv_init(void)
{
    u_drv_init();
    led_init();
    relay_init();
    IIC_Init();
    adc_init();
    init_usart3(115200);
    init_usart2(115200);
    init_uart4(115200);
    init_uart5(115200);
    ext_input_check();
    wk2xx_hw_init();

    spl0601_init();
    LCD_init();
}
