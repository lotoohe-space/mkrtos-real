//
// Created by Administrator on 2022/2/11/011.
//

#ifndef UNTITLED1_TTY_H
#define UNTITLED1_TTY_H
#include <sys/types.h>
#include <termios.h>
#include <mkrtos/fs.h>
#include <mkrtos/sched.h>
#include <ipc/spin_lock.h>
//buf����
#define TTY_READ_BUF_LEN 512

struct tty_queue {
	//��ȡ�������û��ζ���
	uint8_t read_buf[TTY_READ_BUF_LEN];     //��buf����
	int32_t rear;
	int32_t front;
};
struct tty_struct;
//�����ǵײ�Ĵ�����
struct dev_ops {
	int32_t (*open)(struct tty_struct *tty, struct file *filp);
	void (*close)(struct tty_struct *tty, struct file *filp);
	int32_t (*write)(struct tty_struct *tty);
	int32_t (*ioctl)(struct tty_struct *tty, struct file *file, uint32_t cmd,
			uint32_t arg);
};
struct tty_struct {
	struct termios termios;	//!<��ǰʹ�õ��ն���Ϣ
	struct winsize w_size;
	int line_no;			//!<��ʹ�õ�line����
	struct dev_ops *d_ops;	//!<�豸��ops
	/////
	//���ȴ�
	struct wait_queue *r_wait;
	//��ײ���������ȶ�ȡ������
	struct tty_queue r_queue;
	//д���ݵĻ���
	struct tty_queue w_queue;
	//Ȼ��ͨ��handler������ƴ�ŵ�per_queue�У�pre_queue�е�����ֱ�ӿ��Ը��û������߽��л���
	struct tty_queue pre_queue;

	struct spin_lock w_lock;

	//�ж�����
	int col;
	//�ж�����
	int row;
	//�ַ�����
	uint8_t is_error;

	//�Ƿ������
	uint8_t is_nl;
	//
//    uint8_t print_ctl;
	//�Ƿ�ʹ����
	uint8_t used_cn;
};

/*	intr=^C		quit=^|		erase=del	kill=^U
 eof=^D		vtime=\0	vmin=\1		sxtc=\0
 start=^Q	stop=^S		susp=^Z		eol=\0
 reprint=^R	discard=^U	werase=^W	lnext=^V
 eol2=\0
 */
//#define C_CC_INIT "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
#define C_CC_INIT "\003\034\b\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
//line����ṹ��
struct tty_line {
	//��ȡ����
	int32_t (*read)(struct tty_struct *tty, struct file *fp, uint8_t *buf,
			int32_t count);
	//д����
	int32_t (*write)(struct tty_struct *tty, struct file *fp, uint8_t *buf,
			int32_t count);
	//���ݴ�����
	void (*handler)(struct tty_struct *tty);
};

#define INTR_C(tty) ((tty)->termios.c_cc[VINTR])
#define QUIT_C(tty) ((tty)->termios.c_cc[VQUIT])
#define ERASE_C(tty) ((tty)->termios.c_cc[VERASE])
#define KILL_C(tty) ((tty)->termios.c_cc[VKILL])
#define EOF_C(tty) ((tty)->termios.c_cc[VEOF])
#define TIME_C(tty) ((tty)->termios.c_cc[VTIME])
#define MIN_C(tty) ((tty)->termios.c_cc[VMIN])
#define SWTC_C(tty) ((tty)->termios.c_cc[VSWTC])
#define START_C(tty) ((tty)->termios.c_cc[VSTART])
#define STOP_C(tty) ((tty)->termios.c_cc[VSTOP])
#define SUSP_C(tty) ((tty)->termios.c_cc[VSUSP])
#define EOL_C(tty) ((tty)->termios.c_cc[VEOL])
#define REPRINT_C(tty) ((tty)->termios.c_cc[VREPRINT])
#define DISCARD_C(tty) ((tty)->termios.c_cc[VDISCARD])
#define WERASE_C(tty) ((tty)->termios.c_cc[VWERASE])
#define LNEXT_C(tty)	((tty)->termios.c_cc[VLNEXT])
#define EOL2_C(tty) ((tty)->termios.c_cc[VEOL2])

#define _I_FLAG(tty,f)	((tty)->termios.c_iflag & (f))
#define _O_FLAG(tty,f)	((tty)->termios.c_oflag & (f))
#define _C_FLAG(tty,f)	((tty)->termios.c_cflag & (f))
#define _L_FLAG(tty,f)	((tty)->termios.c_lflag & (f))

