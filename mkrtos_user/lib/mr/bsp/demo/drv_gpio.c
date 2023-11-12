/*
 * @copyright (c) 2023, MR Development Team
 *
 * @license SPDX-License-Identifier: Apache-2.0
 *
 * @date 2023-11-11    MacRsh       First version
 */

#include "drv_gpio.h"

#ifdef MR_USING_GPIO

#if !defined(MR_USING_GPIOA) && !defined(MR_USING_GPIOB) && !defined(MR_USING_GPIOC) && !defined(MR_USING_GPIOD) && !defined(MR_USING_GPIOE) && !defined(MR_USING_GPIOF)
#error "Please define at least one GPIO macro like MR_USING_GPIOA. Otherwise undefine MR_USING_GPIO."
#endif

static int gpio_irq_mask[] =
    {
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
};

static struct drv_gpio_data gpio_drv_data[] =
    {

};

static struct mr_gpio gpio_dev;

static struct drv_gpio_data *drv_gpio_get_data(int pin)
{
    return NULL;
}

static int drv_gpio_configure(struct mr_gpio *gpio, int pin, int mode)
{

    return MR_EOK;
}

static int drv_gpio_read(struct mr_gpio *gpio, int pin)
{
    return 0;
}

static void drv_gpio_write(struct mr_gpio *gpio, int pin, int value)
{
}

void EXTI0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI0_IRQHandler(void)
{
}

void EXTI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI1_IRQHandler(void)
{
}

void EXTI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI2_IRQHandler(void)
{
}

void EXTI3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI3_IRQHandler(void)
{
}

void EXTI4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI4_IRQHandler(void)
{
}

void EXTI9_5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void)
{
}

void EXTI15_10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_10_IRQHandler(void)
{
}

static struct mr_gpio_ops gpio_drv_ops =
    {
        drv_gpio_configure,
        drv_gpio_read,
        drv_gpio_write};

static struct mr_drv gpio_drv =
    {
        Mr_Drv_Type_Gpio,
        &gpio_dev,
        &gpio_drv_ops};

int drv_gpio_init(void)
{
    return mr_gpio_register(&gpio_dev, "gpio", &gpio_drv);
}
MR_INIT_DRV_EXPORT(drv_gpio_init);

#endif /* MR_USING_GPIO */
