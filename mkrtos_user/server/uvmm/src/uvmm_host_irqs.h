#pragma once

#include "u_types.h"

enum vmm_irq_return
{
    VMM_IRQ_NONE = (0 << 0),
    VMM_IRQ_HANDLED = (1 << 0),
};
typedef enum vmm_irq_return vmm_irq_return_t;

typedef umword_t irq_flags_t;
