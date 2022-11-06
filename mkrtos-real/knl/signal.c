 //
// Created by Administrator on 2022/1/9.
//
#include <arch/arch.h>
#include <type.h>
#include <errno.h>
#include <signal.h>
#include <mkrtos/sched.h>
void do_exit(int32_t exitCode);

static inline int get_bit(uint32_t bits[_NSIG_WORDS], int inx) {
	return (bits[inx >> 5] >> (inx % 32)) & 0x1;
}

static inline void set_bit(uint32_t bits[_NSIG_WORDS], int inx, int status) {
	if (status) {
		bits[inx >> 5] |= (0x1) << (inx % 32);
	} else {
		bits[inx >> 5] &= ~(0x1 << (inx % 32));
	}
}

//����SIGCHLD��������
void sig_chld(struct task *tk) {
	struct sigaction *sig;
	//
	if (tk->status == TASK_CLOSED || tk->parent_task == NULL
			|| tk->parent_task->status == TASK_CLOSED) {
		return;
	}
	tk = get_current_task()->parent_task;
	sig = &(tk->parent_task->signals[SIGCHLD - 1]);
	if (sig->sa_handler != SIG_IGN) {
		//���������SA_NOCLDSTOP��־���򲻷���SIGCHLD�źŸ�
		if (sig->sa_flags & SA_NOCLDSTOP) {
			return;
		}
		inner_set_sig(SIGCHLD);
//        tk->sig_bmp|=(1<<(SIGCHLD-1));
	}
}
int32_t inner_set_sig(uint32_t signum) {
	if (signum < 1 || signum > _NSIG) {
		return -EINVAL;
	}
	//������Ӧ��λ
	set_bit(get_current_task()->sig_bmp, signum - 1, 1);
//    CUR_TASK->sig_bmp[0]|=(1<<(signum-1));
	//�յ��źŵĽ��̶�Ӧ�ñ�����Ϊ����״̬
	if (get_current_task()->status != TASK_RUNNING
			&& get_current_task()->status != TASK_CLOSED) {
		task_run();
	}
	return 0;
}
int32_t inner_set_task_sig(struct task *tk, uint32_t signum) {
	if (signum < 1 || signum > _NSIG) {
		return -EINVAL;
	}
	//������Ӧ��λ
	set_bit(tk->sig_bmp, signum - 1, 1);
	//�յ��źŵĽ��̶�Ӧ�ñ�����Ϊ����״̬
	if (tk->status != TASK_RUNNING && tk->status != TASK_CLOSED) {
		task_run_1(tk);
	}
	return 0;
}
int32_t inner_set_pid_sig(pid_t pid, uint32_t signum) {
	struct task *tk;
	uint32_t t;
	if (signum < 1 || signum > _NSIG) {
		return -EINVAL;
	}

	t = dis_cpu_intr();
	tk = task_find(pid);
	if (tk->status == TASK_CLOSED) {
		restore_cpu_intr(t);
		return -1;
	}
	//������Ӧ��λ
	set_bit(tk->sig_bmp, signum - 1, 1);
//    tk->sig_bmp[0]|=(1<<(signum-1));
	//�յ��źŵĽ��̶�Ӧ�ñ�����Ϊ����״̬
	if (get_current_task()->status != TASK_RUNNING
			&& get_current_task()->status != TASK_CLOSED) {
		task_run_1(tk);
	}
	restore_cpu_intr(t);
	return 0;
}

int32_t sys_sgetmask() {
	return get_current_task()->sig_mask[0];
}

int32_t sys_ssetmask(int32_t newmask) {
	int32_t old = get_current_task()->sig_mask[0];

	get_current_task()->sig_mask[0] = newmask & ~(1 << (SIGKILL - 1))
			& ~(1 << (SIGSTOP - 1));
	return old;
}

