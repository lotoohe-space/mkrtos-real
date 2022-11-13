//
// Created by Administrator on 2022/1/9.
//
#include <knl_service.h>
#include <type.h>
#include <mkrtos/sched.h>
#include "arch/arch.h"
#include <string.h>
#include <sched.h>
#include <mkrtos/exec.h>
#include <ipc/pipe.h>
// fn为函数指针，此指针指向一个函数体，即想要创建进程的静态程序（我们知道进程的4要素，这个就是指向程序的指针，就是所谓的“剧本", ）；
//     child_stack为给子进程分配系统堆栈的指针（在linux下系统堆栈空间是2页面，就是8K的内存，其中在这块内存中，低地址上放入了值，这个值就是进程控制块task_struct的值）；
//      arg就是传给子进程的参数一般为（0）；
//     flags为要复制资源的标志，描述你需要从父进程继承那些资源（是资源复制还是共享，在这里设置参数：
//下面是flags可以取的值
//  标志                    含义
//  CLONE_PARENT   创建的子进程的父进程是调用者的父进程，新进程与创建它的进程成了“兄弟”而不是“父子”
//  CLONE_FS           子进程与父进程共享相同的文件系统，包括root、当前目录、umask
//  CLONE_FILES      子进程与父进程共享相同的文件描述符（file descriptor）表
//  CLONE_NEWNS   在新的namespace启动子进程，namespace描述了进程的文件hierarchy
//  CLONE_SIGHAND   子进程与父进程共享相同的信号处理（signal handler）表
//  CLONE_PTRACE   若父进程被trace，子进程也被trace 不支持
//  CLONE_VFORK     父进程被挂起，直至子进程释放虚拟内存资源
//  CLONE_VM           子进程与父进程运行于相同的内存空间
//  CLONE_PID          子进程在创建时PID与父进程一致
//  CLONE_THREAD    Linux 2.4中增加以支持POSIX线程标准，子进程与父进程共享相同的线程群

