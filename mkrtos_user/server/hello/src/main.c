
#include <printf.h>
#include <unistd.h>
int main(int argc, char *args[])
{
    mk_printf("print test0.\n");
    mk_printf("print test1.\n");
    mk_printf("print test2.\n");
    float a = 1.1;
    float b = 1.2;
    float c;

    while (1)
    {
        c = a + b;
        // mk_printf("%c %d %f\n", 'a', 1234, 1.1);
        // mk_printf("%c %d %lf\n", 'a', 1234, a * b);
        mk_printf("%c %d %d\n", 'a', 1234, (int)1.1);
        mk_printf("%c %d %d\n", 'a', 1234, (int)(a * b));
        sleep(1);
    }
    return 0;
}
