/*
 * early_console.h
 *
 *  Created on: 2022年9月27日
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_EARLY_CONSOLE_H_
#define INC_MKRTOS_EARLY_CONSOLE_H_

int console_reg(int tty_no);
int console_read(char *data, int len);
int console_write(char *data, int len);

#endif /* INC_MKRTOS_EARLY_CONSOLE_H_ */