//child_stack是用户栈，内核栈还是需要申请
int32_t sys_clone(int (*fn)(void*), void *child_stack, int flags, void *arg) {

	if (!fn //|| !child_stack
	) {
		return -EINVAL;
	}

	uint32_t t = dis_cpu_intr();
	struct task *ptb = get_current_task();
	struct task *new_ptb = malloc(sizeof(struct task));
	if (new_ptb == NULL) {
		restore_cpu_intr(t);
		return -1;
	}

	mkrtos_memcpy(new_ptb, ptb, sizeof(struct task));
	new_ptb->status = TASK_SUSPEND;
	new_ptb->run_count = 0;
	new_ptb->next = NULL;
	new_ptb->next_all = NULL;

	new_ptb->mem_low_stack = NULL;

	//申请内核栈空间
	new_ptb->knl_low_stack = (void*) malloc(
			sizeof(uint32_t) * (new_ptb->kernel_stack_size));
	if (new_ptb->knl_low_stack == NULL) {
		free(new_ptb);
		restore_cpu_intr(t);
		return -1;
	}

	if (!(flags & CLONE_PID)) {
		new_ptb->pid = (pid_t) atomic_read(&get_sys_tasks_info()->pid_temp);
	}
#if 0
    //clone内存空间
    if (ptb->exec) {
        void *exec_tmp = new_ptb->exec;
        //重新复制应用信息
        new_ptb->exec = malloc(sizeof(ELFExec_t));
        if (!new_ptb->exec) {
            free(new_ptb);
            free( new_ptb->knl_low_stack);
            restore_cpu_intr(t);
            return -1;
        }
        mkrtos_memcpy(new_ptb->exec, exec_tmp, sizeof(ELFExec_t));

        if(!(flags&CLONE_VM)) {
            //不使用相同的内存空间，RAM都是独立的
            new_ptb->exec->data.data = malloc(ptb->exec->data.sh_size);
            if (!new_ptb->exec->data.data) {
                free(new_ptb);
                free(ptb->exec);
                free( new_ptb->knl_low_stack);
                restore_cpu_intr(t);
                return -1;
            }
            mkrtos_memcpy(new_ptb->exec->data.data, ptb->exec->data.data, ptb->exec->data.sh_size);
            new_ptb->exec->bss.data = malloc(ptb->exec->bss.sh_size);
            if (!new_ptb->exec->bss.data) {
                free(new_ptb);
                free(ptb->exec);
                free(new_ptb->exec->data.data);
                free( new_ptb->knl_low_stack);
                restore_cpu_intr(t);
                return -1;
            }
            mkrtos_memcpy(new_ptb->exec->bss.data, ptb->exec->bss.data, ptb->exec->bss.sh_size);
//            new_ptb->exec->clone_vm=TRUE;
        }
        //标志该exec块使用次数+1
        (*(ptb->exec->used_count))++;
    }
#endif
	int32_t err;
	/*通过优先级添加任务*/
	err = add_task(new_ptb, 1);
	if (err != 0) {
		//	restore_cpu_intr(t);
		/*释放申请的内存*/
		free(new_ptb);
#if 0
		free(ptb->exec);
		free(new_ptb->exec->data.data);
		free(new_ptb->exec->bss.data);
#endif
		free(new_ptb->knl_low_stack);
		restore_cpu_intr(t);
		return -1;
	}

	atomic_inc(&get_sys_tasks_info()->pid_temp);

	//设置内核栈位置
	new_ptb->sk_info.knl_stack =
			(ptr_t) (&(((uint32_t*) new_ptb->knl_low_stack)[ptb->kernel_stack_size
					- 1]));
	new_ptb->sk_info.knl_stack = (ptr_t) os_task_set_reg(
			new_ptb->sk_info.knl_stack, fn, arg, 0, 0,
			new_ptb->thread_exit_func, 0);
	//设置用户栈
	new_ptb->sk_info.user_stack = child_stack;
	if (new_ptb->sk_info.user_stack) {
		//重新定位
		new_ptb->sk_info.user_stack = (ptr_t) os_task_set_reg(
				new_ptb->sk_info.user_stack, fn, arg, 0, 0,
				new_ptb->thread_exit_func, new_ptb->user_ram);
	} else {
		new_ptb->user_stack_size = 0;
		new_ptb->sk_info.user_stack = (ptr_t) (~(0L));
	}
	if (!new_ptb->user_stack_size) {
		new_ptb->sk_info.stack_type = 2;
	} else {
		new_ptb->sk_info.stack_type = 1;
	}
	if (flags & CLONE_PARENT) {
		//CLONE_PARENT   创建的子进程的父进程是调用者的父进程，新进程与创建它的进程成了“兄弟”而不是“父子”
		new_ptb->parent_task = ptb->parent_task;
	} else {
		//设置父进程
		new_ptb->parent_task = ptb;
	}

	if (!(flags & CLONE_FS)) {
		//不使用父进程的FS信息
		//用0号进程的吧
		new_ptb->root_inode = get_sys_tasks_info()->idle_task->root_inode;
		new_ptb->pwd_inode = get_sys_tasks_info()->idle_task->pwd_inode;
		//引用计数器+1
		atomic_inc(&new_ptb->root_inode->i_used_count);
		atomic_inc(&new_ptb->pwd_inode->i_used_count);
#if 0
        new_ptb->mask=0;
        new_ptb->pwd_path[0]='\0';
#endif
	}

	if (flags & CLONE_FILES) {
		//子进程与父进程共享相同的文件描述符（file descriptor）表
		//对于打开的文件，我们应当对其inode的引用进行+1的操作。
		for (int i = 0; i < NR_FILE; i++) {
			if (new_ptb->files[i].used) {
				struct inode *tmp;
#if MKRTOS_USED_NET
                extern int lwip_fork(int s) ;
                if(new_ptb->files[i].net_file){
                    lwip_fork(new_ptb->files[i].net_sock);
                }else
#endif
				{
					tmp = new_ptb->files[i].f_inode;
					if (tmp) {
						if (i < 3) {
							tmp->i_ops->default_file_ops->open(tmp,
									&new_ptb->files[i]);
						}
#if MKRTOS_USED_M_PIPE
                        //pipe.c
                        void do_fork_pipe(struct inode *inode,struct task* new_task,int fd);
                        //对pipe进行fork
                        do_fork_pipe(tmp, new_ptb, i);
#endif
						atomic_inc(&(tmp->i_used_count));
					}
				}
			}
		}
	} else {
		//不复制父进程的file
		//前三个不用管
		for (int i = 0; i < NR_FILE; i++) {
			if (i < 3) {
				if (new_ptb->files[i].used) {
//                    if(new_ptb->files[i].net_file){
//                        new_ptb->files[i].sock_used_cn++;
//                    }else {
					if (new_ptb->files[i].f_inode) {
						atomic_inc(&new_ptb->files[i].f_inode->i_used_count);
					}
//                    }
				}
			} else {
				if (new_ptb->files[i].used) {
					new_ptb->files[i].used = 0;
				}
			}
		}
	}
	new_ptb->del_wait = NULL;
	new_ptb->close_wait = NULL;

	if (!(flags & CLONE_SIGHAND)) {
		//不共享信号处理表
		new_ptb->sig_bmp[0] = 0;
		new_ptb->sig_bmp[1] = 0;
		for (int i = 0; i < _NSIG; i++) {
			new_ptb->signals[i]._u._sa_handler = SIG_DFL;
		}
	}
	if (flags & CLONE_THREAD) {
		new_ptb->parent_task = ptb->parent_task;
#if 0
        new_ptb->tpid=ptb->pid;
#endif
	}

	//mem信息不进行fork
//	new_ptb->mems = NULL;
	new_ptb->status = TASK_RUNNING;
	new_ptb->clone_flag = flags;
	restore_cpu_intr(t);

	//返回pid
	return new_ptb->pid;
}

