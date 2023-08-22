
#include "u_log.h"
#include <stdio.h>
// char ad = 'a';
void ulog_test(void)
{
    ulog_write_str("Init task running..\n");
    ulog_write_str("todo..\n");
    // char data[2] = {ad,0};
    // ulog_write_str(data);
}
void printf_test(void)
{
    printf("print test0.\n");
    printf("print test1.\n");
    printf("print test2.\n");
}
int main(int argc, char *args[])
{
    ulog_test();
    printf_test();
    while (1)
        ;
    return 0;
}
