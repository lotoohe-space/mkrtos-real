#include <arm_gicv2.h>

static gic_t m_irq;


gic_t *arm_gicv2_get_global(void)
{
    return &m_irq;
}

