/*
 *  send.c - serial ymodem sender
 *  
 *  Copyright (c) 2012 April Jianzhong.Cui <jianzhong.cui@cs2c.com.cn>
 *
 */
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
#include "ymodem.h"

#define DEV "/dev/ttyUSB0"

int init_packet(struct packet *p, int fd, const char *path)
{
	struct packet *pkt;
	pkt = p;
	
	pkt->fd = fd;
	pkt->size = PACKET_128;
	pkt->head = 1;
	pkt->sno = 0x0;
	pkt->buf = malloc(pkt->size);
	pkt->crc1 = 0;
	pkt->crc2 = 0;
	
	pkt->file = malloc(sizeof(struct mcu_hex));
	pkt->file->path = path;
	pkt->file->name = strrchr(path, '/') + 1;
	if (strlen(pkt->file->name) > 128)
		return -1;
	pkt->file->size = get_file_size(path);
	pkt->file->idx = NULL;
	pkt->file->idx_save = NULL;

	return 0;
}

void packet_send(struct packet *p)
{
	int i, j, crc = 0;
	printf("\033[45;37m--> \033[0m\n");
	if (p->size == PACKET_128) {
		put_char(p->fd, SOH);
		printf("\033[35mSOH ");
	} else if (p->size == PACKET_1k) {
		put_char(p->fd, STX);
		printf("\033[35mSTX ");
	} else 	
		printf("packet size is neither 128 bits nor 1K bits\n");

	put_char(p->fd, p->sno);
	printf("%x ", p->sno);
	put_char(p->fd, 255 - p->sno);
	printf("%x ", 255 - p->sno);
	p->sno++;

	if (p-> head == HEAD) {
		for(i = 0; i < p->size; i++) {
			put_char(p->fd, p->buf[i]);
			printf("%c", p->buf[i]);
			if (i == strlen(p->file->name)) printf(" ");
			crc = crc ^ (int)p->buf[i] << 8;
			for (j = 0; j < 8; j++)
				if (crc & 0x8000)
					crc = crc << 1 ^ 0x1021;
				else
					crc = crc << 1;
		}
		p->crc1 = (crc >> 8) & 0xff;
		p->crc2 = crc & 0xff;
		put_char(p->fd, p->crc1);
		put_char(p->fd, p->crc2);

	} else if (p->head == DATA) {
		//printf("data packet -->");	
		for(i = 0; i < p->size; i++) {
			put_char(p->fd, p->buf[i]);
			//printf("%c", p->buf[i]);
			//if (i == strlen(p->file->name)) printf(" ");
			crc = crc ^ (int)p->buf[i] << 8;
			for (j = 0; j < 8; j++)
				if (crc & 0x8000)
					crc = crc << 1 ^ 0x1021;
				else
					crc = crc << 1;
		}
		p->crc1 = (crc >> 8) & 0xff;
		p->crc2 = crc & 0xff;
		put_char(p->fd, p->crc1);
		put_char(p->fd, p->crc2);
	} else
		printf("\033[31mpacket not head or data!\033[0m\n");

	printf("%x ", p->crc1);
	printf("%x \033[0m\n", p->crc2);
}

int packet_xfr(struct packet *p)
{
	int fd, len, i, end;
	char c;
	//printf("\033[46;37msending first packet !i\033[0m\n");
	if (p->head == HEAD) {
		p->size = PACKET_128;
		p->buf = malloc(p->size);
		if (p->buf == NULL) {
			perror("malloc()");
			return -1;
		}
		/*fix me: if name or size is too long */

		strncpy(p->buf, p->file->name, strlen(p->file->name)+1);
		//printf("name: %s\n", p->buf);
		sprintf(p->buf + strlen(p->file->name)+1, "%d", p->file->size);
		//printf("size: %s\n", p->buf + strlen(p->file->name)+1);

		packet_send(p);
	} else if (p->head == DATA) {
		if (p->file->size > PACKET_1k)
			p->size = PACKET_1k;
		else
			p->size = PACKET_128;
	
		p->buf = realloc(p->buf, p->size);
		if (p->buf == NULL) {
			perror("realloc()");
			return -1;
		}

		fd = open(p->file->path, O_RDWR);
		if (fd < 0) {
			perror("open() mcu_hex file");
			exit(1);
		}

		end = 0;
		while(1) {
			if(end == 1) break;
			switch (c = get_char(p->fd)) {
				case 'C': 
				case ACK: 
					printf("					\033[42;37m<--  ACK\033[0m\n");	
					len = read(fd, p->buf, p->size);
					if (p->file->size <= 0) {
						i = 0;
						do{
							put_char(p->fd, EOT);
							c=get_char(p->fd); /* Flush the NCK */
							if(c == ACK)
								break;
							i++;
						} while(i < 3);

						c = get_char(p->fd);
						if (c == 'C' || c == 'c') {
							printf("					\033[42;37m<--  C\033[0m\n");	
							p->size = PACKET_128;
							for(i=0; i < p->size; i++)
								p->buf[i]=0;
							p->sno=0;
							packet_send(p); 
							c = get_char(p->fd);
							if (c == ACK) {
								printf("					\033[42;37m<--  ACK\033[0m\n");	
								end = 1;
								break;
							}
						}

					} 
					p->file->size -= len;
					packet_send(p); 
					break;
				case NAK:
					packet_send(p); 
					break;
				default: break;
			}
		}
		close(fd);
	} else {
		printf("\033[31mpacket error\033[0m\n");
		return -1;
	}
	
	printf("successful!\n");
	return 0;
}

int ymodem_xfr(int fd, const char *path)
{
	struct packet pkt;
	 
	printf("      	  \033[34mSENDER\033[0m			    \033[34mMCU\033[0m\n");
	init_packet(&pkt, fd, path);
	
	if ('C' == get_char(pkt.fd)) {
		printf("					\033[42;37m<--  C\033[0m\n");	
		pkt.head = HEAD;
		packet_xfr(&pkt);
		if (ACK == get_char(pkt.fd)){
			pkt.head = DATA;
			packet_xfr(&pkt);
		}
	}

	free(pkt.file);
	free(pkt.buf);

	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	const char *file_path = "./install.log";
	//const char *file_path = "./hs_andorid_mcu.hex";
	
        fd = open(DEV, O_RDWR);
        if (fd < 0) {
                perror("open()");
                exit(1);
        }
	
	set_serial(fd);

	//put_char(fd, 'C');
	ymodem_xfr(fd, file_path);

        close_serial(fd);
        return 0;
}
