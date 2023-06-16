// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	//receive的线程再次被调度时 发送进程的send函数已经执行完了   值env_ipc_value已经存储在当前进程（正在运行的进程）的进程控制块里面
	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}


int sigemptyset(struct sigset_t *set) {
    if (set == NULL) {
        return -1;
    }
    set->sig[0] = 0;
	set->sig[1] = 0; 
	// 将信号集中的所有位清零
    //*set = 0;
    
    return 0;
}

int sigfillset(struct sigset_t *set) {
    if (set == NULL) {
        return -1;
    }
    set->sig[0] = ~0;
	set->sig[1] = ~0; 
    // 将信号集中的所有位置位
    //*set = ~0;
    
    return 0;
}

int sigaddset(struct sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -E_INVAL;
    }
    
    // 将信号集中对应signo的位置位
	set->sig[signo/32]|=_S(signo%32);
    
    return 0;
}

int sigdelset(struct sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -E_INVAL;
    }
    
    // 将信号集中对应signo的位清零
    set->sig[signo/32] &=(~_S(signo%32));
    
    return 0;
}

int sigismember(const struct sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -E_INVAL;
    }
    
    // 检查信号集中对应signo的位是否置位
	return (set->sig[signo/32] & _S(signo%32))!=0;
    // int bitmask = 1 << (signo - 1);
    // return (*set & bitmask) != 0;
}

int sigprocmask(int how, const struct sigset_t *set, struct sigset_t *oset) {
    // TODO: 实现根据how参数指定的操作，修改当前进程的信号屏蔽字
    // 可以使用sigprocmask函数或直接操作系统调用来完成该功能

    return syscall_sigprocmask(how,set,oset);
}

int sigpending(struct sigset_t *set) {
    // TODO: 实现读取当前进程阻塞且未决的信号集，并将结果存储在set所指向的位置
    // 可以使用sigpending函数或直接操作系统调用来完成该功能
    
    return -1; // 暂时返回失败
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
	return syscall_sigaction(signum,act,oldact);
}

/**
 * 信号的发送
信号的发送采用下面的函数：
int kill(u_int envid, int sig);
向进程控制号编号为 envid 的进程发送 sig 信号，注意当 envid = 0 时代表向自身发送信号。该函数成为完成返回 0，如果信号编号超过限制或者进程编号不存在则返回 -1。
*/
int kill(u_int envid, int sig){
	return syscall_kill(envid,sig);
}

// int main() {
//     sigset_t myset;
//     int result;
    
//     // 测试 sigemptyset
//     result = sigemptyset(&myset);
//     if (result == 0) {
//         printf("sigemptyset succeeded\n");
//     } else {
//         printf("sigemptyset failed\n");
//     }
    
//     // 测试 sigfillset
//     result = sigfillset(&myset);
//     if (result == 0) {
//         printf("sigfillset succeeded\n");
//     } else {
//         printf("sigfillset failed\n");
//     }
    
//     // 测试 sigaddset
//     result = sigaddset(&myset, SIGINT);
//     if (result == 0) {
//         printf("sigaddset succeeded\n");
//     } else {
//         printf("sigaddset failed\n");
//     }
    
//     // 测试 sigdelset
//     result = sigdelset(&myset, SIGINT);
//     if (result == 0) {
//         printf("sigdelset succeeded\n");
//     } else {
//         printf("sigdelset failed\n");
//     }
    
//     // 测试 sigismember
//     result = sigismember(&myset, SIGINT);
//     if (result == 1) {
//         printf("SIGINT is a member of the signal set\n");
//     } else if (result == 0) {
//         printf("SIGINT is not a member of the signal set\n");
//     } else {
//         printf("sigismember failed\n");
//     }
    
//     return 0;
// }


/*你需要实现下面的函数来对进程的信号掩码进行修改：
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);*/
