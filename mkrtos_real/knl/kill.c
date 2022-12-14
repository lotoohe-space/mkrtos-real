//
// Created by zz on 2022/2/14.
//
#include <sys/types.h>
#include <mkrtos/sched.h>
#include <arch/arch.h>
/**
 * 
 * @param pid
 * @param sig
 */
int32_t do_kill_pid(pid_t pid, int32_t sig) {
	struct task *tmp;
	uint32_t t;
	t = dis_cpu_intr();
	tmp = task_find(pid);
	if (tmp == get_current_task() && tmp->pid != pid) {
		return -ESRCH;
	}
	if (sig) {
		inner_set_task_sig(tmp, sig);
	}
//   if(sig) {
//       inner_set_sig(sig);
//   }
	restore_cpu_intr(t);
	return 0;
}
/**
 * 
 * @param pgid
 * @param sig
 * @return
 */
int32_t do_kill_group(pid_t pgid, int32_t sig) {
	uint32_t t;
	struct task *tmp;
	t = dis_cpu_intr();

	slist_foreach(tmp, &get_sys_tasks_info()->all_tk_list, all_node) {
		if (pgid == tmp->pgid) {
			if (sig) {
				inner_set_task_sig(tmp, sig);
			}
//            if(sig) {
//                inner_set_sig(sig);
//            }
		}
	}
	restore_cpu_intr(t);
	return 0;
}
/**
 * 
 * @param pid
 * @param sig
 * @return
 */
int32_t sys_kill(pid_t pid, int32_t sig) {
	int ret = 0;
	if (sig < 0 || sig > NSIG) {
		return -EINVAL;
	}
	if (pid > 0) {
		ret = do_kill_pid(pid, sig);
	} else if (pid == 0) {
		ret = do_kill_group(get_current_task()->pgid, sig);
	} else if (pid == -1) {

	} else if (pid < -1) {

	}
	return ret;
}
