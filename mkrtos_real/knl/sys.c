//
// Created by Administrator on 2022/1/9.
//

#include <errno.h>
#include <mkrtos/sched.h>
#include <knl_service.h>
#include <arch/arch.h>
#include <config.h>

#include <sys/utsname.h>
#include <sys/times.h>
#include <sys/resource.h>

char* sys_getenv(char *key) {
	return NULL;
}
int sys_vhangup(void) {
	return -ENOSYS;
}
// 废弃了
int sys_ftime() {
	return -ENOSYS;
}

//废弃了
int sys_break() {
	return -ENOSYS;
}

// 用于当前进程对子进程进行调试(degugging)。
int sys_ptrace(int request, int pid, int addr, int data) {
	return -ENOSYS;
}

// 改变并打印终端行设置。
int sys_stty() {
	return -ENOSYS;
}

// 取终端行设置信息。
int sys_gtty() {
	return -ENOSYS;
}

int sys_prof() {
	return -ENOSYS;
}
int sys_setregid(int rgid, int egid) {
	return -ENOSYS;
}
// 设置进程组号(gid)。如果任务没有超级用户特权，它可以使用setgid()将其有效gid
// （effective gid）设置为成其保留gid(saved gid)或其实际gid(real gid)。如果任务有
// 超级用户特权，则实际gid、有效gid 和保留gid 都被设置成参数指定的gid。
int sys_setgid(gid_t gid) {
//	if (get_current_task()->is_s_user) {
//		get_current_task()->egid = gid;
//		get_current_task()->rgid = gid;
//		get_current_task()->sgid = gid;
//	} else
	{
		if (gid == get_current_task()->rgid
				|| gid == get_current_task()->sgid) {
			get_current_task()->egid = gid;
		} else {
			return -EPERM;
		}
	}
	return 0;
}

// 打开或关闭进程计帐功能。
int sys_acct() {
	return -ENOSYS;
}

// 映射任意物理内存到进程的虚拟地址空间。
int sys_phys() {
	return -ENOSYS;
}

int sys_lock() {
	return -ENOSYS;
}

int sys_mpx() {
	return -ENOSYS;
}

int sys_ulimit() {
	return -ENOSYS;
}
//extern uint32_t rtc_set_tick(uint32_t tick);
//extern uint32_t rtc_get_tick(void);
//extern uint32_t rtc_get_msofsec(void);
// 返回从1970 年1 月1 日00:00:00 GMT 开始计时的时间值（秒）。如果tloc 不为null，则时间值
// 也存储在那里。
int sys_time(time_t *tloc) {
	time_t tick = 0;
	//后面应该使用rtc驱动设备，但是这样是最快的rtc.c

//	tick = rtc_get_tick();
//	if (tloc) {
//		*tloc = tick;
//	}
	return tick;
}
int sys_setreuid(uid_t ruid, uid_t euid) {

	if (ruid != -1) {
		get_current_task()->ruid = ruid;
	}
	if (euid != -1) {
		get_current_task()->euid = euid;
	}

	return -ENOSYS;
}
//设置用户id
int sys_setuid(uid_t uid) {
//	if (get_current_task()->is_s_user) {
//		get_current_task()->euid = uid;
//		get_current_task()->ruid = uid;
//		get_current_task()->suid = uid;
//	} else
	{
		if (uid == get_current_task()->ruid
				|| uid == get_current_task()->suid) {
			get_current_task()->euid = uid;
		} else {
			return -EPERM;
		}
	}
	return 0;
}
int sys_stime(time_t *tptr) {
	if (!tptr) {
		return -EINVAL;
	}
//	if (!get_current_task()->is_s_user) {
//		return -EPERM;
//	}
	//rtc.c
//	extern uint32_t rtc_set_tick(uint32_t tick);
//	rtc_set_tick(*tptr);
	return -ENOSYS;
}
// 获取当前任务时间。tms 结构中包括用户时间、系统时间、子进程用户时间、子进程系统时间。
int sys_times(struct tms *tbuf) {
	if (!tbuf) {
		return -EINVAL;
	}
	tbuf->tms_utime = get_current_task()->run_count;
	//下面三个后面再说吧，现在好像没啥用
	tbuf->tms_cstime = 0;
	tbuf->tms_cutime = 0;
	tbuf->tms_stime = 0;
	return -ENOSYS;
}
int sys_settimeofday(struct timeval *tv, struct timezone *tz) {
	if (!tv) {
		return -EINVAL;
	}
//	if (!get_current_task()->is_s_user) {
//		return -EPERM;
//	}

//	rtc_set_tick(tv->tv_sec + tv->tv_usec / 1000 / 1000);

	return 0;
}
int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {

	if (!tv) {
		return -EINVAL;
	}
	time_t r_tim = { 0 };
//	r_tim = rtc_get_tick();
	tv->tv_sec = r_tim;
	//tv->tv_usec = rtc_get_msofsec() * 1000;
//    DST_NONE     /* not on DST */
//    DST_USA      /* USA style DST */
//    DST_AUST     /* Australian style DST */
//    DST_WET      /* Western European DST */
//    DST_MET      /* Middle European DST */
//    DST_EET      /* Eastern European DST */
//    DST_CAN      /* Canada */
//    DST_GB       /* Great Britain and Eire */
//    DST_RUM      /* Romania */
//    DST_TUR      /* Turkey */
//    DST_AUSTALT  /* Australian style with shift in 1986 */
	if (tz) {
		tz->tz_dsttime = 0;
		tz->tz_minuteswest = -480;
	}

	return -ENOSYS;
}
// 当参数end_data_seg 数值合理，并且系统确实有足够的内存，而且进程没有超越其最大数据段大小
// 时，该函数设置数据段末尾为end_data_seg 指定的值。该值必须大于代码结尾并且要小于堆栈
// 结尾16KB。返回值是数据段的新结尾值（如果返回值与要求值不同，则表明有错发生）。
// 该函数并不被用户直接调用，而由libc 库函数进行包装，并且返回值也不一样。
int sys_brk(unsigned long end_data_seg) {
	//没有虚拟内存的概念，直接返回成功
	return 0;
}
/**
 * 设置组ID
 * @param pid
 * @param pgid
 * @return
 */
