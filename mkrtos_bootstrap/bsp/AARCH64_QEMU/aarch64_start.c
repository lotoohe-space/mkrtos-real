
#include <mk_sys.h>
#include <elf_loader.h>
#include <uart.h>
#include "cpio.h"

#define KERNEL_NAME "mkrtos.elf"

__ALIGN__(16)
uint64_t stack[4096 / sizeof(uint64_t)];
extern uint64_t knl_entry; // 内核的启动地址，其它核跳转使用
typedef void (*main_func)(umword_t cpio_addr);

void jump2kernel(addr_t cpio_start, addr_t cpio_end)
{
    uart_init();
    uart_write_str("bootstrap...\n");
    umword_t knl_st_addr = cpio_find_file((umword_t)cpio_start, (umword_t)cpio_end, KERNEL_NAME);
    if (!knl_st_addr)
    {
        while (1)
            ;
    }
    Elf64_Addr entry = 0;

    elf_load_knl(knl_st_addr, &entry);
    if (entry == 0) {
        uart_write_str("not find knl image...\n");
    }
    main_func entry_fun = (main_func)entry;
    knl_entry = entry;
    entry_fun((mword_t)cpio_start);

    while (1)
        ;
}
