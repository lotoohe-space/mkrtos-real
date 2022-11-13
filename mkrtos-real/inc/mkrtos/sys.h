

extern int sys_setup();        // 0 - ϵͳ������ʼ�����ú�����
extern int sys_exit();         // 1 - �����˳���                 (kernel/exit.c )
extern int sys_fork();         // 2 - �������̡�                 (kernel/system_call.s)
extern int sys_read();         // 3 - ���ļ���                   (fs/read_write.c)
extern int sys_write();        // 4 - д�ļ���                   (fs/read_write.c)
extern int sys_open();         // 5 - ���ļ���                 (fs/open.c)
extern int sys_close();        // 6 - �ر��ļ���                 (fs/open.c)
extern int sys_waitpid();      // 7 - �ȴ�������ֹ��             (kernel/exit.c)
extern int sys_creat();        // 8 - �����ļ���                 (fs/open.c)
extern int sys_link();         // 9 - ����һ���ļ���Ӳ���ӡ�     (fs/namei.c)
extern int sys_unlink();       // 10 - ɾ��һ���ļ���(��ɾ���ļ�)�� (fs/namei.c)
extern int sys_execve();       // 11 - ִ�г���                (kernel/system_call.s)
extern int sys_chdir();        // 12 - ���ĵ�ǰĿ¼��            (fs/open.c)
extern int sys_time();         // 13 - ȡ��ǰʱ�䡣              (kernel/sys.c)
extern int sys_mknod();        // 14 - ������/�ַ������ļ���     (fs/namei.c)
extern int sys_chmod();        // 15 - �޸��ļ����ԡ�            (fs/open.c)
extern int sys_chown();        // 16 - �޸��ļ������������顣    (fs/open.c)
extern int sys_break();        // 17 -                           (kernel/sys.c)*
extern int sys_stat();         // 18 - ʹ��·����ȡ�ļ�״̬��Ϣ��(fs/stat.c)
extern int sys_lseek();        // 19 - ���¶�λ��/д�ļ�ƫ�ơ�   (fs/read_write.c)
extern int sys_getpid();       // 20 - ȡ����id��               (kernel/sched.c)
extern int sys_mount();        // 21 - ��װ�ļ�ϵͳ��            (fs/super.c)
extern int sys_umount();       // 22 - ж���ļ�ϵͳ��            (fs/super.c)
extern int sys_setuid();       // 23 - ���ý����û�id��         (kernel/sys.c)
extern int sys_getuid();       // 24 - ȡ�����û�id��           (kernel/sched.c)
extern int sys_stime();        // 25 - ����ϵͳʱ�����ڡ�        (kernel/sys.c)*
extern int sys_ptrace();       // 26 - ������ԡ�                (kernel/sys.c)*
extern int sys_alarm();        // 27 - ���ñ�����                (kernel/sched.c)
extern int sys_fstat();        // 28 - ʹ���ļ����ȡ�ļ���״̬��Ϣ��(fs/stat.c)
extern int sys_pause();        // 29 - ��ͣ�������С�            (kernel/sched.c)
extern int sys_utime();        // 30 - �ı��ļ��ķ��ʺ��޸�ʱ�䡣(fs/open.c)
extern int sys_stty();         // 31 - �޸��ն������á�          (kernel/sys.c)*
extern int sys_gtty();         // 32 - ȡ�ն���������Ϣ��        (kernel/sys.c)*
extern int sys_access();       // 33 - ����û���һ���ļ��ķ���Ȩ�ޡ�(fs/open.c)
extern int sys_nice();         // 34 - ���ý���ִ������Ȩ��      (kernel/sched.c)
extern int sys_ftime();        // 35 - ȡ���ں�ʱ�䡣            (kernel/sys.c)*
extern int sys_sync();         // 36 - ͬ�����ٻ������豸�����ݡ�(fs/buffer.c)
extern int sys_kill();         // 37 - ��ֹһ�����̡�            (kernel/exit.c)
extern int sys_rename();       // 38 - �����ļ�����              (kernel/sys.c)*
extern int sys_mkdir();        // 39 - ����Ŀ¼��                (fs/namei.c)
extern int sys_rmdir();        // 40 - ɾ��Ŀ¼��                (fs/namei.c)
extern int sys_dup();          // 41 - �����ļ������            (fs/fcntl.c)
extern int sys_pipe();         // 42 - �����ܵ���                (fs/pipe.c)
extern int sys_times();        // 43 - ȡ����ʱ�䡣              (kernel/sys.c)
extern int sys_prof();         // 44 - ����ִ��ʱ������        (kernel/sys.c)*
extern int sys_brk();          // 45 - �޸����ݶγ��ȡ�          (kernel/sys.c)
extern int sys_setgid();       // 46 - ���ý�����id��            (kernel/sys.c)
extern int sys_getgid();       // 47 - ȡ������id��              (kernel/sched.c)
extern int sys_signal();       // 48 - �źŴ���                (kernel/signal.c)
extern int sys_geteuid();      // 49 - ȡ������Ч�û�id��        (kenrl/sched.c)
extern int sys_getegid();      // 50 - ȡ������Ч��id��          (kenrl/sched.c)
extern int sys_acct();         // 51 - ���̼��ʡ�                (kernel/sys.c)*
extern int sys_phys();         // 52 -                           (kernel/sys.c)*
extern int sys_lock();         // 53 -                           (kernel/sys.c)*
extern int sys_ioctl();        // 54 - �豸����������ơ�        (fs/ioctl.c)
extern int sys_fcntl();        // 55 - �ļ�������Ʋ�����        (fs/fcntl.c)
extern int sys_mpx();          // 56 -                           (kernel/sys.c)*
extern int sys_setpgid();      // 57 - ���ý�����id��            (kernel/sys.c)
extern int sys_ulimit();       // 58 - ͳ�ƽ���ʹ����Դ�����     (kernel/sys.c)
extern int sys_uname();        // 59 - ��ʾϵͳ��Ϣ��             (kernel/sys.c)
extern int sys_umask();        // 60 - ȡĬ���ļ����������롣     (kernel/sys.c)
extern int sys_chroot();       // 61 - �ı��Ŀ¼��               (fs/open.c)
extern int sys_ustat();        // 62 - ȡ�ļ�ϵͳ��Ϣ��           (fs/open.c)
extern int sys_dup2();         // 63 - �����ļ������             (fs/fcntl.c)
extern int sys_getppid();      // 64 - ȡ������id��              (kernel/sched.c)
extern int sys_getpgrp();      // 65 - ȡ������id������getpgid(0)��(kernel/sys.c)
extern int sys_setsid();       // 66 - ���»Ự�����г���       (kernel/sys.c)
extern int sys_sigaction();    // 67 - �ı��źŴ�����̡�         (kernel/signal.c)
extern int sys_sgetmask();     // 68 - ȡ�ź������롣             (kernel/signal.c)
extern int sys_ssetmask();     // 69 - �����ź������롣           (kernel/signal.c)
extern int sys_setreuid();     // 70 - ������ʵ��/����Ч�û�id��  (kernel/sys.c)
extern int sys_setregid();     // 71 - ������ʵ��/����Ч��id��    (kernel/sys.c)
extern int sys_sigpending();   // 73 - �����δ������źš�       (kernel/signal.c)
extern int sys_sigsuspend();   // 72 - ʹ���������������̡�     (kernel/signal.c)
extern int sys_sethostname();  // 74 - ������������               (kernel/sys.c)
extern int sys_setrlimit();    // 75 - ������Դʹ�����ơ�         (kernel/sys.c)
extern int sys_getrlimit();    // 76 - ȡ�ý���ʹ����Դ�����ơ�   (kernel/sys.c)
extern int sys_getrusage();    // 77 -
extern int sys_gettimeofday(); // 78 - ��ȡ����ʱ�䡣             (kernel/sys.c)
extern int sys_settimeofday(); // 79 - ���õ���ʱ�䡣             (kernel/sys.c)
extern int sys_getgroups();    // 80 - ȡ�ý����������ʶ�š�     (kernel/sys.c)
extern int sys_setgroups();    // 81 - ���ý������ʶ�����顣     (kernel/sys.c)
extern int sys_select();       // 82 - �ȴ��ļ�������״̬�ı䡣   (fs/select.c)
extern int sys_symlink();      // 83 - �����������ӡ�             (fs/namei.c��767)
extern int sys_lstat();        // 84 - ȡ���������ļ�״̬��       (fs/stat.c��47)
extern int sys_readlink();     // 85 - ��ȡ���������ļ���Ϣ��     (fs/stat.c��69)
extern int sys_uselib();       // 86 - ѡ����⡣
extern int sys_mmap();       // 90 - ѡ����⡣
extern int sys_munmap();    //91
extern int sys_truncate();//92
extern int sys_ftruncate();//93
extern int sys_fchown();//95
extern int sys_getpriority(); //96
extern int sys_setpriority(); //97
extern int sys_socketcall();
extern int sys_wait4();     //114-wait4
extern int sys_ipc();       //117
extern int sys_fsync();
extern int sys_getdents();
extern int sys_clone(); //120
extern int sys_sigprocmask();
extern int sys_sched_yield();//158
extern int sys_nanosleep(); //162
extern int sys_mremap();//163
extern int sys_poll();//168
extern int sys_rt_sigaction(); //174
extern int sys_rt_sigprocmask();
extern int sys_rt_sigsuspend();//179
extern int sys_getcwd();//183
extern int sys_getenv();

