
#include <stdio.h>
#include <mkrtos_su.h>

int test_printf(void)
{
    double p = 3.141592653589793;

    printf("test done:%c %s %d %x %.30f\n", 'a', "hello", 0x123, 0x123, p);

    return 0;
}

int main(int argc, char *args[])
{
    test_printf();
    // fcall(0, 0, 0, 0);
    return 0;
}
