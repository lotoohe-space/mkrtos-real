
#include <mkrtos/tty.h>

#include <mkrtos/tty.h>
#include <mkrtos/early_console.h>
#include <config.h>

struct tty_struct *cons_tty = NULL;

int console_reg(int tty_no)
{
	if (!cons_tty) {
		int ret;
		struct tty_struct *tmp_tty;

		tty_init();
		tmp_tty = get_tty(tty_no);
		if (!tmp_tty) {
			return -1;
		}
		init_termios(tty_no, &tmp_tty->termios);
		ret = uart_open(tmp_tty, 0);
		if (ret < 0) {
			return -ENODEV;
		}

		tmp_tty->used_cn++;
		tmp_tty->line_no = tty_no;
		tmp_tty->termios.c_line = tty_no;
		cons_tty = tmp_tty;
		return 0;
	}
	return -ENODEV;
}
int console_read(char *data, int len)
{
	int ret;
	struct tty_line *cur_line;

	if (!cons_tty){
		return 0;
	}

	cur_line = get_tty_line(cons_tty->line_no);
	if(!cur_line) {
		return -ENODEV;
	}
	if (cur_line->read) {
		ret = cur_line->read(cons_tty, 0, data, len);
	} else {
		return -ENODEV;
	}

	return ret;
}
int console_write(char *data, int len)
{
	int ret;
	struct tty_line *cur_line;

	if (!cons_tty){
		return 0;
	}

	cur_line = get_tty_line(cons_tty->line_no);
	if(!cur_line) {
		return -ENODEV;
	}
	if (cur_line->write) {
		ret = cur_line->write(cons_tty, 0, data, len);
	} else {
		return -ENODEV;
	}

	return ret;
}

static int reg_early_console(void)
{
	console_reg(0);
	return 0;
}
INIT_REG(reg_early_console, INIT_FUNC_REG);