int32_t sys_setpgid(int32_t pid, int32_t pgid) {
	uint32_t t;
	struct task *md_task;
	t = dis_cpu_intr();
	md_task = task_find(pid);
	if (!pgid) {
		md_task->pgid = md_task->pid;
	} else {
		md_task->pgid = pgid;
	}
	restore_cpu_intr(t);
	return -ENOSYS;
}
int sys_getpgrp(void) {
	return -ENOSYS;
}
int sys_setsid(void) {
	return -ENOSYS;
}

int sys_uname(struct utsname *name) {
	if (!name) {
		return -1;
	}
	mkrtos_strncpy(name->sysname, SYS_NAME, _UTSNAME_LENGTH);
	mkrtos_strncpy(name->version, SYS_VERSION, _UTSNAME_LENGTH);
	mkrtos_strncpy(name->release, SYS_RELEASE, _UTSNAME_LENGTH);
	mkrtos_strncpy(name->domainname, SYS_NAME, _UTSNAME_DOMAIN_LENGTH);
	mkrtos_strncpy(name->machine, SYS_MACHINE, _UTSNAME_LENGTH);
	mkrtos_strncpy(name->nodename, SYS_NAME, _UTSNAME_NODENAME_LENGTH);
	return -ENOSYS;
}
/**
 * 在进程创建一个新文件或新目录时，就一定会使用文件方式创建屏蔽字 (回忆3 . 3和3 . 4节，
 * 在那里我们说明了 o p e n和c r e a t函数。这两个函数都有一个参数 m o d e，它指定了新文件的存取
 * 许可权位)。我们将在4 . 2 0节说明如何创建一个新目录，在文件方式创建屏蔽字中为 1的位，在
 * 文件m o d e中的相应位则一定被转成0。
 */
mode_t sys_umask(mode_t mask) {
	mode_t old_mask;
	old_mask = get_current_task()->mask;
	get_current_task()->mask = mask;
	return old_mask;
}

int sys_seteuid(uid_t uid) {
//	if (get_current_task()->is_s_user) {
//		get_current_task()->euid = uid;
//	} else
	{
		if (uid == get_current_task()->ruid
				|| uid == get_current_task()->suid) {
			get_current_task()->euid = uid;
		} else {
			return -EPERM;
		}
	}
	return 0;
}
int sys_setegid(gid_t gid) {
//	if (get_current_task()->is_s_user) {
//		get_current_task()->egid = gid;
//	} else
	{
		if (gid == get_current_task()->ruid
				|| gid == get_current_task()->egid) {
			get_current_task()->euid = gid;
		} else {
			return -EPERM;
		}
	}
	return 0;
}

// 取当前进程号pid。
int32_t sys_getpid(void) {
	return get_current_task()->pid;
}

// 取父进程号ppid。
int32_t sys_getppid(void) {
	if (get_current_task()->parent_task) {
		return get_current_task()->parent_task->pid;
	} else {
		return -1;
	}
}

// 取用户号uid。
int32_t sys_getuid(void) {
	return get_current_task()->ruid;
}

// 取euid。
int32_t sys_geteuid(void) {
	return get_current_task()->euid;
}

// 取组号gid。
int32_t sys_getgid(void) {
	return get_current_task()->rgid;
}

// 取egid。
int32_t sys_getegid(void) {
	return get_current_task()->egid;
}

