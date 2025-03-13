#ifndef __YMODEM_H__
#define __YMODEM_H__

#define HEAD 1
#define DATA 0

enum mode {
        SOH = 0x01,
        STX,
        EOT = 0x04,
        ACK = 0x06,
        NAK = 0x15,
        CAN = 0x18,
        C   = 0x43
};

struct mcu_hex {
	unsigned char *name;
	const unsigned char *path;
	unsigned int size;
	unsigned char *idx;
	unsigned char *idx_save;
}; 

struct packet {
	int fd; 		//serial file descripter

	unsigned int size; 	//packet size 128 or 1024
	unsigned char head;	//1: head packet 0:data packet
	unsigned char sno;	//packet number
	unsigned char *buf;	//packet data buffer
	unsigned int crc1;	//crc check code
	unsigned int crc2;
	
	struct mcu_hex *file;	//mcu hex file
};

unsigned int get_file_size(const char *);

#endif
