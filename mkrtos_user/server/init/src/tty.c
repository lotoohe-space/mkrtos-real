#include "tty.h"
#include "u_types.h"
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <u_queue.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include "cons.h"
#include "u_hd_man.h"
#include "u_prot.h"
#include "u_task.h"
#include "u_sema.h"
#include "u_mutex.h"
#include "u_factory.h"
#include "u_thread_util.h"
#include "u_log.h"
#include "u_sleep.h"
#include "rpc_prot.h"
#include "fs_svr.h"
#include "ns.h"
#include "u_task.h"
#include "pm.h"
#include "pm_svr.h"
#if IS_ENABLED(CONFIG_MMU)
#define CONS_STACK_SIZE 1024
#else
#define CONS_STACK_SIZE 1024
#endif
static ATTR_ALIGN(8) uint8_t cons_stack[CONS_STACK_SIZE];
static uint8_t cons_ipc_msg[MSG_BUG_LEN];
static tty_struct_t sys_tty;

static obj_handler_t cons_th;
static obj_handler_t sem_th;
static u_mutex_t lock_cons;
static meta_t tty_meta;
static obj_handler_t tty_ipc_hd;
static fs_t tty_fs;

static int tty_def_line_handler(tty_struct_t *tty, uint8_t r);
static int tty_write_hw(tty_struct_t *tty);

void tty_set_fg_pid(pid_t pid)
{
    sys_tty.fg_pid = pid;
}

void tty_struct_init(tty_struct_t *tty)
{
    // q_init(&tty->r_queue, tty->r_queue_data, TTY_QUEUE_DATA_SIZE);
    q_init(&tty->w_queue, tty->w_queue_data, TTY_QUEUE_DATA_SIZE);
    q_init(&tty->pre_queue, tty->pre_uque_data, TTY_QUEUE_DATA_SIZE);
}

static inline void cons_read_lock(void)
{
    u_mutex_lock(&lock_cons, 0, 0);
}
static inline void cons_read_unlock(void)
{
    u_mutex_unlock(&lock_cons);
}

static void console_read_func(void)
{
    uint8_t data[12];

    while (1)
    {
        int r_len = ulog_read_bytes(LOG_PROT, data, sizeof(data));

        if (r_len > 0)
        {
            cons_read_lock();
            for (int i = 0; i < r_len; i++)
            {
                // q_enqueue(&sys_tty.r_queue, data[i]);
                if (tty_def_line_handler(&sys_tty, data[i]) == 1)
                {
                    break;
                }
            }
            if (!sys_tty.is_nl && L_ICANON(&sys_tty))
            {
                tty_write_hw(&sys_tty);
            }
            else
            {
                u_sema_up(sem_th);
            }
            cons_read_unlock();
        }
    }
    // handler_free_umap(cons_obj.hd_cons_read);
    while (1)
    {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
}

// 初始化termios
static void init_termios(struct termios *tp)
{
    memset(tp, 0, sizeof(struct termios));
    memcpy(tp->c_cc, C_CC_INIT, NCCS);

    //    if (IS_A_CONSOLE(line) || IS_A_PTY_SLAVE(line)) {
    //        tp->c_iflag = ICRNL | IXON;
    //        tp->c_oflag = OPOST | ONLCR;
    //        tp->c_cflag = B38400 | CS8 | CREAD;
    //        tp->c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK |
    //                      ECHOCTL | ECHOKE | IEXTEN;
    //    } else if (IS_A_SERIAL(line)) {
    tp->c_iflag = ICRNL | IXOFF;
    tp->c_oflag = OPOST | ONLCR | XTABS;
    tp->c_cflag = B115200 | CS8 | CREAD | HUPCL | CLOCAL;
    tp->c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK |
                  // ECHOCTL |
                  ECHOKE | IEXTEN;
    //    } else if (IS_A_PTY_MASTER(line))
    //        tp->c_cflag = B9600 | CS8 | CREAD;
}
static int cons_init(void)
{
    msg_tag_t tag;

    u_mutex_init(&lock_cons, handler_alloc());

    sem_th = handler_alloc();
    if (sem_th == HANDLER_INVALID)
    {
        return -1;
    }
    tag = facotry_create_sema(FACTORY_PROT,
                              vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sem_th), 0, 1);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    u_thread_create(&cons_th, (char *)cons_stack + sizeof(cons_stack) - 8, cons_ipc_msg, console_read_func);
    u_thread_run(cons_th, 4);
    ulog_write_str(LOG_PROT, "cons init...\n");
    return 0;
}
static int tty_open(const char *path, int flags, int mode)
{

    ulog_write_str(LOG_PROT, "tty open..\n");
    sys_tty.fd_flags = flags;
    return 0;
}

