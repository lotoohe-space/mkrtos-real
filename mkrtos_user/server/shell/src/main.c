
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <shell_port.h>
int main(int argc, char *args[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, args[i]);
    }
    userShellInit();
    return 0;
}
