#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <u_drv.h>
#include <stdio.h>
#include <syscall_backend.h>
#include <rtthread_inter.h>
#include <test.h>
/* defined the LED0 pin: PC0 */
#define LED0_PIN GET_PIN(C, 0)
extern void rt_hw_board_init(void);
extern int dfs_init(void);
static struct rt_thread main_rtt;
int main(void)
{
    printf("test\n");
    rt_thread_bind_mkrtos(&main_rtt);
    /* init board */
    rt_hw_board_init();
    dfs_init();

    rtthread_drv_test();

    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(200);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(200);
        printf("led test..\n");
    }
}