#ifndef _SIGNAL_H_
#define _SIGNAL_H_
#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGUNUSED	 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22

#define SIGIO		23
#define SIGPOLL		SIGIO
#define SIGURG		SIGIO
#define SIGXCPU		24
#define SIGXFSZ		25


#define SIGVTALRM	26
#define SIGPROF		27

#define SIGWINCH	28

/*
#define SIGLOST		29
*/
#define SIGPWR		30

/* Arggh. Bad user source code wants this.. */
#define SIGBUS		SIGUNUSED


#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */

/* Type of a signal handler.  */
typedef void (*__sighandler_t)(int);
#define SIG_DFL	((__sighandler_t)0)	/* default signal handling */
#define SIG_IGN	((__sighandler_t)1)	/* ignore signal */
#define SIG_ERR	((__sighandler_t)-1)	/* error return from signal */

#define _S(nr) (1<<((nr)-1))
#define _BLOCKABLE (~(_S(SIGKILL) | _S(SIGSTOP)))
 struct sigset_t{
    u_int sig[2]; //最多 32*2=64 种信号
};

 

 struct sigaction{
	__sighandler_t sa_handler;
	struct sigset_t sa_mask;
	// int sa_flags;
	// void (*sa_restorer)(void);
};

struct sigqueue{
	int next;//下一个信号结构体的编号
	int signo;//只需记录待处理的信号num即可
	struct sigaction action;
};
extern void do_signal(struct Trapframe *tf);
#endif /* _SIGNAL_H_ */