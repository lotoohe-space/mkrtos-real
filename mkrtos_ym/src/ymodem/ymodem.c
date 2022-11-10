#include 	 "ymodem.h"

int fd;
char CrcFlag=0;
unsigned int FileLen=0;
unsigned int FileLenBkup=0;

#ifndef WINYWY

int uart_test(const char *device)
{
	int fd;
	int len;
	char data;

	if((fd=_open(device, _O_RDWR|_O_BINARY|_O_NONBLOCK ))==-1)
			return -1;


	for(;;)
	{
		if((len=_read(fd,&data,1))==-1)
		{
			if(errno==EAGAIN)
				continue;

			_close(fd);

			return -1;
		}
		_write(fd,&data,1);
	}
	_close(fd);
	return 0;
}

int main(int argc,char* argv[])
{


	int i;
	char bt;
	char* fp;
	int trans_ind;
	int set_ind;
	char* device;

	device="/dev/tty0";//serial port 2
	
	set_ind=set_com_common(device);
	if(set_ind)
	{
		if(set_ind==COM_OPEN_ERR)
			printf("sorry,I can't open the serial port!\n");
		else
			printf("sorry,error ocurred when set the searial port!\n");
		exit(1);
	}
	switch(*argv[1])
	{
		case 'r':
			trans_ind=control_recv(device);
			break;
		case 's':
			fp=argv[2];
			if(fp==NULL)
			{
				printf("please input file name!\n");
				exit(1);
			}
			trans_ind=control_send(fp,device);
			break;
		case 't':
			trans_ind=uart_test(device);
			break;
		default :
			printf("please use 'r' or 's fname' cmd to receive or send file\n");
			return 0;
	}

	if(trans_ind)
	{
		printf("sorry! get error in transmission!ERR ID:0x%x\n",trans_ind);
		if(trans_ind==COM_OPEN_ERR)
		{
			printf("sorry,I can't open the serial port!\n");
		}
		else
		{
			if((fd=_open(device, _O_RDWR|_O_BINARY|_O_NONBLOCK ))==-1)
			{
				printf("sorry,I can't open the serial port!\n");
			}
			else
			{
				bt=(char)CAN;
				if(_write(fd, &bt,1)==-1) //request sender to send file with CRC check
				{
					_close(fd);
					exit(1);
				}
				if(_write(fd, &bt,1)==-1) //request sender to send file with CRC check
				{
					_close(fd);
					exit(1);
				}
				delay(10000000);
			}
			_close(fd);
			exit(1);
		}
		
	}
	return 0;

}

#else
int main()
{
	char* fp;
	int trans_ind;
	char* device;

	device="COM1";
	fp="receive.c";

	set_com_common(device);
	trans_ind=control_send(fp,device);
	
	if(trans_ind)
	{
		printf("trans_ind:0x%x\n",trans_ind);
		printf("sorry! get error in transmission\n");
	}
	return 0;
}
#endif

int set_com_common(char* device) //common for receive and send
{
	
#ifndef WINYWY
	struct termios Opt;

	if((fd=open(device, O_RDWR))==-1)
		return(COM_OPEN_ERR);
	
	tcgetattr(fd, &Opt);
	cfsetispeed(&Opt,B115200);     //115200Bps
    Opt.c_lflag &= ~ICANON;
	Opt.c_lflag &= ~IEXTEN;
	Opt.c_lflag &= ~ISIG;
	Opt.c_lflag &= ~ECHO;

	Opt.c_iflag = 0;
	
	Opt.c_oflag = 0;


	if (tcsetattr(fd,TCSANOW,&Opt) != 0)   
	{ 
		return (COM_SET_ERR);  
	} 
	close(fd);
#endif
	
	return 0;
}


void delay(int clock_count)
{
	int i;
	for(i=0;i<clock_count;i++)  //delay
	{
	}
}

