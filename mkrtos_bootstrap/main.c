

#include <types.h>
#include <mk_sys.h>
extern char cpio_start[];
extern char cpio_end[];
int main(void)
{      
    jump2kernel((addr_t)cpio_start, (addr_t)cpio_end);
    while (1)
        ;
}
