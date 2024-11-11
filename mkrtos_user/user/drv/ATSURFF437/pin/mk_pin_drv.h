#pragma once

#include <u_types.h>
#include "mk_drv.h"
#include "mk_dev.h"
#define MK_PIN_MAX_NUM (16 * 8)

typedef struct mk_pin
{
    uint32_t pins[MK_PIN_MAX_NUM]; //!< 最大256引脚
} mk_pin_t;

typedef struct mk_pin_ops
{
    int (*drv_pin_configure)(mk_pin_t *pin, int number, int mode);
    int (*drv_pin_read)(mk_pin_t *pin, int number, uint8_t *value);
    int (*drv_pin_write)(mk_pin_t *pin, int number, uint8_t value);
} mk_pin_ops_t;

typedef struct mk_pin_dev
{
    mk_pin_t pins;
    mk_pin_ops_t *ops;
    int val;
} mk_pin_dev_t;

enum mk_pin_ioctl_op
{
    MK_PIN_SET_MODE,   //!< 设置模式
    MK_PIN_GET_MODE,   //!< 获取模式
    MK_PIN_SET_OP_PIN, //!< 设置要操作的引脚，标记读写的起始引脚
};

enum mk_pin_mode
{
    MK_PIN_MODE_NONE = 1,
    MK_PIN_MODE_OUTPUT,
    MK_PIN_MODE_OUTPUT_OD,
    MK_PIN_MODE_INPUT,
    MK_PIN_MODE_INPUT_DOWN,
    MK_PIN_MODE_INPUT_UP,

    MK_PIN_MODE_IRQ_RISING,
    MK_PIN_MODE_IRQ_FALLING,
    MK_PIN_MODE_IRQ_EDGE,
    MK_PIN_MODE_IRQ_LOW,
    MK_PIN_MODE_IRQ_HIGH,
};
typedef union mk_pin_cfg
{
    struct
    {
        uint16_t mode;
        uint16_t pin;
    };
    uint32_t cfg_raw;
} mk_pin_cfg_t;

int mk_pin_register(mk_dev_t *dev, mk_pin_dev_t *pin_dev);
