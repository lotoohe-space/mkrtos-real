#include "delay.h"
#include "u_sleep.h"
void delay_us(int n)
{
    volatile int i = 10 * n;
    while (i--)
        ;
}
void delay_ms(int i)
{
    u_sleep_ms(i);
}