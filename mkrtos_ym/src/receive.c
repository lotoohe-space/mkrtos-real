#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "ymodem.h"
#include "serial.h"

#define RESEND 0
#define SENDED 1
#define PACKET_SIZE 128

enum STATE {
	START, RECEIVE_DATA, END, STOP
};
#define PRINTF
static char buf[150] = { 0 };

/**
 * @brief  Update CRC16 for input byte
 * @param  CRC input value
 * @param  input byte
 * @retval Updated CRC value
 */
uint16_t update_CRC16(uint16_t crcIn, uint8_t byte) {
	uint32_t crc = crcIn;
	uint32_t in = byte | 0x100;

	do {
		crc <<= 1;
		in <<= 1;

		if (in & 0x100) {
			++crc;
		}

		if (crc & 0x10000) {
			crc ^= 0x1021;
		}
	} while (!(in & 0x10000));

	return (crc & 0xffffu);
}

/**
 * @brief  Cal CRC16 for YModem Packet
 * @param  data
 * @param  length
 * @retval CRC value
 */
static inline uint16_t cal_CRC16(const uint8_t *data, uint32_t size) {
	uint32_t crc = 0;
	const uint8_t *dataEnd = data + size;

	while (data < dataEnd) {
		crc = update_CRC16(crc, *data++);
	}
	crc = update_CRC16(crc, 0);
	crc = update_CRC16(crc, 0);

	return (crc & 0xffffu);
}

int ymodem_receive(const char *path, int fd) {
	int fd_file;
	char start_char, tmp_char;
	int state, len, i;
	int trans_end = 0;
	int ret;
	char packet_num;
	int packet_size;
	int file_size = 0;
	int eot_cn = 0;
	uint16_t crc;
	uint16_t recv_crc;

	fd_file = open(path, O_RDWR | O_CREAT |O_TRUNC, 0777);
	if (fd_file < 0) {
		perror("open() file");
		return-1;
	}
	while(try_get_char(fd,&tmp_char)>0);
	packet_num = 0;
	packet_size = 0;
	state = START;
	while (1) {

		if (trans_end == 1)
			break;
		switch (state) {
		case START:
			again:
			ret = try_get_char(fd, &start_char);
			if (ret == -EAGAIN) {
				sleep(2);
				put_char(fd, 'C');
				goto again;
			}
			if (start_char == SOH || start_char == STX) {
				tmp_char = get_char(fd);
				if (tmp_char != 0x00)
					return -1; //not 00

				tmp_char = get_char(fd);
				if (tmp_char != 0xff) {
					return -1; //not ff
				}
				int r_b=0;
				while(r_b<PACKET_SIZE){
					int ret;
					ret = get_bytes(fd,buf+r_b,PACKET_SIZE - r_b);
					if(ret == -EAGAIN) {
						continue;
					}
					r_b+=ret;
				}
				crc= cal_CRC16((uint8_t*)buf,PACKET_SIZE);

				//crc
				recv_crc = get_char(fd) << 8;
				recv_crc |= get_char(fd);
				if(recv_crc != crc) {
					put_char(fd, NAK);
					goto again;
				}else {
					put_char(fd, ACK);
					state = RECEIVE_DATA;
				}
				//get name and file size
				PRINTF("receive file name: %s, length :%d\n", buf, strlen(buf));
				//PRINTF("file size: %s\n", (buf + strlen(buf) + 1));
				file_size = atoi((buf + strlen(buf) + 1));
				PRINTF("file size: %d\n", file_size);

				if (ioctl(fd_file, 1, (file_size + 1023)&(~1023)) < 0) {
					//perror("file CT mode set error.\n");
				}
				put_char(fd, 'C');
			}else {
				goto again;
			}
			packet_num = 1;
			break;
		case RECEIVE_DATA:
			start_char = get_char(fd);
			switch (start_char) {
			case EOT:
				eot_cn++;
				if (eot_cn == 1) {
					put_char(fd, NAK);
					continue;
				} else if (eot_cn == 2) {
					state = END;
					put_char(fd, ACK);
				}
				break;
			case STX:
				packet_size = 1024;
				break;
			case SOH:
				packet_size = 128;
				break;
			case CAN:
				break;
			default:
				break;
			}
			if (state == END)
				break;

			tmp_char = get_char(fd);
			tmp_char = get_char(fd);

			int r_b=0;

			while(r_b<PACKET_SIZE){
				int ret;

				ret = get_bytes(fd,buf+r_b,PACKET_SIZE - r_b);
				if(ret == -EAGAIN) {
					continue;
				}
				r_b+=ret;
			}
			crc= cal_CRC16(buf,PACKET_SIZE);
			//crc
			recv_crc = get_char(fd) << 8;
			recv_crc |= get_char(fd);
			if(recv_crc!=crc) {
				while(try_get_char(fd,&tmp_char)>0);
				put_char(fd, NAK);
			}else {
				if (file_size < packet_size) {
					len = write(fd_file, buf, file_size);
					file_size -= len;
				} else {
					write(fd_file, buf, PACKET_SIZE);
					file_size -= PACKET_SIZE;
				}
				put_char(fd, ACK);
			}
			//state = END;
			break;
		case END:
			put_char(fd, 'C');

			start_char = get_char(fd);
			switch (start_char) {
			case STX:
				packet_size = 1024;
				break;
			case SOH:
				packet_size = 128;
				break;
			case CAN:
				break;
			default:
				break;
			}
			//if (state == ) break;

			tmp_char = get_char(fd);
			tmp_char = get_char(fd);

			for (i = 0; i < packet_size; i++) {
				buf[i] = get_char(fd);
			}

			tmp_char = get_char(fd);
			tmp_char = get_char(fd);

			put_char(fd, ACK);
			state = STOP;
			break;
		case STOP:
			put_char(fd, 'O');
			trans_end = 1;
			break;
		}
	}
	close(fd_file);

	return 0;
}

int main(int argc, char **argv) {
	int fd;
	int ret;
	if (argc < 3){
		return -1;
	}
	char *path = argv[2];
	ret = 0;
	printf("recv file is %.\n",path);
	fd = open("/dev/tty0", O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("open()");
		exit(1);
	}

	if (set_serial(fd) < 0) {
		PRINTF("set_serial() failed!\n");
		close(fd);
	}

	ret = ymodem_receive(path, fd);
	if (ret < 0) {
		PRINTF("ymodem_receive() error\n");
	}

	close_serial(fd);
	printf("ymodem end.\n");

	return 0;
}