int32_t sys_sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	unsigned long tmp_mask[_NSIG_WORDS] = { 0 };

	if (oldset) {
		oldset->sig[0] = get_current_task()->sig_mask[0];
		oldset->sig[1] = get_current_task()->sig_mask[1];
	}
	if (set) {
		switch (how) {
		case SIG_BLOCK:
			//ֵ����Ĺ����ǽ�newset��ָ����źż������������źżӵ���ǰ���ź������У���Ϊ�µ��ź�������(ԭ���ź������� + set������)��
			for (int i = 0; i < _NSIG; i++) {
				if (((*set).sig[0] >> i) & 0x1) {
					get_current_task()->sig_mask[0] |= (1 << i);
				}
			}
			break;
		case SIG_UNBLOCK:
			//������newset��ָ����źż��е��źŴӵ�ǰ���ź��������Ƴ���
			for (int i = 0; i < _NSIG; i++) {
				int bit = get_bit((*set).sig, i);
				if (bit) {
					set_bit(get_current_task()->sig_mask, i, 0);
				}
//                if(((*set).sig[0]>>i)&0x1){
//                    CUR_TASK->sig_mask[0]&=~(1<<i);
//                }
			}
			break;
		case SIG_SETMASK:
			//���õ�ǰ�ź�����Ϊ����newset��ָ����źż������������źš�
			for (int i = 0; i < _NSIG; i++) {
				int bit = get_bit((*set).sig, i);
				set_bit(tmp_mask, i, bit);
//                if(((*set).sig[0]>>i)&0x1){
//                    tmp_mask&=~(1<<i);
//                }
			}
			get_current_task()->sig_mask[0] = tmp_mask[0];
			get_current_task()->sig_mask[1] = tmp_mask[1];
			break;
		}
	}
	return 0;
}
//extern void rt_sigreturn();
/**
 * ��������linux 0.11
 * @param signum
 * @param handler
 * @param restorer
 * @return
 */
int32_t sys_signal(int32_t signum, int32_t handler, int32_t restorer) {
	struct sigaction temp;

	if (signum < 1 || signum > _NSIG || signum == SIGKILL || signum == SIGSTOP) {
		return -EINVAL;
	}

	temp.sa_handler = (void (*)(int)) handler;
	temp.sa_mask.sig[0] = 0;
	temp.sa_mask.sig[1] = 0;
	temp.sa_flags = SA_RESETHAND | SA_NODEFER;
	temp.sa_restorer = (void (*)(void)) restorer;
//	temp.sa_restorer = rt_sigreturn;    //ֱ��ָ��һ��ϵͳ����
	handler = (uint32_t) get_current_task()->signals[signum - 1].sa_handler;
	get_current_task()->signals[signum - 1] = temp;
	return handler;
}
void do_signal_isr(void *sp);
//sigsuspend( )ϵͳ���ý���������TASK_INTERRUPTIBLE״̬��
// ����������������ָ���λ��������ָ���ı�׼�źź�
// ֻ�е�һ��δ�����Եġ�δ�����ε��źű��͵�����ǰʱ��
// �ý��̲Żᱻ���ѡ���Ӧ��sys_sigsuspend( ) ��������ִ����Щ
int sys_sigsuspend(void *psp, const sigset_t *sigmask) {
	int32_t ret;
	sigset_t oldset;
	//����������
	ret = sys_sigprocmask(SIG_SETMASK, sigmask, &oldset);
	sys_pause();

	//ִ���źŴ�����
	//�ں�Ҫ�����û�̬����
	//��α�������û�ջ
	//Ȼ�����û�̬ģʽ����

	do_signal_isr(psp);

	ret = sys_sigprocmask(SIG_SETMASK, &oldset, NULL);

	return -EINTR;
}

// �������act����һ����ָ�룬����ָ��һ���ṹ��ָ����ָ���ź���صĶ�����
// ��ָ���ź�������Ľṹ��
// �������oact����һ����ָ�룬��ô֮ǰ���ź���صĶ��� ���洢�ڲ���oact��ָ���λ�á�
// ��� ����act��һ����ָ�룬�źŵĴ����ǲ���ģ�
// ��ˣ��õ��ÿ����� ��ѯ��ǰ��һ�������źŵĴ���
// SIGKILL��SIGSTOP�ź�Ӧ ��Ӧʹ�����ֻ��ƽ�SIGKILL��SIGSTOP�ź���ӵ��ź������У�
// ��һ����Ӧ��ϵͳǿ��ִ�С��������Ӧ����ϵͳ��ִ�У������ᵼ����ʾ����

//�����sigaction�ṹ��sa_flags�ֶ��������SA_SIGINFO��־�������ģ���
// sa_handler�ֶξͻ�ȷ����ָ���ź���صĶ�����
// ���sa_flags�ֶ��е�SA_SIGINFO��־�����ã�����ʵ��֧��ʵʱ�ź���չѡ���XSI��չѡ�
// sa_sigaction�ֶ�ָ����һ���źŲ�׽������
//sa_flags�ֶο������޸�ָ���źŵ���Ϊ��

