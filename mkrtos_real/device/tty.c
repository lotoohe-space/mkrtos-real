//
// Created by Administrator on 2022/2/6/006.
//

//ttyʵ��
//���ȣ�һ��ϵͳֻ��һ��console�豸�������ж��tty�豸����Щtty�豸�������ӵ�console�豸����Щtty�豸�ֶ�Ӧ�˴����豸
//����˳����������console<->tty<->serise

//ϵͳ�������������ȴ���һ��console�豸

#include <knl_service.h>
#include <mkrtos/tty.h>
#include <mkrtos/dev.h>
#include <mkrtos/fs.h>
#include <ctype.h>
#include <mkrtos/sched.h>

#include <sys/types.h>
#include <sys/arm-ioctl.h>
#include <termios.h>
#include <fcntl.h>

int32_t tty_def_line_read(struct tty_struct *tty, struct file *fp, uint8_t *buf,
		int32_t count);
int32_t tty_def_line_write(struct tty_struct *tty, struct file *fp,
		uint8_t *buf, int32_t count);

#define TTY_MAX_NUM 1
static struct tty_struct ttys[TTY_MAX_NUM] = { 0 };
static struct tty_line tty_lines[TTY_MAX_NUM] = { 0 };

struct tty_line *get_tty_line(int line_no) {
	struct tty_line *tty;

	if(line_no >= TTY_MAX_NUM) {
		return 0;
	}
	return &tty_lines[line_no];
}

struct tty_struct* get_tty(dev_t dev_no) {
	struct tty_struct *cur_tty;
	int tty_dev_no;

	tty_dev_no = MINOR(dev_no);
	if (tty_dev_no >= TTY_MAX_NUM) {
		return NULL;
	}
	cur_tty = &ttys[tty_dev_no];
	return cur_tty;
}
//��ʼ��termios
void init_termios(int line, struct termios *tp) {
	mkrtos_memset(tp, 0, sizeof(struct termios));
	mkrtos_memcpy(tp->c_cc, C_CC_INIT, NCCS);

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
	//ECHOCTL |
	ECHOKE | IEXTEN;
//    } else if (IS_A_PTY_MASTER(line))
//        tp->c_cflag = B9600 | CS8 | CREAD;
}
//tty��
static int tty_open(struct inode *ino, struct file *fp) {
	//��
	int tty_dev_no;
	struct tty_struct *cur_tty;
	//����豸�Ƿ��
	if (MAJOR(ino->i_rdev_no) != TTY_MAJOR
			&& MAJOR(ino->i_rdev_no) != TTYMAUX_MAJOR) {
		return -ENODEV;
	}
//	if (MAJOR(ino->i_rdev_no) == TTYMAUX_MAJOR) {
//		fp->f_rdev = MKDEV(TTY_MAJOR, 0);
//	}
	tty_dev_no = MINOR(ino->i_rdev_no);
	if (tty_dev_no >= TTY_MAX_NUM) {
		return -ENODEV;
	}
	cur_tty = &ttys[tty_dev_no];
	//��ʱֻ֧�ִ���
	//����open����
	//cur_tty->open = uart_open;

	if (!cur_tty->used_cn && cur_tty->d_ops == NULL) {
		//��ʼ��termio
		init_termios(tty_dev_no, &cur_tty->termios);
		uart_open(cur_tty,fp);
		//cur_tty->open(cur_tty, fp);
	}
	cur_tty->used_cn++;
	//����
	cur_tty->line_no = tty_dev_no;
	cur_tty->termios.c_line = tty_dev_no;

//    fp->f_rdev=ino->i_rdev_no;
	return 0;
}
//�����Ǹ�vfs�Ķ���������ȡ�����ǣ�vfs_read->tty_read->line_read����buf�����ȡ��
static int tty_read(struct inode *ino, struct file *fp, char *buf, int count) {
	int tty_dev_no;
	int ret;
	struct tty_struct *cur_tty;
	struct tty_line *cur_line;
	//����豸�Ƿ�򿪰�
	if (MAJOR(ino->i_rdev_no) != TTY_MAJOR) {
		return -ENODEV;
	}
	tty_dev_no = MINOR(ino->i_rdev_no);
	if (tty_dev_no >= TTY_MAX_NUM) {
		return -ENODEV;
	}
	cur_tty = &ttys[tty_dev_no];
	if (!cur_tty->used_cn) {
		kprint("tty is close.\n");
		return -ENODEV;
	}

	cur_line = &tty_lines[tty_dev_no];
	if (cur_line->read) {
		ret = cur_line->read(cur_tty, fp, buf, count);
	} else {
		return -ENODEV;
	}

	return ret;
}