/*
 * @brief gbase ram的开始地址,tbase text的开始地址
 */
static void reloc(struct exec_file_info *exec_info, uint32_t last_data, unsigned int *ram, unsigned int text)
{
    int i;
    unsigned int s;
    unsigned int offset;
    unsigned int gs, ge, ts;
    unsigned int *rs, *re;
    unsigned int *pointer;
    uint32_t *w_pointer;

    gs = exec_info->i.got_start; //got表的开始偏移
    ge =  exec_info->i.got_end; //got表的结束偏移
    ts = exec_info->i.text_start;//text段的开始偏移

    uint32_t *t_got_start = (uint32_t *)((uint32_t)exec_info + exec_info->i.data_offset);

    for (i = 0, s = gs; s < ge; s += 4, i++)
    {
        if (t_got_start[i] >= gs)
        {
            offset = t_got_start[i] - gs;
            ram[i] = offset + (unsigned int)ram;
        }
        else
        {
            offset = t_got_start[i] - ts;
            ram[i] = offset + text;
        }
    }

    rs = text + exec_info->i.rel_start;
    re = text + exec_info->i.rel_end;

    for (i = 0, s = (unsigned int)rs; s < (unsigned int)re; s+=8, i+=2)
    {
        if (rs[i+1] == 0x00000017)
        {
            offset = rs[i] - gs;//相对data的偏移
            pointer = (unsigned int *)((unsigned int)t_got_start + offset); //读取所在位置

            w_pointer = (unsigned int *)((unsigned int)ram + offset);

            if (*pointer >= gs)
            {
                offset = *pointer- gs;
                *w_pointer = offset + (unsigned int)ram;
            }
            else
            {
                offset = *pointer- ts;
                *w_pointer = offset + (unsigned int)text;
            }
        }
    }
    return;
}

