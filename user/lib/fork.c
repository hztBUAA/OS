#include <env.h>
#include <lib.h>
#include <mmu.h>


void __attribute__((noreturn)) signal_entry(struct Trapframe *tf,int signo,u_int sa_mask0,u_int sa_mask1,u_int sa_handler){
	//获得待处理信号的处理函数的sa_mask和sa_ahndler 并将当前进程的屏蔽码设置为对应sigaction的sa_mask   
	//注意结果都应该是一个数值 而不是对应内核存储数据的地址---除了那个用户态处理函数 可以地址
	struct sigset_t env_mask;
	struct sigset_t handler_mask;
	handler_mask.sig[0] = sa_mask0;
	handler_mask.sig[1] = sa_mask1;
	//debugf("cow?\n");
	__sighandler_t user_func = (__sighandler_t)sa_handler;
	//debugf("user func 函数：%x\n",user_func);
	int r;
	//debugf("in signal_entry: signo= %d,tf's adddress = %x,tf->EPC:%x,tf->vaddr:%x\n",signo,tf,tf->cp0_epc,tf->cp0_badvaddr);
	r = syscall_sigprocmask(SIG_SETMASK,&handler_mask,&env_mask);////////
	if(r<0){
		user_panic("syscall_sigprocmask exec misatake");
	}
	//debugf("in signal_entry用户态: before the handler\n");
	//根据sa_handler是否为空进行分类讨论
	if (!sa_handler)//默认处理方式
	{
		if (signo == 9 || signo == 11 || signo == 15)//课程组对特定的三种信号的要求默认处理方式
		{
			if (signo == SIGSEGV)
			{
				user_panic("Error:SIGSEGV!\n");
			}
			
			exit();
		}
		//other signal 忽略  直接接着运行后面的set_trapframe
		tf->cp0_epc += 4;
		debugf("no temporary registered sa_handler in user-level");
	}else{
		//debugf("in signal_entry用户态: before the handler\n");
		(*user_func)(signo);//用户自定义的信号处理函数 一定是接收一个signo信号编码作为参数的
		//执行结束后 将当前进程的信号掩码还原成上一次
		
		syscall_sigprocmask(SIG_SETMASK,&env_mask,&handler_mask);
		tf->cp0_epc += 4;
		debugf("return to the next instruction that call a kernel,EPC:%x\n",tf->cp0_epc);
	}
	r = syscall_set_trapframe(0,tf);
	//如果上面的set_trapframe系统调用成功调用，用户栈会恢复，上下文环境会变成tf，此时PC也会更改，不会运行下一条panic
	user_panic("syscall_set_trapframe error");



}


/* Overview:
 *   Map the faulting page to a private writable copy.
 *
 * Pre-Condition:
 * 	'va' is the address which led to the TLB Mod exception.
 *
 * Post-Condition:
 *  - Launch a 'user_panic' if 'va' is not a copy-on-write page.
 *  - Otherwise, this handler should map a private writable copy of
 *    the faulting page at the same address.
 */
static void __attribute__((noreturn)) cow_entry(struct Trapframe *tf) { // 参数为来到用户态之前的tf
	u_int va = tf->cp0_badvaddr;
	u_int perm;

	/* Step 1: Find the 'perm' in which the faulting address 'va' is mapped. */
	/* Hint: Use 'vpt' and 'VPN' to find the page table entry. If the 'perm' doesn't have
	 * 'PTE_COW', launch a 'user_panic'. */
	/* Exercise 4.13: Your code here. (1/6) */
	perm = (vpt[VPN(va)] & 0xFFF);
	// if((perm&PTE_COW) != PTE_COW){
	// 	//user_panic("perm not PTE_COW");
	// 	user_panic("miss something in PTE_COW\n");
	// }
	/* Step 2: Remove 'PTE_COW' from the 'perm', and add 'PTE_D' to it. */
	/* Exercise 4.13: Your code here. (2/6) */
	perm = perm - PTE_COW;
	perm = perm|PTE_D;
	/* Step 3: Allocate a new page at 'UCOW'. */
	/* Exercise 4.13: Your code here. (3/6) */
	syscall_mem_alloc(0,UCOW,perm);///
	/* Step 4: Copy the content of the faulting page at 'va' to 'UCOW'. */
	/* Hint: 'va' may not be aligned to a page! */
	/* Exercise 4.13: Your code here. (4/6) */

	memcpy((void*)UCOW, (void*)ROUNDDOWN(va,BY2PG), BY2PG);
	// Step 5: Map the page at 'UCOW' to 'va' with the new 'perm'.
	/* Exercise 4.13: Your code here. (5/6) */
	syscall_mem_map(0, UCOW, 0, ROUNDDOWN(va,BY2PG), perm);
	// Step 6: Unmap the page at 'UCOW'.
	/* Exercise 4.13: Your code here. (6/6) */
	syscall_mem_unmap(0, UCOW);

	// Step 7: Return to the faulting routine.
	int r = syscall_set_trapframe(0, tf); //恢复处理函数前的tf  这个是为了防止重入  即用户态进行异常处理时 如果进程切换了 或者其他任何打断  可以如同压栈  内核栈都是用一次的即可被覆盖  正是由于有这个set_trapframe
	user_panic("syscall_set_trapframe returned %d", r);
}

