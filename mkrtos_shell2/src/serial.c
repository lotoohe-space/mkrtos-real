#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "serial.h"

int put_char(int fd, unsigned char c)
{
        int ret = 0;

        again:
        ret = write(fd, &c, 1);
        if (ret < 0) {
        	if(errno==EAGAIN)
			{
				goto again;
			}
			printf("write() in put_char(), errno:%d\n",errno);
        }

        return ret;
}
int get_bytes(int fd, char *buf, int len)
{
	int ret = read(fd, buf, len);
	if (ret<0){
		ret=-errno;
	}
	return ret;
}
int try_get_char(int fd, char *res_char)
{
	int ret = read(fd, res_char, 1);
	if (ret<0){
		ret=-errno;
	}
	return ret;
}
char get_char(int fd)
{
	int len = 0;
	char c;

	again:
	len = read(fd, &c, 1);
	if (len < 0) {
		if(errno==EAGAIN)
		{
			goto again;
		}
		perror("read() in get_char()");
	}
	return c;
}

int set_serial(int fd)
{
        struct termios opt;

        tcgetattr(fd, &opt);
        tcflush(fd, TCIOFLUSH);

    	cfsetispeed(&opt,B115200);     //115200Bps
    	opt.c_lflag &= ~ICANON;
    	opt.c_lflag &= ~IEXTEN;
    	opt.c_lflag &= ~ISIG;
    	opt.c_lflag &= ~ECHO;

    	opt.c_iflag = 0;

    	opt.c_oflag = 0;


        if (tcsetattr(fd, TCSANOW, &opt) == -1) {
                perror("tcsetattr()");
                return -1;
        }

        return 0;
}

void close_serial(int fd)
{
        close(fd);
}

unsigned int get_file_size(const char *path)
{
        unsigned long filesize = -1;
        struct stat statbuff;
        if(stat(path, &statbuff) < 0){
                return filesize;
        }else{
                filesize = statbuff.st_size;
        }
#if 0
        FILE *fp;
        fp = fopen(path, "r");
        if (fp == NULL) 
                return filesize;
        fseek(fp, 0L, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
#endif
        return filesize;
}