int sys_sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oact) {

	if (sig < 1 || sig > _NSIG || sig == SIGKILL || sig == SIGSTOP) {
		return -EINVAL;
	}
	if (!act) {
		return -EINVAL;
	}
	if (oact) {
		*oact = get_current_task()->signals[sig - 1];
	}

	get_current_task()->signals[sig - 1] = *act;
//	get_current_task()->signals[sig - 1].sa_restorer = (void (*)(void)) rt_sigreturn;
	// ��������ź����Լ����źž�����յ�������������Ϊ0�������������α��źš�
	if (get_current_task()->signals[sig - 1].sa_flags & SA_NODEFER) {
		get_current_task()->signals[sig - 1].sa_mask.sig[0] = 0;
		get_current_task()->signals[sig - 1].sa_mask.sig[1] = 0;
	} else {
		set_bit(get_current_task()->signals[sig - 1].sa_mask.sig, sig - 1, 1);
//        CUR_TASK->signals[sig - 1].sa_mask.sig[0] |= (1 << (sig - 1));
	}
//    if(CUR_TASK->signals[sig-1].sa_flags&SA_RESETHAND){
//        CUR_TASK->signals[sig - 1].sa_mask.sig[0]=0;
//        CUR_TASK->signals[sig - 1].sa_mask.sig[1]=0;
//    }
	return 0;
}
int sys_rt_sigaction(int signum, const struct sigaction *action,
		struct sigaction *oldaction) {
	return sys_sigaction(signum, action, oldaction);
}
int sys_rt_sigsuspend(void *psp, const sigset_t *sigmask, int bytes) {
	return sys_sigsuspend(psp, sigmask);
}
/**
 * ���ﴦ���źţ��źŴ��������жϵ��ý�������ģ��źŲ��ܹ���
 * @param signr
 * @return
 */
int32_t do_signal(void *cur_psp, uint32_t signr) {
	struct sigaction *sig;
	if (get_current_task()->status == TASK_CLOSED
			|| get_current_task()->user_stack_size == 0) {
		// �������û������ִ��ϵͳ����
		// �ں��̲߳��ܹ�ִ���ź���
		return -2;
	}
	sig = &(get_current_task()->signals[signr - 1]);
	//�ȸ�λ�ź�
	set_bit(get_current_task()->sig_bmp, signr - 1, 0);
	// sysTasks.currentTask->sig_bmp&=~(1<<(signr-1));
	//�����ź�
	if (sig->sa_handler == SIG_IGN) {
		return -1;
	}
	//Ĭ�ϵ��źŴ�����
	if (sig->sa_handler == SIG_DFL) {

		switch (signr) {
		// ����ź�������������Ҳ����֮�������ء�
		case SIGCONT:
			//����ҵ�����ź��͸���Ҫ�������еĴ���ֹͣ״̬�Ľ��̡�
			// ������յ����źŵĽ��̴���ֹͣ״̬��
			// ��ϵͳĬ�϶�����ʹ�ý��̼������У�����Ĭ�϶����Ǻ��Դ��źš�
			// �յ��źŵĽ��̶��ᱻ�����������ź�ֱ�Ӻ��Ծ�����
		case SIGCHLD:
			return 1;

		case SIGSTOP:
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
			//����ǰ����
			task_suspend();
//                CUR_TASK->status = TASK_SUSPEND;
			get_current_task()->exit_code = signr;
			sig_chld(get_current_task());
			return (1); /* Reschedule another event */
			// ����ź�������6���ź�֮һ����ô���źŲ�����core dump�������˳���Ϊsignr|0x80
			// ����do_exit()�˳��������˳�������ź�ֵ��do_exit()�Ĳ����Ƿ�����ͳ����ṩ���˳�
			// ״̬��Ϣ������Ϊwait()��waitpid()������״̬��Ϣ���μ�sys/wait.h�ļ���13-18�С�
			// wait()��waitpid()������Щ��Ϳ���ȡ���ӽ��̵��˳�״̬����ӽ�����ֹ��ԭ���źţ���
		case SIGQUIT:
		case SIGILL:
		case SIGTRAP:
		case SIGIOT:
		case SIGFPE:
		case SIGSEGV:
			//ֱ�Ӹɵ�����
			//���ﻹӦ�ý���core_dump
//                if (core_dump(signr))
			do_exit((signr) | 0x80);
			return 0;
		default:
			//ֱ�Ӹɵ�����
			do_exit(signr);
			return 0;
		}
	}

	if (sig->sa_flags & SA_ONSTACK) {
		//�Ƿ���ñ���ջ

	}
	uint32_t *_psp;
	_psp = cur_psp;
	//����������ѹ�������б�
	*(_psp) = sig->sa_mask.sig[0];
	*(--_psp) = sig->sa_mask.sig[1];
	*(--_psp) = (uint32_t) 0x01000000L; /* xPSR */
	*(--_psp) = ((uint32_t) sig->sa_handler); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--_psp) = (uint32_t) sig->sa_restorer;/*LR*/
	*(--_psp) = (uint32_t) 0x12121212L; /* R12*/
	*(--_psp) = (uint32_t) 0x03030303L; /* R3 */
	if (sig->sa_flags & SA_SIGINFO) {
		struct siginfo siginfo;
		siginfo.si_signo = signr;
		siginfo.si_errno = 0;
		siginfo.si_code = 0;
		*(--_psp) = (uint32_t) NULL; /* R2 */
		*(--_psp) = (uint32_t) &siginfo; /* R1 */
		*(--_psp) = (uint32_t) signr; /* R0 : argument */
	} else {
		*(--_psp) = (uint32_t) 0x02020202L; /* R2 */
		*(--_psp) = (uint32_t) 0x01010101L; /* R1 */
		*(--_psp) = (uint32_t) signr; /* R0 : argument */
	}

	//�����µ��û�ջ
	set_psp(_psp);
	//���ֻ����һ�Σ�������źŴ�����������ΪĬ��
	if (sig->sa_flags & SA_RESETHAND) {
		sig->sa_handler = SIG_DFL;
		//�ָ�Ĭ��ֵ
		sig->sa_flags = 0;
	}

	get_current_task()->sig_mask[0] |= sig->sa_mask.sig[0];
	get_current_task()->sig_mask[1] |= sig->sa_mask.sig[1];