//д������vfs_write->tty_write->line_write->dirver_write
static int tty_write(struct inode *ino, struct file *fp, char *buf, int count) {
	int tty_dev_no;
	int ret;
	struct tty_struct *cur_tty;
	struct tty_line *cur_line;
	//����豸�Ƿ�򿪰�
	if (MAJOR(ino->i_rdev_no) != TTY_MAJOR) {
		return -ENODEV;
	}
	tty_dev_no = MINOR(ino->i_rdev_no);
	if (tty_dev_no >= TTY_MAX_NUM) {
		return -ENODEV;
	}
	cur_tty = &ttys[tty_dev_no];
	if (!cur_tty->used_cn) {
		return -ENODEV;
	}

	cur_line = &tty_lines[tty_dev_no];
	if (cur_line->write) {
		ret = cur_line->write(cur_tty, fp, buf, count);
	} else {
		return -ENODEV;
	}

	return ret;
}
static int tty_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		unsigned long arg) {
	void *res_term;
	dev_t dev_no;
	struct tty_struct *cur_tty;
	struct winsize *pw_size;
	struct serial_struct seri;
	dev_no = inode->i_rdev_no;
	res_term = (void*) arg;
	if (!res_term) {
		return -EINVAL;
	}
	//�����豸��Ӧ��tty
	cur_tty = get_tty(dev_no);

	if (!cur_tty) {
		return -ENOTTY;
	}

	switch (cmd) {
	case TCGETS:
		if (cur_tty->d_ops->ioctl) {
			if (cur_tty->d_ops->ioctl(cur_tty, file, TTY_IO_GET, &seri) >= 0) {
				cur_tty->termios.c_cflag = seri.c_cflag;
			}
		}
		//��ȡ����
		mkrtos_memcpy(res_term, &cur_tty->termios, sizeof(struct termio));
		break;
	case TCSETS:
		//���ò���
		mkrtos_memcpy(&cur_tty->termios, res_term, sizeof(struct termio));
		seri.c_cflag = cur_tty->termios.c_cflag;
		if (cur_tty->d_ops->ioctl) {
			cur_tty->d_ops->ioctl(cur_tty, file, TTY_IO_SET, &seri);
		}
		break;
	case TIOCGWINSZ:
		//TODO:
		cur_tty->w_size.ws_col = 80;
		cur_tty->w_size.ws_row = 80;
		//��ȡ���ڴ�С
		pw_size = (struct winszie*) arg;
		*pw_size = cur_tty->w_size;
		break;
	case TIOCSWINSZ:
		//�ı䴰���С
		pw_size = (struct winszie*) arg;
		if (pw_size->ws_col != cur_tty->w_size.ws_col
				|| pw_size->ws_row != cur_tty->w_size.ws_row) {
			inner_set_sig(SIGWINCH);
		}
		cur_tty->w_size = *pw_size;
		break;
	default:
		//��ʱ��֧�ֵ�CMD
		break;
	}
	return 0;
}
static void tty_release(struct inode *ino, struct file *fp) {
	int tty_dev_no;
	int ret;
	struct tty_struct *cur_tty;
	struct tty_line *cur_line;
	if (MAJOR(ino->i_rdev_no) != TTY_MAJOR) {
		return;
	}
	tty_dev_no = MINOR(ino->i_rdev_no);
	if (tty_dev_no >= TTY_MAX_NUM) {
		return;
	}
	cur_tty = &ttys[tty_dev_no];
	if (cur_tty->used_cn == 0) {
		return;
	}
	if (cur_tty->used_cn == 1) {
		cur_tty->d_ops->close(cur_tty, fp);
		cur_tty->d_ops->open = NULL;
		mkrtos_memset(&cur_tty->termios, 0, sizeof(cur_tty->termios));
	}
	cur_tty->used_cn--;
}

int tty_reg_line(int disc, struct tty_line *new_line) {
	if (disc < N_TTY || disc >= TTY_MAX_NUM)
		return -EINVAL;

	if (new_line) {
		tty_lines[disc] = *new_line;
//        tty_lines[disc].flags |= LDISC_FLAG_DEFINED;
	} else
		mkrtos_memset(&tty_lines[disc], 0, sizeof(struct tty_line));

	return 0;
}
/**
 * ttyĬ�ϵ�read������
 * @param tty
 * @param bug
 * @param count
 * @return
 */
