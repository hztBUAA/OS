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

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}





u_int get_time(u_int *us){
	int time = 0;
	int timee = 0;
    if (syscall_read_dev((u_int) & time, 0x15000000, 4) < 0)
        user_panic("time_read panic");
    if (syscall_read_dev((u_int) & time, 0x15000010, 4) < 0)
        user_panic("time_read panic");
    if (syscall_read_dev((u_int) & timee, 0x15000010, 4) < 0)
        user_panic("time_read panic");
    *us = timee;
    return time;
}

void usleep(u_int us) {
	// 读取进程进入 usleep 函数的时间
	u_int timee = 0;
	u_int starttime = get_time(&timee);
	while (1) {
		// 读取当前时间
		u_int curtime = get_time(&timee);
		if ((int)curtime-(int)starttime >= us/1000000+1) {
			return;
		} else {
			syscall_yield();
			// 进程切换
		}
	}
}
