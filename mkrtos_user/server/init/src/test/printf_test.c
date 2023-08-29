#include "u_types.h"
#include <stdio.h>
void printf_test(void)
{
    printf("print test0.\n");
    printf("print test1.\n");
    printf("print test2.\n");
    float a = 1.1;
    float b = 1.2;
    float c = a + b;
    // printf("%c %d %f\n", 'a', 1234, 1.1);
    // c = c;
    // printf("%c %d %lf\n", 'a', 1234, 1.1); 浮点打印有问题
}