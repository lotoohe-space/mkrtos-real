
#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"

void irq_test(void)
{
    obj_handler_t obj = handler_alloc();
    factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, obj));
    uirq_bind(obj, 38);
    uirq_wait(obj);
    uirq_ack(obj, 38);
}