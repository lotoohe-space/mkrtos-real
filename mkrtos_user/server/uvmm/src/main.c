
#include <printf.h>
#include <unistd.h>
#include <u_types.h>
#include "guest_os.h"
#include "u_task.h"
#include <stdio.h>
#include "uvmm_dev_man.h"
extern umword_t os_bin;
extern umword_t os_bin_end;
extern umword_t uvmm_dev_json;
static guest_os_t gos;
int main(int argc, char *args[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_hello");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_hello");

    uvmm_dev_simul_init();

    mk_printf("guest os entry addr:[0x%lx 0x%lx]\n", &os_bin, &os_bin_end);
    guest_os_create(&gos, (char *)&uvmm_dev_json,
                    0x20000000, &os_bin, &os_bin_end, 32 * 1024 * 1024);
    while (1)
    {
        sleep(1);
    }
    // mk_printf("print test0.\n");
    // mk_printf("print test1.\n");
    // mk_printf("print test2.\n");
    // float a = 1.1;
    // float b = 1.2;
    // float c;

    // while (1)
    // {
    //     c = a + b;
    //     // mk_printf("%c %d %f\n", 'a', 1234, 1.1);
    //     // mk_printf("%c %d %lf\n", 'a', 1234, a * b);
    //     mk_printf("%c %d %d\n", 'a', 1234, (int)1.1);
    //     mk_printf("%c %d %d\n", 'a', 1234, (int)(a * b));
    //     sleep(1);
    // }
    return 0;
}