int32_t tty_def_line_read(struct tty_struct *tty, struct file *fp, uint8_t *buf,
		int32_t count) {
	uint8_t r;
	int i;

	again:
	if (!q_length(&tty->pre_queue) || L_ICANON(tty)) {    //��ȡ���ݵĳ���Ϊ�㣬�����ߵȴ�
		if (!q_length(&tty->r_queue)) {
			if (fp->f_flags & O_NONBLOCK) {
				return -EAGAIN;
			}
		} else {
			goto next;
		}

		struct wait_queue wait = { get_current_task(), NULL };
		add_wait_queue(&tty->r_wait, &wait);
		task_suspend();
		task_sche();
		remove_wait_queue(&tty->r_wait, &wait);
		task_run();
		if (!q_length(&tty->r_queue)) {
			goto again;
		}
	}
	next:
	if (tty_lines[tty->line_no].handler) {
		tty_lines[tty->line_no].handler(tty);
	}
	if (!tty->is_nl && L_ICANON(tty)) {
		ttys[tty->line_no].d_ops->write(tty);
		//�淶ģʽ��û�г�һ��
		goto again;
	}
	tty->is_nl = 0;
	for (i = 0; i < count && q_get(&tty->pre_queue, &r) >= 0; i++) {
		buf[i] = r;
	}
	//����л��Ե��ַ���ֱ��������д
	ttys[tty->line_no].d_ops->write(tty);
	return i;
}
static void tty_add_w_queue(struct tty_struct *tty, char r) {
	if (O_OPOST(tty)) {
		if (O_OLCUC(tty) && mkrtos_isupper(r)) {
			//Сдת��д
			r = mkrtos_toupper(r);
		}
		if (O_ONLCR(tty) && r == '\n') {
			q_add(&tty->w_queue, '\r');
		}
		if (O_OCRNL(tty) && r == '\r') {
			r = '\n';
			if (O_ONLRET(tty)) {
				tty->col = 0;
			}
		}

		if (O_XTABS(tty) && r == '\t') {
			//�Ʊ���ᱻת���ɿո��
			r = ' ';
		}
	}

	q_add(&tty->w_queue, r);
}
/**
 * ttyĬ�ϵ�read������
 * @param tty
 * @param bug
 * @param count
 * @return
 */
int32_t tty_def_line_write(struct tty_struct *tty, struct file *fp,
		uint8_t *buf, int32_t count) {
	int32_t ret;
	int32_t i;
	uint8_t r;
	uint32_t t;
	spin_lock(&tty->w_lock);
	for (i = 0; i < count; i++) {
		r = buf[i];
		tty_add_w_queue(tty, r);
	}
	spin_unlock(&tty->w_lock);
	//����д����
	ret = tty->d_ops->write(tty);

	return i;
}
/**
 * ɾ��һ���ַ�
 * @param tty
 * @return
 */
static int erase_c(struct tty_struct *tty) {
	char r_tmp;
	//������һ���ַ��ǻ��з��ţ������ڽ��в����ˡ�
	if (q_check_f(&tty->pre_queue, '\n') == 0) {
		return 0;
	}
	if (q_get_f(&tty->pre_queue, &r_tmp) >= 0) {
		//�h���ϴ�д�������ַ�
		//����ڱ�׼ģʽ���趨��ECHOE��־�����յ�һ��ERASE���Ʒ�ʱ��ɾ��ǰһ����ʾ�ַ���
		tty_add_w_queue(tty, '\b');
		tty_add_w_queue(tty, ' ');
		tty_add_w_queue(tty, '\b');
		if (tty->col > 0) {
			tty->col--;
		}
	}
	return 0;
}
/**
 * �Զ�ȡ�����ݽ��д���
 * @param tty
 */
