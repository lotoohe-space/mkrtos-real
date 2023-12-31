

#include <types.h>
#include <mk_sys.h>
int main(void)
{
    jump2kernel();
    while (1)
        ;
}