static void tty_add_w_queue(tty_struct_t *tty, char r)
{
    if (O_OPOST(tty))
    {
        if (O_OLCUC(tty) && isupper(r))
        {
            // 小写转大写
            r = toupper(r);
        }
        if (O_ONLCR(tty) && r == '\n')
        {
            q_enqueue(&tty->w_queue, '\r');
        }
        if (O_OCRNL(tty) && r == '\r')
        {
            r = '\n';
            if (O_ONLRET(tty))
            {
                tty->col = 0;
            }
        }

        if (O_XTABS(tty) && r == '\t')
        {
            // 制表符会被转换成空格符
            r = ' ';
        }
    }

    q_enqueue(&tty->w_queue, r);
}
/**
 * 删除一个字符
 * @param tty
 * @return
 */
static int erase_c(tty_struct_t *tty)
{
    char r_tmp;

    // 如果最后一个字符是换行符号，则不能在进行擦除了。
    if (q_get_tail(&tty->pre_queue, &r_tmp) < 0)
    {
        return -1;
    }
    if (r_tmp == '\n')
    {
        return 0;
    }
    if (q_dequeue_tail(&tty->pre_queue, &r_tmp) >= 0)
    {
        // 刪除上次写的两个字符
        // 如果在标准模式下设定了ECHOE标志，则当收到一个ERASE控制符时将删除前一个显示字符。
        tty_add_w_queue(tty, '\b');
        tty_add_w_queue(tty, ' ');
        tty_add_w_queue(tty, '\b');
        if (tty->col > 0)
        {
            tty->col--;
        }
    }
    return 0;
}
/**
 * 对读取的数据进行处理
 * @param tty
 */
