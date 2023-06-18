
#include <lib.h>
//#include <syscall.h>


#define _S(nr) (1<<((nr)-1))


#define _BLOCKABLE (~(_S(SIGKILL) | _S(SIGSTOP)))


int sigemptyset(sigset_t *set) {
    if (set == NULL) {
        return -1;
    }
    set->sig[0] = 0;
	set->sig[1] = 0; 
	// 将信号集中的所有位清零
    //*set = 0;
    
    return 0;
}

int sigfillset(sigset_t *set) {
    if (set == NULL) {
        return -1;
    }
    // set->sig[0] = ~0;
	// set->sig[1] = ~0; 
    // 将信号集中的所有位置位
    //*set = ~0;
    memset((void *)set,-1,sizeof(set));
    return 0;
}

int sigaddset(sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -1;
    }
    
    // 将信号集中对应signo的位置位
	set->sig[signo/32]|=_S(signo%32);
    
    return 0;
}

int sigdelset(sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -1;
    }
    
    // 将信号集中对应signo的位清零
    set->sig[signo/32] &=(~_S(signo%32));
    
    return 0;
}

int sigismember(const sigset_t *set, int signo) {
    if (set == NULL || signo <= 0 || signo > 64) {
        return -1;
    }
    
    // 检查信号集中对应signo的位是否置位
	return (set->sig[signo/32] & _S(signo%32))!=0;
    // int bitmask = 1 << (signo - 1);
    // return (*set & bitmask) != 0;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
    // TODO: 实现根据how参数指定的操作，修改当前进程的信号屏蔽字
    // 可以使用sigprocmask函数或直接操作系统调用来完成该功能

    return syscall_sigprocmask(how,set,oset);
}

int sigpending(sigset_t *set) {
    // TODO: 实现读取当前进程阻塞且未决的信号集，并将结果存储在set所指向的位置
    // 可以使用sigpending函数或直接操作系统调用来完成该功能
    
    return -1; // 暂时返回失败
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
int sys_sigprocmask(int how, sigset_t *set, sigset_t *oset)
{
	sigset_t new_set, old_set = current->blocked;
	int error;

	if (set) {
		//error = verify_area(VERIFY_READ, set, sizeof(sigset_t));
		if (error)
			return error;
		new_set = get_fs_long((unsigned long *) set) & _BLOCKABLE;
		switch (how) {
		case SIG_BLOCK:
			current->blocked |= new_set;
			break;
		case SIG_UNBLOCK:
			current->blocked &= ~new_set;
			break;
		case SIG_SETMASK:
			current->blocked = new_set;
			break;
		default:
			return -EINVAL;
		}
	}
	if (oset) {
		//error = verify_area(VERIFY_WRITE, oset, sizeof(sigset_t));
		if (error)
			return error;
		put_fs_long(old_set, (unsigned long *) oset);
	}
	return 0;
}