extern int sys_rt_sigreturn(void* psp);
extern int sys_mmap2();
extern int sys_readdir();
int sys_fchmod(unsigned int fd, mode_t mode);
extern int sys_fchdir();
int sys_statfs(const char * path, struct statfs * buf);
int sys_fstatfs(unsigned int fd, struct statfs * buf);
extern int sys_sigreturn();

extern int sys_socket();//281
extern int sys_bind();//282
extern int sys_connect();//283
extern int sys_listen();//284
extern int sys_accept();//285
extern int sys_getsockname();//286
extern int sys_getpeername();//287
extern int sys_socketpair();//288
extern int sys_send();//289
extern int sys_sendto();//290
extern int sys_recv();//291
extern int sys_recvfrom();//292
extern int sys_shutdown();//293
extern int sys_setsockopt();//294
extern int sys_getsockopt();//295
extern int sys_sendmsg();//296
extern int sys_recvmsg();//297

extern int sys_subscribe_msg();
extern int sys_publish_msg();
extern int sys_msgreturn();
extern int sys_p2c_addr();
extern int slot_reg(void);
extern int fork_exec(void);

const sys_call_func sys_call_table[] = {
        [0]=sys_setup,//ʵ��
        [1]=sys_exit,//ʵ��
        [2]=sys_fork,//ʵ��
        [3]=sys_read,//ʵ��
        [4]=sys_write,//ʵ��
        [5]=sys_open,//ʵ��
        [6]=sys_close,//ʵ��
        [7]=sys_waitpid,//ʵ��
        [8]= sys_creat,//ʵ��
        [9]=sys_link,//ʵ��
        [10]=sys_unlink,//ʵ��
        [11]=sys_execve,
        [12]=sys_chdir,//ʵ��
        [13]=sys_time,//
        [14]=sys_mknod,
        [15]=sys_chmod,
        [16]= NULL,//sys_chown
        [17]=sys_break,
        [18]=sys_stat,
        [19]=sys_lseek,
        [20]=sys_getpid,
        [21]=sys_mount,
        [22]=NULL,//sys_umount,
        [23]=sys_setuid,
        [24]=sys_getuid,
        [25]=sys_stime,
        [26]=sys_ptrace,
        [27]=sys_alarm,
        [28]=sys_fstat,
        [29]=sys_pause,
        [30]=sys_utime,
        [31]=sys_stty,
        [32]=sys_gtty,
        [33]=sys_access,
        [34]=sys_nice,
        [35]=sys_ftime,
        [36]=sys_sync,
        [37]=sys_kill,
        [38]=sys_rename,
        [39]=sys_mkdir,
        [40]=sys_rmdir,
        [41]=sys_dup,
        [42]=sys_pipe,
        [43]=sys_times,
        [44]=sys_prof,
        [45]=sys_brk,
        [46]=sys_setgid,
        [47]=sys_getgid,
        [48]=sys_signal,
        [49]=sys_geteuid,
        [50]=sys_getegid,
        [51]=sys_acct,
        [52]NULL,//sys_phys,
        [53]=sys_lock,
        [54]=sys_ioctl,
        [55]=sys_fcntl,
        [56]=sys_mpx,
        [57]=sys_setpgid,
        [58]=sys_ulimit,
        [59]=sys_uname,
        [60]=sys_umask,
        [61]=sys_chroot,
        [62]=sys_ustat,
        [63]=sys_dup2,
        [64]=sys_getppid,
        [65]=sys_getpgrp,
        [66]=sys_setsid,
        [67]=sys_sigaction,
        [68]=sys_sgetmask,
        [69]=sys_ssetmask,
        [70]=sys_setreuid,
        [71]=sys_setregid,
        [72]=sys_sigsuspend,
//                           sys_sigpending,
//                           sys_sethostname,
//                           sys_setrlimit,
//                           sys_getrlimit,
//                           sys_getrusage,
       [78]=sys_gettimeofday,
       [79]=sys_settimeofday,
//                           sys_getgroups,
//                           sys_setgroups,
        [82]=sys_select,
        [83]=sys_symlink,
        [84]=sys_lstat,
//                           sys_readlink,
//                           sys_uselib,
        [89]=sys_readdir,
        [90]=sys_mmap,
        [91]=sys_munmap,
        [92]=sys_truncate,
        [93]=sys_ftruncate,
        [94]=sys_fchmod,
        [95]=sys_fchown,
        [96]=sys_getpriority,
        [97]=sys_setpriority,
        [99]=sys_statfs,
        [100]=sys_fstatfs,
        [102]=sys_socketcall,
        [106]=sys_stat,
        [107]=sys_lstat,
        [114]=sys_wait4,
        [117]=sys_ipc,
        [118]=sys_fsync,
        [119]=sys_sigreturn,
        [120]=sys_clone,
        [122]=sys_uname,
        [126]=sys_sigprocmask,
        [133]=sys_fchdir,
        [141]=sys_getdents,
        [158]=sys_sched_yield,
        [162]=sys_nanosleep,
        [163]=NULL,//sys_mremap,
        [168]=sys_poll,
        [173]=sys_rt_sigreturn,
        [174]=sys_rt_sigaction,
        [175]=sys_rt_sigprocmask,
        [179]=sys_rt_sigsuspend,
        [182]=sys_chown,
        [183]=sys_getcwd,
		[192]=sys_mmap2,
        [281]=sys_socket,
        [282]=sys_bind,
        [283]=sys_connect,
        [284]=sys_listen,
        [285]=sys_accept,
        [286]=sys_getsockname,
        [287]=sys_getpeername,
        [288]=sys_socketpair,
        [289]=sys_send,
        [290]=sys_sendto,
        [291]=sys_recv,
        [292]=sys_recvfrom,
        [293]=sys_shutdown,
        [294]=sys_setsockopt,
        [295]=sys_getsockopt,
        [296]=sys_sendmsg,
        [297]=sys_recvmsg,
		[395]=sys_p2c_addr,
		[396]=slot_reg,
		[397]=fork_exec,
};