static int tty_def_line_handler(tty_struct_t *tty, uint8_t r)
{
    int ret = 0;

    if (L_ICANON(tty))
    {
        // 能够读到数据
        // while (q_dequeue(&tty->r_queue, &r) >= 0)
        {

            if (tty->is_error)
            {
                if (!I_IGNPAR(tty))
                {
                    if (I_PARMRK(tty))
                    {
                        q_enqueue(&tty->pre_queue, '\377');
                        q_enqueue(&tty->pre_queue, '\0');
                        q_enqueue(&tty->pre_queue, r);
                    }
                    else
                    {
                        q_enqueue(&tty->pre_queue, '\0');
                    }
                }
            }
            else
            {
                if (I_ISTRIP(tty))
                { // 去掉最高位
                    r &= 0x7f;
                }
                if (I_IGNCR(tty))
                { // 去掉输入的cr
                    if (r == '\r')
                    {
                        return ret;
                    }
                }
                else if (I_INLCR(tty))
                {
                    if (r == '\n')
                    {
                        r = '\r';
                    }
                }
                if (I_ICRNL(tty) && !I_IGNCR(tty))
                {
                    if (r == '\r')
                    {
                        r = '\n';
                    }
                }

                if (I_IUCLC(tty) && isupper(r))
                { // 大写转小写
                    r = tolower(r);
                }

                // 本地模式的处理
                if (L_ECHO(tty))
                {
                    if (L_ECHOCTL(tty) && iscntrl(r))
                    {
                        if (r != START_C(tty) // 显示指定的控制字符
                            && r != STOP_C(tty) && r != '\t' && r != '\n' && (r >= 0 && r <= 37))
                        {
                            tty_add_w_queue(tty, '^');
                            tty_add_w_queue(tty, r + 0x40);
                            goto next;
                        }
                    }
                    if (L_ICANON(tty))
                    { // 标准模式
                        if (L_ECHOE(tty) && r == ERASE_C(tty))
                        { // 删除一个字符
                            erase_c(tty);
                            return ret;
                        }
                        if (!L_ECHOKE(tty))
                        {
                            if (L_ECHOK(tty))
                            {
                                // 如果ICANON同时设置，KILL将删除当前行
                                if (r == KILL_C(tty))
                                {
                                    // 删除当前行，还没写
                                    tty_add_w_queue(tty, '\n');
                                    return ret;
                                }
                            }
                        }
                        else if (r == KILL_C(tty))
                        {
                            // 刪除这行的每一个字符
                            for (int i = 0; i < tty->col; i++)
                            {
                                erase_c(tty);
                            }
                            return ret;
                        }
                    }
                    tty_add_w_queue(tty, r);
                }
                else
                {
                    if (L_ECHONL(tty) && L_ICANON(tty))
                    {
                        // 如果在标准模式下设置了该标志，即使没有设置ECHO标志，换行符还是会被显示出来。
                        if (r == '\n')
                        {
                            tty_add_w_queue(tty, r);
                        }
                    }
                }
            next:
                //                if(!I_IGNBRK(tty) && I_BRKINT(tty) && r==){
                //                	inner_set_sig(SIGINT-1);
                //                }

                if (L_ISIG(tty))
                {
                    // 发送响应的信号
                    if (r == INTR_C(tty))
                    {
                        // 发送给前台进程组的所有进程
                        // inner_set_sig(SIGINT);TODO:
                        pm_rpc_kill_task(-1, tty->fg_pid, 0, 0);
                        // ulog_write_str(LOG_PROT, "ctrl c");
                        if (!L_NOFLSH(tty))
                        {
                            q_queue_clear(&tty->w_queue);
                            // q_queue_clear(&tty->r_queue);
                            ret = 1;
                        }
                        tty->is_nl = 1;
                        return ret;
                    }
                    else if (r == QUIT_C(tty))
                    {
                        // inner_set_sig(SIGQUIT);TODO:
                        ulog_write_str(LOG_PROT, "Ctrl+C");
                        if (!L_NOFLSH(tty))
                        {
                            q_queue_clear(&tty->w_queue);
                            // q_queue_clear(&tty->r_queue);
                            ret = 1;
                        }
                        tty->is_nl = 1;
                        return ret;
                    }
                    else if (r == SUSP_C(tty))
                    {
                        // inner_set_sig(SIGTSTP);TODO:
                        if (!L_NOFLSH(tty))
                        {
                            // q_queue_clear(&tty->r_queue);
                            ret = 0;
                        }
                        tty->is_nl = 1;
                        return ret;
                    }
                }
                if (L_TOSTOP(tty))
                {
                    // 个非前台进程组的进程试图向它的控制终端写入数据时，
                    //  信号SIGTTOU会被被发送到这个进程所在的进程组。
                    //  默认情况下，这个信号会使进程停止
                    //  ，就像收到SUSP控制符一样。
                }

                if (!iscntrl(r))
                {
                    tty->col++;
                }
                else
                {
                    if (r == '\n')
                    {
                        tty->col = 0;
                    }
                }
                if (L_ICANON(tty))
                {
                    // 规范工作方式
                    if (
                        //                            r == EOF_C(tty)
                        //                        || r == EOL_C(tty)
                        //                        || r == EOL2_C(tty)
                        //                        || r == ERASE_C(tty)
                        //                        || r == KILL_C(tty)
                        //                        || r == REPRINT_C(tty)
                        //                        || r == START_C(tty)
                        //                        || r == WERASE_C(tty)
                        r == '\n' || r == EOL_C(tty) || r == EOL2_C(tty) || r == EOF_C(tty))
                    {
                        if (r != EOF_C(tty))
                        {
                            if (q_enqueue(&tty->pre_queue, r) < 0)
                            {
                                // 添加失败了
                                // 这里应该加上等待机制
                            }
                        }
                        tty->is_nl = 1;
                        //                        q_clear(&tty->r_queue);
                        return ret;
                    }
                    else
                    {
                        tty->is_nl = 0;
                    }
                }
                if (q_enqueue(&tty->pre_queue, r) < 0)
                {
                    // 添加失败了
                    // 这里应该加上等待机制
                }
            }
        }
    }
    else
    {
        // while (q_dequeue(&tty->r_queue, &r) != -1)
        {
            q_enqueue(&tty->pre_queue, r);
        }
    }
end:
    return ret;
}
static int tty_write_hw(tty_struct_t *tty)
{
    uint8_t r;
    int w_len = 0;
    int res;

    if (!q_queue_len(&tty->w_queue))
    {
        return w_len;
    }
    while ((res = q_dequeue(&tty->w_queue, &r)) >= 0)
    {
        ulog_write_bytes(LOG_PROT, &r, 1);
        w_len++;
    }
    return w_len;
}
static int tty_read(int fd, void *buf, size_t len)
{
    uint8_t r;
    uint8_t *tmp_buf = buf;
    int i = 0;

    if (thread_get_src_pid() != sys_tty.fg_pid)
    {
        return -ENOTTY;
    }
    if (q_queue_len(&sys_tty.pre_queue) == 0)
    {
        if (sys_tty.fd_flags & O_NONBLOCK)
        {
            return -EAGAIN;
        }
        u_sema_down(sem_th, 0, NULL);
    }
again:
    cons_read_lock();
    if (q_queue_len(&sys_tty.pre_queue) == 0)
    {
        cons_read_unlock();
        return -EAGAIN;
    }

    sys_tty.is_nl = 0;
    for (i = 0; i < len && q_dequeue(&sys_tty.pre_queue, &r) >= 0; i++)
    {
        tmp_buf[i] = r;
    }
    if (q_queue_len(&sys_tty.pre_queue) != 0)
    {
        u_sema_up(sem_th);
    }
    cons_read_unlock();
    // 如果有回显的字符，直接在这里写
    tty_write_hw(&sys_tty);
    return i;
}
static int tty_write(int fd, void *buf, size_t len)
{
    int i;
    uint8_t r;
#if 0
    int ret;
    uint8_t *tmp_buf = buf;
    for (i = 0; i < len; i++)
    {
        r = tmp_buf[i];
        tty_add_w_queue(&sys_tty, r);
    }
    // 调用写函数
    ret = tty_write_hw(&sys_tty);
#else
    ulog_write_bytes(LOG_PROT, buf, len);
#endif
    return len;
}

