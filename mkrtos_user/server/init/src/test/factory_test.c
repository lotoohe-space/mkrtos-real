#include "u_types.h"
#include "u_prot.h"
#include "u_factory.h"
#include "u_task.h"
void factory_test(void)
{
    msg_tag_t tag = factory_create_ipc(FACTORY_PROT, 12);
    if (msg_tag_get_prot(tag) < 0)
    {
        printf("factory_create_ipc no memory\n");
        return;
    }
    task_unmap(TASK_PROT, 12);
}