//    if(!(sig->sa_flags&SA_NODEFER)) {
//        //�ڴ����ź�ʱ�����յ����ź�
//        sysTasks.currentTask->sig_mask |= (1<<(signr-1));
//    }
	//��������źţ���λ�ź�
//    sysTasks.currentTask->sig_bmp&=~(1<<(signr-1));
	return 0;
}
/**
 * �źŷ��ش���
 * @param psp
 * @return
 */
int32_t sys_sigreturn(void *psp) {
	uint32_t *_psp = psp;
	//���SA_NODEFER��־����λsig_mask
//    sysTasks.currentTask->sig_mask &= ~(1<<(_psp[8]-1));
	get_current_task()->sig_mask[0] &= ~(_psp[10]);
	get_current_task()->sig_mask[1] &= ~(_psp[9]);
	void *newPSP = ((uint32_t) psp) + 10 * 4;
	set_psp(newPSP);
	return 0;
}
int32_t sys_rt_sigreturn(void *psp) {
	return sys_sigreturn(psp);
}
int32_t sys_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset,
		int nr) {
	return sys_sigprocmask(how, set, oldset);
}
//#define __NR_rt_sigreturn		(__NR_SYSCALL_BASE+173)
//#define __NR_rt_sigaction		(__NR_SYSCALL_BASE+174)
//#define __NR_rt_sigprocmask		(__NR_SYSCALL_BASE+175)
//#define __NR_rt_sigpending		(__NR_SYSCALL_BASE+176)
//#define __NR_rt_sigtimedwait		(__NR_SYSCALL_BASE+177)
//#define __NR_rt_sigqueueinfo		(__NR_SYSCALL_BASE+178)
//#define __NR_rt_sigsuspend		(__NR_SYSCALL_BASE+179)

void do_signal_isr(void *sp) {
	if (get_current_task() && get_current_task()->sk_info.stack_type != 2
			&& get_current_task()->status != TASK_CLOSED) {
		//���˵�������
		uint32_t bBmp[2];
		bBmp[0] = (~get_current_task()->sig_mask[0])
				& get_current_task()->sig_bmp[0];
		bBmp[1] = (~get_current_task()->sig_mask[1])
				& get_current_task()->sig_bmp[1];
		if ((!bBmp[0]) && (!bBmp[1])) {
			return;
		}
		for (uint32_t i = 0; i < _NSIG; i++) {
			//����ź��Ƿ���Ч
			if (!get_bit(bBmp, i)) {
				continue;
			}
			//�ź�һ��һ��������������򷵻�,�´ε����ڴ�������һ��
			if (do_signal(sp, i + 1) == 1) {
				task_sche();
			}
		}
	}
}
