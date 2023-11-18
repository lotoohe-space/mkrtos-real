#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <u_drv.h>
#include <stdio.h>
/* defined the LED0 pin: PC0 */
#define LED0_PIN GET_PIN(C, 0)
extern void rt_hw_board_init(void);
extern int dfs_init(void);
int main(void)
{
    printf("test\n");
    u_drv_init();
    /* init board */
    rt_hw_board_init();
    dfs_init();
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
        printf("led test..\n");
    }
}