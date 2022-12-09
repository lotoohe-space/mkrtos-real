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
// ������
int sys_ftime() {
	return -ENOSYS;
}

//������
int sys_break() {
	return -ENOSYS;
}

// ���ڵ�ǰ���̶��ӽ��̽��е���(degugging)��
int sys_ptrace(int request, int pid, int addr, int data) {
	return -ENOSYS;
}

// �ı䲢��ӡ�ն������á�
int sys_stty() {
	return -ENOSYS;
}

// ȡ�ն���������Ϣ��
int sys_gtty() {
	return -ENOSYS;
}

int sys_prof() {
	return -ENOSYS;
}
int sys_setregid(int rgid, int egid) {
	return -ENOSYS;
}
// ���ý������(gid)���������û�г����û���Ȩ��������ʹ��setgid()������Чgid
// ��effective gid������Ϊ���䱣��gid(saved gid)����ʵ��gid(real gid)�����������
// �����û���Ȩ����ʵ��gid����Чgid �ͱ���gid �������óɲ���ָ����gid��
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

// �򿪻�رս��̼��ʹ��ܡ�
int sys_acct() {
	return -ENOSYS;
}

// ӳ�����������ڴ浽���̵������ַ�ռ䡣
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
// ���ش�1970 ��1 ��1 ��00:00:00 GMT ��ʼ��ʱ��ʱ��ֵ���룩�����tloc ��Ϊnull����ʱ��ֵ
// Ҳ�洢�����
int sys_time(time_t *tloc) {
	time_t tick = 0;
	//����Ӧ��ʹ��rtc�����豸����������������rtc.c

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
//�����û�id
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
// ��ȡ��ǰ����ʱ�䡣tms �ṹ�а����û�ʱ�䡢ϵͳʱ�䡢�ӽ����û�ʱ�䡢�ӽ���ϵͳʱ�䡣
int sys_times(struct tms *tbuf) {
	if (!tbuf) {
		return -EINVAL;
	}
	tbuf->tms_utime = get_current_task()->run_count;
	//��������������˵�ɣ����ں���ûɶ��
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
// ������end_data_seg ��ֵ��������ϵͳȷʵ���㹻���ڴ棬���ҽ���û�г�Խ��������ݶδ�С
// ʱ���ú����������ݶ�ĩβΪend_data_seg ָ����ֵ����ֵ������ڴ����β����ҪС�ڶ�ջ
// ��β16KB������ֵ�����ݶε��½�βֵ���������ֵ��Ҫ��ֵ��ͬ��������д�������
// �ú����������û�ֱ�ӵ��ã�����libc �⺯�����а�װ�����ҷ���ֵҲ��һ����
int sys_brk(unsigned long end_data_seg) {
	//û�������ڴ�ĸ��ֱ�ӷ��سɹ�
	return 0;
}
/**
 * ������ID
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
 * �ڽ��̴���һ�����ļ�����Ŀ¼ʱ����һ����ʹ���ļ���ʽ���������� (����3 . 3��3 . 4�ڣ�
 * ����������˵���� o p e n��c r e a t��������������������һ������ m o d e����ָ�������ļ��Ĵ�ȡ
 * ���Ȩλ)�����ǽ���4 . 2 0��˵����δ���һ����Ŀ¼�����ļ���ʽ������������Ϊ 1��λ����
 * �ļ�m o d e�е���Ӧλ��һ����ת��0��
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

// ȡ��ǰ���̺�pid��
int32_t sys_getpid(void) {
	return get_current_task()->pid;
}

// ȡ�����̺�ppid��
int32_t sys_getppid(void) {
	if (get_current_task()->parent_task) {
		return get_current_task()->parent_task->pid;
	} else {
		return -1;
	}
}

// ȡ�û���uid��
int32_t sys_getuid(void) {
	return get_current_task()->ruid;
}

// ȡeuid��
int32_t sys_geteuid(void) {
	return get_current_task()->euid;
}

// ȡ���gid��
int32_t sys_getgid(void) {
	return get_current_task()->rgid;
}

// ȡegid��
int32_t sys_getegid(void) {
	return get_current_task()->egid;
}