void tty_def_line_handler(struct tty_struct *tty) {
	uint8_t r;

	if (L_ICANON(tty)) {
		//�ܹ���������
		while (q_get(&tty->r_queue, &r) != -1) {

			if (tty->is_error) {
				if (!I_IGNPAR(tty)) {
					if (I_PARMRK(tty)) {
						q_add(&tty->pre_queue, '\377');
						q_add(&tty->pre_queue, '\0');
						q_add(&tty->pre_queue, r);
					} else {
						q_add(&tty->pre_queue, '\0');
					}
				}
			} else {
				if (I_ISTRIP(tty)) {        //ȥ�����λ
					r &= 0x7f;
				}
				if (I_IGNCR(tty)) {        //ȥ�������cr
					if (r == '\r') {
						continue;
					}
				} else if (I_INLCR(tty)) {
					if (r == '\n') {
						r = '\r';
					}
				}
				if (I_ICRNL(tty) && !I_IGNCR(tty)) {
					if (r == '\r') {
						r = '\n';
					}
				}

				if (I_IUCLC(tty) && mkrtos_isupper(r)) {        //��дתСд
					r = mkrtos_tolower(r);
				}

				//����ģʽ�Ĵ���
				if (L_ECHO(tty)) {
					if (L_ECHOCTL(tty) && mkrtos_iscntrl(r)) {
						if (r != START_C(tty)        //��ʾָ���Ŀ����ַ�
						&& r != STOP_C(tty) && r != '\t' && r != '\n'
								&& (r >= 0 && r <= 37)) {
							tty_add_w_queue(tty, '^');
							tty_add_w_queue(tty, r + 0x40);
							goto next;
						}
					}
					if (L_ICANON(tty)) {        //��׼ģʽ
						if (L_ECHOE(tty) && r == ERASE_C(tty)) {        //ɾ��һ���ַ�
							erase_c(tty);
							continue;
						}
						if (!L_ECHOKE(tty)) {
							if (L_ECHOK(tty)) {
								//���ICANONͬʱ���ã�KILL��ɾ����ǰ��
								if (r == KILL_C(tty)) {
									//ɾ����ǰ�У���ûд
									tty_add_w_queue(tty, '\n');
									continue;
								}
							}
						} else if (r == KILL_C(tty)) {
							//�h�����е�ÿһ���ַ�
							for (int i = 0; i < tty->col; i++) {
								erase_c(tty);
							}
							continue;
						}
					}
					tty_add_w_queue(tty, r);
				} else {
					if (L_ECHONL(tty) && L_ICANON(tty)) {
						//����ڱ�׼ģʽ�������˸ñ�־����ʹû������ECHO��־�����з����ǻᱻ��ʾ������
						if (r == '\n') {
							tty_add_w_queue(tty, r);
						}
					}
				}
				next:
//                if(!I_IGNBRK(tty) && I_BRKINT(tty) && r==){
//                	inner_set_sig(SIGINT-1);
//                }

				if (L_ISIG(tty)) {
					//������Ӧ���ź�
					if (r == INTR_C(tty)) {
						//���͸�ǰ̨����������н���
						inner_set_sig(SIGINT);
						if (!L_NOFLSH(tty)) {
							q_clear(&tty->w_queue);
							q_clear(&tty->r_queue);
						}
						tty->is_nl = 1;
						continue;
					} else if (r == QUIT_C(tty)) {
						inner_set_sig(SIGQUIT);
						if (!L_NOFLSH(tty)) {
							q_clear(&tty->w_queue);
							q_clear(&tty->r_queue);
						}
						tty->is_nl = 1;
						continue;
					} else if (r == SUSP_C(tty)) {
						inner_set_sig(SIGTSTP);
						if (!L_NOFLSH(tty)) {
							q_clear(&tty->r_queue);
						}
						tty->is_nl = 1;
						continue;
					}
				}
				if (L_TOSTOP(tty)) {
					//����ǰ̨������Ľ�����ͼ�����Ŀ����ն�д������ʱ��
					// �ź�SIGTTOU�ᱻ�����͵�����������ڵĽ����顣
					// Ĭ������£�����źŻ�ʹ����ֹͣ
					// �������յ�SUSP���Ʒ�һ����
				}

				if (!mkrtos_iscntrl(r)) {
					tty->col++;
				} else {
					if (r == '\n') {
						tty->col = 0;
					}
				}
				if (L_ICANON(tty)) {
					//�淶������ʽ
					if (
//                            r == EOF_C(tty)
//                        || r == EOL_C(tty)
//                        || r == EOL2_C(tty)
//                        || r == ERASE_C(tty)
//                        || r == KILL_C(tty)
//                        || r == REPRINT_C(tty)
//                        || r == START_C(tty)
//                        || r == WERASE_C(tty)
					r == '\n'||r== EOL_C(tty)
					||r== EOL2_C(tty)
					||r== EOF_C(tty)
					) {
						if (r != EOF_C(tty)) {
							if (q_add(&tty->pre_queue, r) < 0) {
								//���ʧ����
								//����Ӧ�ü��ϵȴ�����
							}
						}
						tty->is_nl = 1;
//                        q_clear(&tty->r_queue);
						return;
					} else {
						tty->is_nl = 0;
					}
				}
				if (q_add(&tty->pre_queue, r) < 0) {
					//���ʧ����
					//����Ӧ�ü��ϵȴ�����
				}
			}
		}
	} else {
		while (q_get(&tty->r_queue, &r) != -1) {
			q_add(&tty->pre_queue, r);
		}
	}
	end: return;
}

static struct file_operations tty_ops = { .open = tty_open, .read = tty_read,
		.write = tty_write, .ioctl = tty_ioctl, .release = tty_release };

static struct tty_line def_tty_line = { .write = tty_def_line_write, .read =
		tty_def_line_read, .handler = tty_def_line_handler };
int tty_init(void) {
	tty_reg_line(0, &def_tty_line);
	if (reg_ch_dev(TTY_MAJOR, "tty", &tty_ops) < 0) {
		return -1;
	}
	if (reg_ch_dev(TTYMAUX_MAJOR, "tty", &tty_ops) < 0) {
		return -1;
	}

	return 0;
}
int tty_close(void) {
	tty_reg_line(0, 0);
	unreg_ch_dev(TTY_MAJOR, "tty");
	unreg_ch_dev(TTYMAUX_MAJOR, "tty");
	return 0;
}

DEV_BK_EXPORT(tty_init, tty_close, tty);
