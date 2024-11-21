

#include <u_types.h>
#include <errno.h>
#include <libfdt.h>
#include <mk_dev.h>
#include <stdio.h>
#include <assert.h>
#include "mk_drv.h"
#include "mk_pin_drv.h"
#include "mk_dtb_parse.h"
#include <u_prot.h>
#include <u_vmam.h>
#include "at32f435_437_gpio.h"
#include "at32f435_437_crm.h"

static gpio_type *gpio_type_list[] =
    {
        GPIOA,
        GPIOB,
        GPIOC,
        GPIOD,
        GPIOE,
        GPIOF,
        GPIOG,
        GPIOH,
};
static gpio_type *drv_pin_get_port_data(int pin)
{
    pin >>= 4;
    if ((pin >= ARRAY_SIZE(gpio_type_list)) || (gpio_type_list[pin] == NULL))
    {
        return NULL;
    }
    return gpio_type_list[pin];
}

static int pin_configure(mk_pin_t *pin, int number, mk_pin_mode_t cfg)
{
    gpio_type *pin_port_data = drv_pin_get_port_data(number);
    uint32_t exti_line = number & 0x0f;
    gpio_init_type gpio_init_struct;
    exint_init_type exint_init_struct;

    /* Check pin is valid */
    if (pin_port_data == NULL)
    {
        printf("pin_port_data is NULL.\n");
        return -EINVAL;
    }
    printf("%s:%d number:%d mode:%d cfg:%d\n", __func__, __LINE__, number, cfg.mode, cfg.mux_cfg);
    gpio_default_para_init(&gpio_init_struct);
    exint_default_para_init(&exint_init_struct);

    /* Configure clock */
    if (pin_port_data == GPIOA)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOB)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOC)
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOD)
    {
        crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOE)
    {
        crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOF)
    {
        crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOG)
    {
        crm_periph_clock_enable(CRM_GPIOG_PERIPH_CLOCK, TRUE);
    }
    if (pin_port_data == GPIOH)
    {
        crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK, TRUE);
    }
    switch (cfg.mode)
    {
    case MK_PIN_MODE_NONE:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        break;
    }
    case MK_PIN_MODE_OUTPUT:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        break;
    }
    case MK_PIN_MODE_OUTPUT_OD:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        break;
    }
    case MK_PIN_MODE_INPUT:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        break;
    }
    case MK_PIN_MODE_INPUT_DOWN:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
        break;
    }
    case MK_PIN_MODE_INPUT_UP:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        break;
    }
    case MK_PIN_MODE_MUX_OUTPUT:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_pin_mux_config(pin_port_data, number % 16, cfg.mux_cfg);
        break;
    }
    case MK_PIN_MODE_MUX_OUTPUT_OD:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_pin_mux_config(pin_port_data, number % 16, cfg.mux_cfg);
        break;
    }
    case MK_PIN_MODE_IRQ_RISING:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
        break;
    }
    case MK_PIN_MODE_IRQ_FALLING:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        exint_init_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
        break;
    }
    case MK_PIN_MODE_IRQ_EDGE:
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        exint_init_struct.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
        break;
    }
    default:
    {
        return -EINVAL;
    }
    }

    if (cfg.mode >= MK_PIN_MODE_IRQ_RISING)
    {
        crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
        scfg_exint_line_config(number / 16, number % 16);

        exint_init_struct.line_enable = TRUE;
        exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
        exint_init_struct.line_select = (1 << (number / 16));
        exint_init(&exint_init_struct);
        // nvic_irq_enable(EXINT0_IRQn, 1, 0);TODO:这里需要在内核设置
    }

    pin->pins[number] = cfg;
    gpio_init_struct.gpio_pins = (1 << (number % 16));
    gpio_init(pin_port_data, &gpio_init_struct);
    return 0;
}
static int pin_read(mk_pin_t *pin, int number, uint8_t *value)
{
    assert(pin);
    assert(value);
    gpio_type *pin_port_data = drv_pin_get_port_data(number);
    if (!pin_port_data)
    {
        return -EINVAL;
    }
    *value = !!gpio_input_data_bit_read(pin_port_data, (1 << (number % 16)));
    return 0;
}
static int pin_write(mk_pin_t *pin, int number, uint8_t value)
{
    assert(pin);
    gpio_type *pin_port_data = drv_pin_get_port_data(number);
    if (!pin_port_data)
    {
        return -EINVAL;
    }
    gpio_bits_write(pin_port_data, (1 << (number % 16)), !!value);
    return 0;
}

static mk_pin_ops_t pin_ops = {
    .drv_pin_configure = pin_configure,
    .drv_pin_read = pin_read,
    .drv_pin_write = pin_write,
};
static mk_pin_dev_t pin_dev = {
    .ops = &pin_ops,
};

static int pin_probe(mk_dev_t *dev)
{
    int ret;

    /* ioremap */
    ret = dev_regs_map(dev, dev->dtb);
    if (ret < 0)
    {
        printf("pin dev regs map failed.\n");
    }
    /* 注册pin设备 */
    mk_pin_register(dev, &pin_dev);
    return 0;
}

static mk_drv_compatible_t pin_drv_compatilbe[] =
    {
        {"at32f43x,pin"},
        {NULL},
};

static mk_drv_t pin_drv =
    {
        .compatilbe = pin_drv_compatilbe,
        .probe = pin_probe,
        .data = NULL,
};
void drv_pin_init(void)
{
    mk_drv_register(&pin_drv);
}