//创建一个子进程
int32_t sys_fork(void) {
	struct exec_file_info * exec_info;
	struct task *ptb = get_current_task();
	uint32_t *user_knl_sp;

	user_knl_sp = (uint32_t *)get_psp();
	struct task *new_ptb = malloc(sizeof(struct task));

	if (new_ptb == NULL) {
		return -1;
	}
	mkrtos_memcpy(new_ptb, ptb, sizeof(struct task));
	new_ptb->status = TASK_SUSPEND;
	new_ptb->run_count = 0;
	new_ptb->next = NULL;
	new_ptb->next_all = NULL;

	// 申请内核栈
	new_ptb->knl_low_stack = (void*) malloc_align(
			sizeof(uint32_t) * (new_ptb->kernel_stack_size), 8);
	if (new_ptb->knl_low_stack == NULL) {
		free(new_ptb);
		return -1;
	}

	new_ptb->pid = (pid_t) atomic_read(&get_sys_tasks_info()->pid_temp);
	uint32_t t = dis_cpu_intr();

    if(ptb->exec_id >= 0){

    	exec_info = exec_get(new_ptb->exec_id);
		if (!exec_info) {
			free_align(new_ptb->knl_low_stack);
			free(new_ptb);
			restore_cpu_intr(t);
			return -1;
		}

		new_ptb->mpu = (mpu_t *)malloc(sizeof(mpu_t));
		if (!new_ptb->mpu) {
			exec_put(new_ptb->exec_id);
			free_align(new_ptb->knl_low_stack);
			free(new_ptb);
			restore_cpu_intr(t);
			return -ENOMEM;
		}

    	uint32_t pre_alloc_addr;
    	struct mem_heap *heap;

    again_alloc:
    	heap = knl_mem_get_free(new_ptb->user_ram_size, &pre_alloc_addr);
    	if (!heap) {
    		free(new_ptb->mpu);
    		exec_put(new_ptb->exec_id);
    		if (new_ptb->mpu) {
    			free(new_ptb->mpu);
    		}
    		free_align(new_ptb->knl_low_stack);
    		free(new_ptb);
    		kprint("The system is low on memory.\n");
    		knl_mem_trace();
    		restore_cpu_intr(t);
    		return -ENOMEM;
    	}

    	int need_align;
    	//重新设置mpu
    	mpu_set(new_ptb->mpu->mpu_info, (void *)pre_alloc_addr, new_ptb->user_ram_size, &need_align);

    	uint32_t last_data;

    	last_data = new_ptb->user_ram;
    	/*3.分配ram内存*/
    	new_ptb->user_ram = malloc_align(new_ptb->user_ram_size, need_align);
    	if (!new_ptb->user_ram) {
    		free(new_ptb->mpu);
    		exec_put(new_ptb->exec_id);
			free_align(new_ptb->knl_low_stack);
			free(new_ptb);
			restore_cpu_intr(t);
    		kprint("The system is low on memory.\n");
    		return -ENOMEM;
    	}
    	if (new_ptb->user_ram != new_ptb->mpu->mpu_info[0].start_addr) {
    		kprint("Again.\n");
    		free_align(new_ptb->user_ram);
    		goto again_alloc;
    	}
		new_ptb->mem_low_stack = ((uint32_t)new_ptb->user_ram
				+ exec_info->i.stack_offset
				- exec_info->i.data_offset);

		mkrtos_memcpy(new_ptb->user_ram, ptb->user_ram, new_ptb->user_ram_size);
		reloc(exec_info, last_data,new_ptb->user_ram, (uint32_t *)exec_info);
    }

	int32_t err;
	new_ptb->parent = NULL;
	/*通过优先级添加任务*/
	err = add_task(new_ptb, 1);
	if (err != 0) {
		/*释放申请的内存*/
		free(new_ptb->mpu);
        free_align(new_ptb->knl_low_stack);
        free_align(new_ptb->user_ram);
		exec_put(new_ptb->exec_id);
		if (new_ptb->mpu) {
			free(new_ptb->mpu);
		}
		free(new_ptb);
		restore_cpu_intr(t);
		return -1;
	}

	atomic_inc(&get_sys_tasks_info()->pid_temp);
	//设置父进程
	new_ptb->parent_task = ptb;
	//复制栈
	mkrtos_memcpy(new_ptb->knl_low_stack, ptb->knl_low_stack,
			sizeof(uint32_t) * (new_ptb->kernel_stack_size));


	if (new_ptb->user_stack_size != 0) {
		//复制栈
		mkrtos_memcpy(new_ptb->mem_low_stack, ptb->mem_low_stack,
					sizeof(uint32_t) * (new_ptb->user_stack_size));

		new_ptb->sk_info.user_stack =
				(ptr_t) ((uint32_t) (new_ptb->mem_low_stack)
						+ ((uint32_t) (user_knl_sp) - (uint32_t) (ptb->mem_low_stack)));

		//设置用户栈位置
		new_ptb->sk_info.knl_stack =
				((ptr_t) (((uint32_t)(&(((uint32_t*) new_ptb->knl_low_stack)[ptb->kernel_stack_size
						- 1]))) & (~0x7)));

		uint32_t *user_psp = (uint32_t *)get_psp();
		//设置子进程返回值为0
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = 0;
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-1];
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-2];
		new_ptb->sk_info.user_stack -= 4;
		//设置子进程的r9寄存器值
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = (uint32_t)new_ptb->user_ram;
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-4];
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-5];/*TODO:*/
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-6];
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-7];
		new_ptb->sk_info.user_stack -= 4;
		((uint32_t*) (new_ptb->sk_info.user_stack))[0] = user_psp[-8];

		new_ptb->sk_info.stack_type = 1;
	} else {
		mkrtos_memcpy(new_ptb->knl_low_stack, ptb->knl_low_stack,
							sizeof(uint32_t) * (new_ptb->kernel_stack_size));
		//设置内核栈位置
		new_ptb->sk_info.knl_stack =
						(ptr_t) ((uint32_t) (new_ptb->knl_low_stack)
								+ ((uint32_t) (user_knl_sp) - (uint32_t) (ptb->knl_low_stack)));

		new_ptb->sk_info.user_stack = (ptr_t) (~(0L));
		new_ptb->sk_info.stack_type = 0;
	}
	restore_cpu_intr(t);


	//对于打开的文件，我们应当对其inode的引用进行+1的操作。
	for (int i = 0; i < NR_FILE; i++) {
		if (new_ptb->files[i].used) {
			struct inode *tmp;
#if MKRTOS_USED_NET
            if(new_ptb->files[i].net_file){
                lwip_fork(new_ptb->files[i].net_sock);
            }else
#endif
			{
				tmp = new_ptb->files[i].f_inode;
				if (tmp) {
					if (i < 3) {
						tmp->i_ops->default_file_ops->open(tmp,
								&new_ptb->files[i]);
					}
#if MKRTOS_USED_M_PIPE
                    //对pipe进行fork
                    do_fork_pipe(tmp, new_ptb, i);
#endif
					atomic_inc(&(tmp->i_used_count));
				}
			}
		}
	}
	//引用计数器+1
	if (new_ptb->root_inode) {
		atomic_inc(&new_ptb->root_inode->i_used_count);
	}
	if (new_ptb->pwd_inode) {
		atomic_inc(&new_ptb->pwd_inode->i_used_count);
	}
	new_ptb->del_wait = NULL;
	new_ptb->close_wait = NULL;
	new_ptb->sig_bmp[0] = 0;
	new_ptb->sig_bmp[1] = 0;
	new_ptb->sig_mask[0] = 0;
	new_ptb->sig_mask[1] = 0;
	for (int i = 0; i < _NSIG; i++) {
		new_ptb->signals[i]._u._sa_handler = SIG_DFL;
	}

	new_ptb->parent_ram_offset = (uint32_t)(new_ptb->user_ram) - (uint32_t)(ptb->user_ram);
	new_ptb->status = TASK_RUNNING;

	//返回pid
	return new_ptb->pid;
}
