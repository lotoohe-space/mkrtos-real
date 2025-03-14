
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
int a,b;
// extern int add(int a, int b);
int add(int a,int b)
{
    return a+b;
}
int main(int argc, char *argv[])
{
    a=10;
    b=10;
    printf("test,%d\n", a+b);
    // return add(10, 2);
    return add(a,b);
}
