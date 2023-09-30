
#include "u_types.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_sleep.h"
#include "led.h"
#include "relay.h"
#include "soft_iic.h"
#include "spl06.h"
#include <assert.h>

void drv_init(void)
{
    msg_tag_t tag;
    u_sleep_init();

    tag = mm_align_alloc(MM_PROT, (void *)0x40000000, 0x50000000 - 0x40000000);
    assert(msg_tag_get_val(tag) >= 0);

    led_init();
    relay_init();
    IIC_Init();
    spl0601_init();
}
