#pragma once

#include <termios.h>
#include "u_queue.h"
#include "u_types.h"
#include <u_mutex.h>
#define TTY_QUEUE_DATA_SIZE 257
typedef struct tty_struct
{
    struct termios termios; //!< 当前使用的终端信息
    struct winsize w_size;  //!< 窗口大小
    u_mutex_t lock_cons;
    u_mutex_t lock_write_cons;
    queue_t w_queue; //!< 写数据的缓存
    uint8_t w_queue_data[TTY_QUEUE_DATA_SIZE];
    queue_t pre_queue; //!< 然后通过handler处理机制存放到per_queue中，pre_queue中的数据直接可以给用户，或者进行回显
    uint8_t pre_uque_data[TTY_QUEUE_DATA_SIZE];
    // 有多少列
    int col;
    // 有多少行
    int row;
    // 字符错误
    uint8_t is_error;

    // 是否成行了
    uint8_t is_nl;
    pid_t fg_pid;

    // FIXME:
    int fd_flags; //!< 暂时这样
} tty_struct_t;

#define C_CC_INIT "\003\034\177\025\004\0\1\0\021\023\032\0\022\017\027\026\0"
// #define C_CC_INIT "\003\034\b\025\004\0\1\0\021\023\032\0\022\017\027\026\0"

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
#define LNEXT_C(tty) ((tty)->termios.c_cc[VLNEXT])
#define EOL2_C(tty) ((tty)->termios.c_cc[VEOL2])

#define _I_FLAG(tty, f) ((tty)->termios.c_iflag & (f))
#define _O_FLAG(tty, f) ((tty)->termios.c_oflag & (f))
#define _C_FLAG(tty, f) ((tty)->termios.c_cflag & (f))
#define _L_FLAG(tty, f) ((tty)->termios.c_lflag & (f))

#define I_IGNBRK(tty) _I_FLAG((tty), IGNBRK)
#define I_BRKINT(tty) _I_FLAG((tty), BRKINT)
#define I_IGNPAR(tty) _I_FLAG((tty), IGNPAR)
#define I_PARMRK(tty) _I_FLAG((tty), PARMRK)
#define I_INPCK(tty) _I_FLAG((tty), INPCK)
#define I_ISTRIP(tty) _I_FLAG((tty), ISTRIP)
#define I_INLCR(tty) _I_FLAG((tty), INLCR)
#define I_IGNCR(tty) _I_FLAG((tty), IGNCR)
#define I_ICRNL(tty) _I_FLAG((tty), ICRNL)
#define I_IUCLC(tty) _I_FLAG((tty), IUCLC)
#define I_IXON(tty) _I_FLAG((tty), IXON)
#define I_IXANY(tty) _I_FLAG((tty), IXANY)
#define I_IXOFF(tty) _I_FLAG((tty), IXOFF)
#define I_IMAXBEL(tty) _I_FLAG((tty), IMAXBEL)

#define O_OPOST(tty) _O_FLAG((tty), OPOST)
#define O_OLCUC(tty) _O_FLAG((tty), OLCUC)
#define O_ONLCR(tty) _O_FLAG((tty), ONLCR)
#define O_OCRNL(tty) _O_FLAG((tty), OCRNL)
#define O_ONOCR(tty) _O_FLAG((tty), ONOCR)
#define O_ONLRET(tty) _O_FLAG((tty), ONLRET)
#define O_OFILL(tty) _O_FLAG((tty), OFILL)
#define O_OFDEL(tty) _O_FLAG((tty), OFDEL)
#define O_NLDLY(tty) _O_FLAG((tty), NLDLY)
#define O_CRDLY(tty) _O_FLAG((tty), CRDLY)
#define O_TABDLY(tty) _O_FLAG((tty), TABDLY)
#define O_BSDLY(tty) _O_FLAG((tty), BSDLY)
#define O_VTDLY(tty) _O_FLAG((tty), VTDLY)
#define O_FFDLY(tty) _O_FLAG((tty), FFDLY)
#define O_XTABS(tty) _O_FLAG((tty), XTABS)

#define __C_FLAG(flag, f) ((flag) & (f))

#define C_BAUD(tty) _C_FLAG((tty), CBAUD)
#define C_CSIZE(tty) _C_FLAG((tty), CSIZE)
#define C_CSTOPB(tty) _C_FLAG((tty), CSTOPB)
#define C_CREAD(tty) _C_FLAG((tty), CREAD)
#define C_PARENB(tty) _C_FLAG((tty), PARENB)
#define C_PARODD(tty) _C_FLAG((tty), PARODD)
#define C_HUPCL(tty) _C_FLAG((tty), HUPCL)
#define C_CLOCAL(tty) _C_FLAG((tty), CLOCAL)
#define C_CIBAUD(tty) _C_FLAG((tty), CIBAUD)
#define C_CRTSCTS(tty) _C_FLAG((tty), CRTSCTS)
#define C_BAUDEX(tty) _C_FLAG((tty), CBAUDEX)

#define F_C_BAUD(tty) __C_FLAG((tty), CBAUD)
#define F_C_CSIZE(tty) __C_FLAG((tty), CSIZE)
#define F_C_CSTOPB(tty) __C_FLAG((tty), CSTOPB)
#define F_C_CREAD(tty) __C_FLAG((tty), CREAD)
#define F_C_PARENB(tty) __C_FLAG((tty), PARENB)
#define F_C_PARODD(tty) __C_FLAG((tty), PARODD)
#define F_C_HUPCL(tty) __C_FLAG((tty), HUPCL)
#define F_C_CLOCAL(tty) __C_FLAG((tty), CLOCAL)
#define F_C_CIBAUD(tty) __C_FLAG((tty), CIBAUD)
#define F_C_CRTSCTS(tty) __C_FLAG((tty), CRTSCTS)
#define F_C_BAUDEX(tty) __C_FLAG((tty), CBAUDEX)

#define L_ISIG(tty) _L_FLAG((tty), ISIG)
#define L_ICANON(tty) _L_FLAG((tty), ICANON)
#define L_XCASE(tty) _L_FLAG((tty), XCASE)
#define L_ECHO(tty) _L_FLAG((tty), ECHO)
#define L_ECHOE(tty) _L_FLAG((tty), ECHOE)
#define L_ECHOK(tty) _L_FLAG((tty), ECHOK)
#define L_ECHONL(tty) _L_FLAG((tty), ECHONL)
#define L_NOFLSH(tty) _L_FLAG((tty), NOFLSH)
#define L_TOSTOP(tty) _L_FLAG((tty), TOSTOP)
#define L_ECHOCTL(tty) _L_FLAG((tty), ECHOCTL)
#define L_ECHOPRT(tty) _L_FLAG((tty), ECHOPRT)
#define L_ECHOKE(tty) _L_FLAG((tty), ECHOKE)
#define L_FLUSHO(tty) _L_FLAG((tty), FLUSHO)
#define L_PENDIN(tty) _L_FLAG((tty), PENDIN)
#define L_IEXTEN(tty) _L_FLAG((tty), IEXTEN)

void tty_svr_init(void);
void tty_set_fg_pid(pid_t pid);
