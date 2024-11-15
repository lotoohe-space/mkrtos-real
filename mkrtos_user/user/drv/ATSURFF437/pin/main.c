
#include <stdio.h>
#include <u_vmam.h>
#include <at32f435_437_conf.h>
#include <at_surf_f437_board_lcd.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <mk_dtb_parse.h>
#include <mk_dev.h>
#include <mk_drv.h>
#include "fs_rpc.h"
#include "ns_cli.h"
#include <assert.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;

    printf("pin init..\n");
#if 1
    thread_run(-1, 3);
#endif
    mk_drv_init();
    mk_dev_init();
    extern void drv_pin_init(void);
    drv_pin_init();
    dtb_parse_init();

    ret = rpc_meta_init(THREAD_MAIN, &hd);
    assert(ret >= 0);
    fs_svr_init();
    // mkdir("/dev", 0777);
    ns_register("/pin", hd, FILE_NODE);
    while (1)
    {
        fs_svr_loop();
    }
}
