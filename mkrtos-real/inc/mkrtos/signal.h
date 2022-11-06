
#include <signal.h>
//#ifndef _SIGNAL_H
//#define _SIGNAL_H
//
//#include <type.h>		// ����ͷ�ļ��������˻�����ϵͳ�������͡�
//
//typedef int sig_atomic_t;	// �����ź�ԭ�Ӳ������͡�
//typedef unsigned int sigset_t;	/* 32 bits */// �����źż����͡�
//
//#define _NSIG 32		// �����ź����� -- 32 �֡�
//#define NSIG _NSIG		// NSIG = _NSIG
//
//// ������Щ��Linux 0.11 �ں��ж�����źš�
//#define SIGHUP 1		// Hang Up -- �ҶϿ����ն˻���̡�
//#define SIGINT 2		// Interrupt -- ���Լ��̵��жϡ�
//#define SIGQUIT 3		// Quit -- ���Լ��̵��˳���
//#define SIGILL 4		// Illeagle -- �Ƿ�ָ�
//#define SIGTRAP 5		// Trap -- ���ٶϵ㡣
//#define SIGABRT 6		// Abort -- �쳣������
//#define SIGIOT 6		// IO Trap -- ͬ�ϡ�
//#define SIGUNUSED 7		// Unused -- û��ʹ�á�
//#define SIGFPE 8		// FPE -- Э����������
//#define SIGKILL 9		// Kill -- ǿ�Ƚ�����ֹ��
//#define SIGUSR1 10		// User1 -- �û��ź�1�����̿�ʹ�á�
//#define SIGSEGV 11		// Segment Violation -- ��Ч�ڴ����á�
//#define SIGUSR2 12		// User2 -- �û��ź�2�����̿�ʹ�á�
//#define SIGPIPE 13		// Pipe -- �ܵ�д�����޶��ߡ�
//#define SIGALRM 14		// Alarm -- ʵʱ��ʱ��������
//#define SIGTERM 15		// Terminate -- ������ֹ��
//#define SIGSTKFLT 16	// Stack Fault -- ջ����Э����������
//#define SIGCHLD 17		// Child -- �ӽ���ֹͣ����ֹ��
//#define SIGCONT 18		// Continue -- �ָ����̼���ִ�С�
//#define SIGSTOP 19		// Stop -- ֹͣ���̵�ִ�С�
//#define SIGTSTP 20		// TTY Stop -- tty ����ֹͣ���̣��ɺ��ԡ�
//#define SIGTTIN 21		// TTY In -- ��̨�����������롣
//#define SIGTTOU 22		// TTY Out -- ��̨�������������
//#define SIGURG		23
//#define SIGXCPU		24
//#define SIGXFSZ		25
//#define SIGVTALRM	26
//#define SIGPROF		27
//#define SIGWINCH	28
//#define SIGIO		29
//#define SIGPWR		30
//#define SIGSYS		31
////����ֹͯͣ��ֹͣ�Ķ�ͯ����ʱ����Ҫ����SIGCHLD��
//#define SA_NOCLDSTOP	0x00000001
////������㲢��׽���źţ�������źŲ�׽����Ϊ��void func(int signo);
////����signo���źŲ�������Ψһ������
////����������£�����ʹ��sa_handler��Ա�������źŲ����ܣ�����Ӧ�ó��򲻵��޸�sa_sigaction ��Ա��
////��������� SA_SIGINFO ���������źţ����źŲ�����������Ϊ��void func(int signo, siginfo_t *info, void *context);
////������������Ĳ��������ݸ��źŲ�����
////���ڶ���������ָ�� siginfo_t���͵Ķ��� �����ź����ɵ�ԭ�򣻵�������������ת��Ϊָ�� ucontext_t���Ͷ����ָ�룬 �������ڴ����ź�ʱ�жϵĽ��ս��̵������ġ�
////����������£�����ʹ��sa_sigaction��Ա�������źŲ����ܣ�����Ӧ�ó��򲻵��޸� sa_handler��Ա��
////si_signo��Ա����ϵͳ���ɵ��źű�š�
////si_errno��Ա���ܰ���������ʵ�ֵĸ��Ӵ�����Ϣ��������㣬��������һ������ţ����ڱ�ʶ���������źŵ�������
////si_code��Ա������ʶ�ź�ԭ��Ĵ��롣
////���si_code��ֵС�ڵ��� 0����ô�ź����ɽ��̲����ģ�si_pid��si_uid�ֱ��ʾ���� ID �ͷ����ߵ���ʵ�û� ID��
////<signal.h> ��ͷ���������й� siginfo_t ���� Ԫ�ص��ź��ض����ݵ� ��Ϣ��
//#define SA_SIGINFO	0x00000004
////��������ˣ�������sigaltstack ( )������һ�����õ��ź�ջ��
////��ô���źŽ������ݸ���ջ�ϵĵ��ý��̡������źŽ��ڵ�ǰ��ջ�д��ݡ�
////���sig��SIGCHLD������SA_flags��û������SA_NOCLDSTOP��־��
////����ʵ��֧��SIGCHLD�źţ���ôֻҪ�����κ��ӽ���ֹͣ��
////��Ӧ��Ϊ���ý��̲���һ��SIGCHLD�źţ�����ֻҪ�����κ�ֹͣ���ӽ��̼�����
////�Ϳ���Ϊ���ý��̲���һ��SIGCHLD�źš����sig��SIGCHLD��
////����SA_flags��������SA_NOCLDSTOP��־����ôʵ�ֲ�Ӧ�������ַ�ʽ����SIGCHLD�źš�
//#define SA_ONSTACK      0x08000000
////������ã��źŵĴ���Ӧ������ΪSIG_DFL�������ڽ����źŴ������ʱӦ���SA_SIGINFO��־��
////note:SIGILL��SIGTRAP�ڽ���ʱ�����Զ���λ��ϵͳ��ĬĬ��ִ����һ���ơ�
////���⣬��������˴˱�־�� sigaction() ����Ϊ�ͺ���Ҳ������ SA_NODEFER ��־һ����
//#define SA_RESETHAND    0x80000000
//// ����ֹ��ָ�����źŴ������(�źž��)�����յ����ź�
//#define SA_NODEFER      0x40000000
//
//#define SA_NOMASK       SA_NODEFER
//#define SA_ONESHOT SA_RESETHAND	// �źž��һ�������ù��ͻָ���Ĭ�ϴ�������
//
//// ���²�������sigprocmask()-- �ı������źż�(������)����Щ�������Ըı�ú�������Ϊ��
//#define SIG_BLOCK 0		/* for blocking signals */
//// �������źż��м��ϸ������źż���
//#define SIG_UNBLOCK 1		/* for unblocking signals */
//// �������źż���ɾ��ָ�����źż���
//#define SIG_SETMASK 2		/* for setting the signal mask */
//// ���������źż����ź������룩��
//
//#define SIG_DFL ((void (*)(int))0)	/* default signal handling */
//// Ĭ�ϵ��źŴ�������źž������
//#define SIG_IGN ((void (*)(int))1)	/* ignore signal */
//// �����źŵĴ������
//
//// ������sigaction �����ݽṹ��
//// sa_handler �Ƕ�Ӧĳ�ź�ָ��Ҫ��ȡ���ж��������������SIG_DFL��������SIG_IGN ������
//// ���źţ�Ҳ������ָ������źź�����һ��ָ�롣
//// sa_mask �����˶��źŵ������룬���źų���ִ��ʱ����������Щ�źŵĴ���
//// sa_flags ָ���ı��źŴ�����̵��źż���������37-39 �е�λ��־����ġ�
//// sa_restorer �ָ�����ָ�룬�����ڱ���ԭ���صĹ���ָ�롣
//// ���⣬���𴥷��źŴ�����ź�Ҳ��������������ʹ����SA_NOMASK ��־��
//struct sigaction
//{
//    union{
//        //ָ���źŲ�׽�������SIG_IGN��SIG_DFL֮һ��ָ�롣
//        void (*sa_handler) (int);
//        //ָ���źŲ�������ָ�롣
//        void (*sa_sigaction) (void);
//    };
//    //Ӱ���ź���Ϊ�������־��
//    int sa_flags;
//    //��ִ���źŲ�׽�Ĺ�����Ҫ��ϵĶ����źż�
//    sigset_t sa_mask;
//    void (*sa_restorer)(void);
//};
//
//
//
//void sig_chld(struct task *tk);
//
//
//#endif /* _SIGNAL_H */