#define I_IGNBRK(tty)	_I_FLAG((tty),IGNBRK)
#define I_BRKINT(tty)	_I_FLAG((tty),BRKINT)
#define I_IGNPAR(tty)	_I_FLAG((tty),IGNPAR)
#define I_PARMRK(tty)	_I_FLAG((tty),PARMRK)
#define I_INPCK(tty)	_I_FLAG((tty),INPCK)
#define I_ISTRIP(tty)	_I_FLAG((tty),ISTRIP)
#define I_INLCR(tty)	_I_FLAG((tty),INLCR)
#define I_IGNCR(tty)	_I_FLAG((tty),IGNCR)
#define I_ICRNL(tty)	_I_FLAG((tty),ICRNL)
#define I_IUCLC(tty)	_I_FLAG((tty),IUCLC)
#define I_IXON(tty)	_I_FLAG((tty),IXON)
#define I_IXANY(tty)	_I_FLAG((tty),IXANY)
#define I_IXOFF(tty)	_I_FLAG((tty),IXOFF)
#define I_IMAXBEL(tty)	_I_FLAG((tty),IMAXBEL)

#define O_OPOST(tty)	_O_FLAG((tty),OPOST)
#define O_OLCUC(tty)	_O_FLAG((tty),OLCUC)
#define O_ONLCR(tty)	_O_FLAG((tty),ONLCR)
#define O_OCRNL(tty)	_O_FLAG((tty),OCRNL)
#define O_ONOCR(tty)	_O_FLAG((tty),ONOCR)
#define O_ONLRET(tty)	_O_FLAG((tty),ONLRET)
#define O_OFILL(tty)	_O_FLAG((tty),OFILL)
#define O_OFDEL(tty)	_O_FLAG((tty),OFDEL)
#define O_NLDLY(tty)	_O_FLAG((tty),NLDLY)
#define O_CRDLY(tty)	_O_FLAG((tty),CRDLY)
#define O_TABDLY(tty)	_O_FLAG((tty),TABDLY)
#define O_BSDLY(tty)	_O_FLAG((tty),BSDLY)
#define O_VTDLY(tty)	_O_FLAG((tty),VTDLY)
#define O_FFDLY(tty)	_O_FLAG((tty),FFDLY)
#define O_XTABS(tty)    _O_FLAG((tty),XTABS)

#define __C_FLAG(flag,f)	((flag) & (f))

#define C_BAUD(tty)	    _C_FLAG((tty),CBAUD)
#define C_CSIZE(tty)	_C_FLAG((tty),CSIZE)
#define C_CSTOPB(tty)	_C_FLAG((tty),CSTOPB)
#define C_CREAD(tty)	_C_FLAG((tty),CREAD)
#define C_PARENB(tty)	_C_FLAG((tty),PARENB)
#define C_PARODD(tty)	_C_FLAG((tty),PARODD)
#define C_HUPCL(tty)	_C_FLAG((tty),HUPCL)
#define C_CLOCAL(tty)	_C_FLAG((tty),CLOCAL)
#define C_CIBAUD(tty)	_C_FLAG((tty),CIBAUD)
#define C_CRTSCTS(tty)	_C_FLAG((tty),CRTSCTS)
#define C_BAUDEX(tty)   _C_FLAG((tty),CBAUDEX)

#define F_C_BAUD(tty)	    __C_FLAG((tty),CBAUD)
#define F_C_CSIZE(tty)	__C_FLAG((tty),CSIZE)
#define F_C_CSTOPB(tty)	__C_FLAG((tty),CSTOPB)
#define F_C_CREAD(tty)	__C_FLAG((tty),CREAD)
#define F_C_PARENB(tty)	__C_FLAG((tty),PARENB)
#define F_C_PARODD(tty)	__C_FLAG((tty),PARODD)
#define F_C_HUPCL(tty)	__C_FLAG((tty),HUPCL)
#define F_C_CLOCAL(tty)	__C_FLAG((tty),CLOCAL)
#define F_C_CIBAUD(tty)	__C_FLAG((tty),CIBAUD)
#define F_C_CRTSCTS(tty)	__C_FLAG((tty),CRTSCTS)
#define F_C_BAUDEX(tty)   __C_FLAG((tty),CBAUDEX)

