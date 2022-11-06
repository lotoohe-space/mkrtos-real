#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

struct tms {
	clock_t tms_utime;/*�Ñ�����cpu�r�g*/
	clock_t tms_stime;/*ϵͳ�������ķѵ�cpuʱ��*/
	clock_t tms_cutime;/*�������ӽ��̵�cpuʱ��*/
	clock_t tms_cstime;/*�������ӽ������ķѵ�ϵͳ���� CPU ʱ��*/
};

clock_t times(struct tms *buf) __THROW;

__END_DECLS

#endif