/* Overview:
 *   Grant our child 'envid' access to the virtual page 'vpn' (with address 'vpn' * 'BY2PG') in our
 *   (current env's) address space.
 *   'PTE_COW' should be used to isolate the modifications on unshared memory from a parent and its
 *   children.
 *
 * Post-Condition:
 *   If the virtual page 'vpn' has 'PTE_D' and doesn't has 'PTE_LIBRARY', both our original virtual
 *   page and 'envid''s newly-mapped virtual page should be marked 'PTE_COW' and without 'PTE_D',
 *   while the other permission bits are kept.
 *
 *   If not, the newly-mapped virtual page in 'envid' should have the exact same permission as our
 *   original virtual page.
 *
 * Hint:
 *   - 'PTE_LIBRARY' indicates that the page should be shared among a parent and its children.
 *   - A page with 'PTE_LIBRARY' may have 'PTE_D' at the same time, you should handle it correctly.
 *   - You can pass '0' as an 'envid' in arguments of 'syscall_*' to indicate current env because
 *     kernel 'envid2env' converts '0' to 'curenv').
 *   - You should use 'syscall_mem_map', the user space wrapper around 'msyscall' to invoke
 *     'sys_mem_map' in kernel.
 */
static void duppage(u_int envid, u_int vpn) {
	int r;
	u_int addr;
	u_int perm;

	/* Step 1: Get the permission of the page. */
	/* Hint: Use 'vpt' to find the page table entry. */
	/* Exercise 4.10: Your code here. (1/2) */

	perm = (vpt[vpn] & 0xFFF);
	//debugf("in duppage,vpt[vpn]:%x,*(vpt[vpn]:%x),perm:%x\n",vpt[vpn],*(Pte*)(vpt[vpn]),perm);
	addr = (vpn<<PGSHIFT);
	/* Step 2: If the page is writable, and not shared with children, and not marked as COW yet,
	 * then map it as copy-on-write, both in the parent (0) and the child (envid). */
	/* Hint: The page should be first mapped to the child before remapped in the parent. (Why?)
	 */
	/* Exercise 4.10: Your code here. (2/2) */
	//
	if ((perm&PTE_D)!=0 && (perm&PTE_LIBRARY)==0 && (perm&PTE_COW)==0){
		//debugf("fork-debug-1-1\n");
		perm |= PTE_COW;
		perm = perm-PTE_D;
		syscall_mem_map(0, addr, envid, addr,perm);
		syscall_mem_map(0, addr, 0, addr, perm);
	}
	else{
		syscall_mem_map(0, addr, envid, addr,perm);
		//syscall_mem_map(0, addr, 0, addr, perm);
	}


}

/* Overview:
 *   User-level 'fork'. Create a child and then copy our address space.
 *   Set up ours and its TLB Mod user exception entry to 'cow_entry'.
 *
 * Post-Conditon:
 *   Child's 'env' is properly set.
 *
 * Hint:
 *   Use global symbols 'env', 'vpt' and 'vpd'.
 *   Use 'syscall_set_tlb_mod_entry', 'syscall_getenvid', 'syscall_exofork',  and 'duppage'.
 */
int fork(void) {
	u_int child;
	u_int i;
	extern volatile struct Env *env;

	/* Step 1: Set our TLB Mod user exception entry to 'cow_entry' if not done yet. */
	if (env->env_user_tlb_mod_entry != (u_int)cow_entry) {
		try(syscall_set_tlb_mod_entry(0, cow_entry));
	}
	if (env->env_user_signal_entry != (u_int)signal_entry) {
		try(syscall_set_user_signal_entry(0,signal_entry));
	}
	/* Step 2: Create a child env that's not ready to be scheduled. */
	// Hint: 'env' should always point to the current env itself, so we should fix it to the
	// correct value.
	child = syscall_exofork();
	if (child == 0) {
		env = envs + ENVX(syscall_getenvid());
		return 0;
	}

	/* Step 3: Map all mapped pages below 'USTACKTOP' into the child's address space. */
	// Hint: You should use 'duppage'.
	/* Exercise 4.15: Your code here. (1/2) */
	for (i = 0; i < VPN(USTACKTOP); ++i) {
		if ((vpd[i>>10] & PTE_V) && (vpt[i] & PTE_V)) {
            duppage(child, i);
        }
    }
	//debugf("fork-debug-1-2\n");
	/* Step 4: Set up the child's tlb mod handler and set child's 'env_status' to
	 * 'ENV_RUNNABLE'. */
	/* Hint:
	 *   You may use 'syscall_set_tlb_mod_entry' and 'syscall_set_env_status'
	 *   Child's TLB Mod user exception entry should handle COW, so set it to 'cow_entry'
	 */
	/* Exercise 4.15: Your code here. (2/2) */
		syscall_set_tlb_mod_entry( child, cow_entry);////////在父进程中对孩子进程进行时设置？
		//在还没设置子进程为runnable前完成对信号处理函数的copy


		syscall_set_env_status(child, ENV_RUNNABLE);
	//debugf("fork-debug-1-3\n");
	return child;//能运行到这里 是父进程
}

void fork1(){
	debugf("test for undefined\n");
}