#define L_ISIG(tty)	    _L_FLAG((tty),ISIG)
#define L_ICANON(tty)	_L_FLAG((tty),ICANON)
#define L_XCASE(tty)	_L_FLAG((tty),XCASE)
#define L_ECHO(tty)	    _L_FLAG((tty),ECHO)
#define L_ECHOE(tty)	_L_FLAG((tty),ECHOE)
#define L_ECHOK(tty)	_L_FLAG((tty),ECHOK)
#define L_ECHONL(tty)	_L_FLAG((tty),ECHONL)
#define L_NOFLSH(tty)	_L_FLAG((tty),NOFLSH)
#define L_TOSTOP(tty)	_L_FLAG((tty),TOSTOP)
#define L_ECHOCTL(tty)	_L_FLAG((tty),ECHOCTL)
#define L_ECHOPRT(tty)	_L_FLAG((tty),ECHOPRT)
#define L_ECHOKE(tty)	_L_FLAG((tty),ECHOKE)
#define L_FLUSHO(tty)	_L_FLAG((tty),FLUSHO)
#define L_PENDIN(tty)	_L_FLAG((tty),PENDIN)
#define L_IEXTEN(tty)	_L_FLAG((tty),IEXTEN)

//tty_queue.c

static inline int32_t q_length(struct tty_queue *t_queue) {
	int32_t res ;
	uint32_t t;
	t = dis_cpu_intr();
	res = (t_queue->rear - t_queue->front + TTY_READ_BUF_LEN)
			% TTY_READ_BUF_LEN;
	restore_cpu_intr(t);
	return res;
}
static inline void q_clear(struct tty_queue *t_queue) {
	uint32_t t;
	t = dis_cpu_intr();
	t_queue->front = t_queue->rear = 0;
	restore_cpu_intr(t);
}

static inline int32_t q_add(struct tty_queue *t_queue, uint8_t d) {
	uint32_t t;
	t = dis_cpu_intr();
	if ((t_queue->rear + 1) % TTY_READ_BUF_LEN == t_queue->front) {
		restore_cpu_intr(t);
		return -1; //����
	}
	t_queue->read_buf[t_queue->rear] = d;
	t_queue->rear = (t_queue->rear + 1) % TTY_READ_BUF_LEN;
	restore_cpu_intr(t);
	return 0;
}
static inline int32_t q_add_dont_close_cpu(struct tty_queue *t_queue, uint8_t d) {
	if ((t_queue->rear + 1) % TTY_READ_BUF_LEN == t_queue->front) {
		return -1; //����
	}
	t_queue->read_buf[t_queue->rear] = d;
	t_queue->rear = (t_queue->rear + 1) % TTY_READ_BUF_LEN;
	return 0;
}
//���β�����ַ��Ƿ���ָ�����ַ�
static inline int32_t q_check_f(struct tty_queue *t_queue, uint8_t c) {
	uint32_t t;
	t = dis_cpu_intr();
	if (t_queue->rear == t_queue->front) { //�յ�
		restore_cpu_intr(t);
		return -1;
	}
	uint32_t tmp_rear = (t_queue->rear - 1 + TTY_READ_BUF_LEN)
			% TTY_READ_BUF_LEN;
	if (t_queue->read_buf[tmp_rear] == c) {
		restore_cpu_intr(t);
		return 0;
	}
	restore_cpu_intr(t);
	return -1;
}
//β��������
static inline int32_t q_get_f(struct tty_queue *t_queue, uint8_t *d) {
	uint32_t t;
	t = dis_cpu_intr();
	if (t_queue->rear == t_queue->front) { //�յ�
		restore_cpu_intr(t);
		return -1;
	}
	t_queue->rear = (t_queue->rear - 1 + TTY_READ_BUF_LEN) % TTY_READ_BUF_LEN;
	if (d) {
		*d = t_queue->read_buf[t_queue->rear];
	}
	restore_cpu_intr(t);
	return 0;
}
static inline int32_t q_get(struct tty_queue *t_queue, uint8_t *d) {
	uint32_t t;
	t = dis_cpu_intr();
	if (t_queue->rear == t_queue->front) {
		restore_cpu_intr(t);
		return -1;
	}
	if (d) {
		*d = t_queue->read_buf[t_queue->front];
	}
	t_queue->front = (t_queue->front + 1) % TTY_READ_BUF_LEN;
	restore_cpu_intr(t);
	return 0;
}
//void q_clear(struct tty_queue *t_queue);
//int32_t q_add(struct tty_queue *t_queue, uint8_t d);
//int32_t q_add_dont_close_cpu(struct tty_queue *t_queue, uint8_t d);
//int32_t q_get(struct tty_queue *t_queue, uint8_t *d);
//int32_t q_get_f(struct tty_queue *t_queue, uint8_t *d);
//int32_t q_check_f(struct tty_queue *t_queue, uint8_t c);

//uart.c
extern int32_t uart_open(struct tty_struct *tty, struct file *filp);

//serail.c
#define TTY_IO_SET 0x0001
#define TTY_IO_GET 0x0002
struct serial_struct {
	//���ڵĿ��ƣ���c_cflagһ��
	uint32_t c_cflag;
};

#endif //UNTITLED1_TTY_H