static int tty_ioctl(int fd, int req, void *args)
{
    pid_t src_pid = thread_get_src_pid();
    pid_t cur_pid = TASK_THIS;
    int ret = 0;
    msg_tag_t tag;

    if (!args)
    {
        return -EINVAL;
    }
#if 1
    tag = task_get_pid(TASK_THIS, (umword_t *)(&cur_pid));
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    if (cur_pid == 0)
    {
        cur_pid = TASK_THIS;
    }
#endif
    switch (req)
    {
    case TCGETS:
    {
        ret = pm_rpc_copy_data(cur_pid, src_pid,
                               (addr_t)&sys_tty.termios, (addr_t)args,
                               sizeof(struct termios));
        if (ret < 0)
        {
            return ret;
        }
    }
    break;
    case TCSETS:
    {
        ret = pm_rpc_copy_data(src_pid, cur_pid,
                               (addr_t)args, (addr_t)&sys_tty.termios,
                               sizeof(struct termios));
        {
            return ret;
        }
    }
    break;
    case TIOCGWINSZ:
    {
        sys_tty.w_size.ws_col = 80;
        sys_tty.w_size.ws_row = 80;

        ret = pm_rpc_copy_data(cur_pid, src_pid,
                               (addr_t)&sys_tty.w_size, (addr_t)args,
                               sizeof(struct winsize));
        if (ret < 0)
        {
            return ret;
        }
    }
    break;
    case TIOCSWINSZ:
    {
        ret = pm_rpc_copy_data(src_pid, cur_pid,
                               (addr_t)args, (addr_t)&sys_tty.w_size,
                               sizeof(struct winsize));
        {
            return ret;
        }
        // inner_set_sig(SIGWINCH);TODO:
    }
    break;
    case TIOCSPGRP:
    {
        pid_t pgrp;

        ret = pm_rpc_copy_data(src_pid, cur_pid,
                               (addr_t)args, (addr_t)&pgrp,
                               sizeof(pid_t));
        if (ret < 0)
        {
            return ret;
        }
        sys_tty.fg_pid = pgrp; //!< FIXME:暂时没有进程组的概念
    }
    break;
    case TIOCGPGRP:
    {
        ret = pm_rpc_copy_data(cur_pid, src_pid,
                               (addr_t)&sys_tty.fg_pid, (addr_t)args,
                               sizeof(pid_t));
        if (ret < 0)
        {
            return ret;
        }
    }
    break;
    case FIONREAD:
    {
        int r_len = q_queue_len(&sys_tty.pre_queue);

        ret = pm_rpc_copy_data(cur_pid, src_pid,
                               (addr_t)&r_len, (addr_t)args,
                               sizeof(int));
        if (ret < 0)
        {
            return ret;
        }
    }
    break;
    case TCIOFLUSH:
    {
        cons_read_lock();
        q_queue_clear(&sys_tty.pre_queue);
        q_queue_clear(&sys_tty.w_queue);
        cons_read_unlock();
    }
    break;
    case TCIFLUSH:
    {
        cons_read_lock();
        q_queue_clear(&sys_tty.pre_queue);
        cons_read_unlock();
    }
    break;
    case TCOFLUSH:
    {
        cons_read_lock();
        q_queue_clear(&sys_tty.w_queue);
        cons_read_unlock();
    }
    break;
    default:
        ret = -ENOSYS;
        break;
    }
    return ret;
}
static void tty_close(int fd)
{
    printf("to close tty.\n");
}

static const fs_operations_t ops =
    {
        .fs_svr_close = tty_close,
        .fs_svr_open = tty_open,
        .fs_svr_read = tty_read,
        .fs_svr_write = tty_write,
        .fs_svr_ioctl = tty_ioctl,
};
void tty_svr_init(void)
{
    int ret;

    tty_struct_init(&sys_tty);
    init_termios(&sys_tty.termios);
    cons_init();

    meta_obj_init(&tty_meta);
    rpc_meta_init(&tty_meta, TASK_THIS, &tty_ipc_hd);
    fs_init(&tty_fs, &ops);
    meta_reg_svr_obj_raw(&tty_meta, &tty_fs.svr, FS_PROT);

    ret = namespace_register("/dev/tty", tty_ipc_hd, 0);
    if (ret < 0)
    {
        printf("tty driver reg failed [%d].\n", ret);
    }
}
