#include "u_types.h"
#include <stdio.h>
AUTO_CALL(102)
void printf_test(void)
{
    printf("print test0.\n");
    printf("print test1.\n");
    printf("print test2.\n");
    printf("%s %d %f %x\n", "hello world.", 123, 123.123f, 0x123);
    float a = 1.1;
    float b = 1.2;
    float c;

    c = a + b;
    printf("%c %d %f\n", 'a', 1234, 1.1);
    c = c;
    printf("%c %d %lf\n", 'a', 1234, 1.1);

    int aa;
    printf("please input int val.\n");
    scanf("%d", &aa);
    printf("val is %d.\n", aa);
}
