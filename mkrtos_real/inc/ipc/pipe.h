/*
 * pipe.h
 *
 *  Created on: 2022��10��20��
 *      Author: zhangzheng
 */

#ifndef INC_IPC_PIPE_H_
#define INC_IPC_PIPE_H_

#include <sched.h>

void do_fork_pipe(struct inode *inode,struct task* newtask,int fd);

#endif /* INC_IPC_PIPE_H_ */
