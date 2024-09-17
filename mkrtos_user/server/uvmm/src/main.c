
#include "guest_os.h"
#include "u_task.h"
#include "uvmm_dev_man.h"
#include <printf.h>
#include <stdio.h>
#include <u_types.h>
#include <unistd.h>

#include <u_util.h>

extern const char benos_bin[], _sizeof_benos_bin[];
extern const char benos_json[], _sizeof_benos_json[];

extern umword_t uvmm_dev_json;
static guest_os_t gos;
int main(int argc, char *args[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_hello");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_hello");

    uvmm_dev_simul_init();

    mk_printf("guest os entry addr:[0x%lx 0x%lx]\n", benos_bin, benos_bin + (umword_t)_sizeof_benos_bin);
    guest_os_create(&gos, (char *)benos_json,
                    0x20000000, (void*)benos_bin, (void*)(benos_bin + (umword_t)_sizeof_benos_bin), 32 * 1024 * 1024);
    while (1) {
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


IMPORT_BIN(".rodata", "/Users/zhangzheng/mkrtos-real/mkrtos_user/server/uvmm/src/benos.bin", benos_bin);
IMPORT_BIN(".rodata", "/Users/zhangzheng/mkrtos-real/mkrtos_user/server/uvmm/src/vm_benos.json", benos_json);
