#include "u_types.h"
#include <stdio.h>
void mpu_test(void)
{

#if 1
    // mpu保护测试
    int *test = ((int *)80000000);
    *test = 1;
    printf("test is %d\n", *test);
#endif
}
