#include <type.h>
#include <assert.h>
#include <errno.h>
#include <knl_service.h>
#include <mkrtos/exec.h>
#include <arch/atomic.h>
#include <mkrtos.h>
#include <mkrtos/sched.h>
#include <fcntl.h>
int32_t fork_exec(const char *filename, char *const argv[], char *const envp[])
{
	static struct task_create_par tcp;
	struct task *curr_task = get_current_task();
	int32_t pid;
	int32_t ret;
	int fp = sys_open(filename, O_RDONLY, 0777);

	if (fp<0) {
		return -ENOENT;
	}
	sys_close(fp);

	uint32_t t = dis_cpu_intr();
	tcp.task_fun = 0xFFFFFFFF;
	tcp.arg0 = (void*) 0;
	tcp.arg1 = 0;
	tcp.prio = curr_task->prio;
	tcp.exec_id = -1;
	tcp.user_stack_size = 0;
	tcp.kernel_stack_size = 256;
	tcp.task_name = filename;
	pid = task_create(&tcp, FALSE);
	if (pid < 0) {
		restore_cpu_intr(t);
		return pid;
	}

	struct task *new_task = task_find(pid);
	new_task->parent_task = curr_task;
	ret = sys_do_execve(new_task, filename, argv, envp);
	if (ret < 0) {
		shutdown_task(new_task);
//		sys_tasks_dec(new_task);
		restore_cpu_intr(t);
		return ret;
	}
	sys_tasks_inc(new_task);
	restore_cpu_intr(t);
	return pid;
}
