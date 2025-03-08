
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <u_vmam.h>
#include <u_sleep.h>
#include <termios.h>
#include <sys/stat.h>
extern int vi_main(int argc, char **argv);
static struct termios old_settings;
static struct termios new_settings;
int main(int argc, char *argv[])
{
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);   // 禁用规范模式和回显
    new_settings.c_cc[VMIN] = 1;                // 读取的最小字符数
    new_settings.c_cc[VTIME] = 0;               // 读取的超时时间（以10ms为单位）
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    printf("%s running..\n", argv[0]);
    return vi_main(argc, argv);
}
