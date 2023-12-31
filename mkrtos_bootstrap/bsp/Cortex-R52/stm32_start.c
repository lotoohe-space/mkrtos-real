
#include <mk_sys.h>
extern void enable_mpu(void);
extern void enable_caches(void);
void jump2kernel(void)
{
    enable_mpu();
    enable_caches();
    while(1);
}
