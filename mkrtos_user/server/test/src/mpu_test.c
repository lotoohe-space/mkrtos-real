#include "u_types.h"
#include <stdio.h>
void mpu_test(void)
{
    printf("start mpu test...\n");
    // mpu保护测试
    int *test = ((int *)CONFIG_KNL_DATA_ADDR);
    *test = 1;
    printf("cpu don't support mpu.\n");
}
