#ifndef __SERIAL_H__
#define __SERIAL_H__

#define PACKET_128 128
#define PACKET_1k 1024

char get_char(int fd);

int put_char(int , const unsigned char );

int get_bytes(int fd, char *buf, int len);

int try_get_char(int fd, char *res_char);

int set_serial(int );

void close_serial(int );

#endif
