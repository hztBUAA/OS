#include <env.h>
#include <pmap.h>
#include <signal.h>
#include <malloc.h>
void do_signal(struct Trapframe *tf);
static void passive_alloc(u_int va, Pde *pgdir, u_int asid) {
	struct Page *p = NULL;

	if (va < UTEMP) {
		sys_kill(0,SIGSEGV);
		env_run(curenv);
		//printk("try1\n");
		panic("address too low");
	}

	if (va >= USTACKTOP && va < USTACKTOP + BY2PG) {
		panic("invalid memory");
	}

	if (va >= UENVS && va < UPAGES) {
		panic("envs zone");
	}

	if (va >= UPAGES && va < UVPT) {
		panic("pages zone");
	}

	if (va >= ULIM) {
		panic("kernel address");
	}

	panic_on(page_alloc(&p));
	panic_on(page_insert(pgdir, asid, p, PTE_ADDR(va), PTE_D));
}

/* Overview:
 *  Refill TLB.
 */
Pte _do_tlb_refill(u_long va, u_int asid) {
	Pte *pte;
	/* Hints:
	 *  Invoke 'page_lookup' repeatedly in a loop to find the page table entry 'pte' associated
	 *  with the virtual address 'va' in the current address space 'cur_pgdir'.
	 *
	 *  **While** 'page_lookup' returns 'NULL', indicating that the 'pte' could not be found,
	 *  allocate a new page using 'passive_alloc' until 'page_lookup' succeeds.
	 */

	/* Exercise 2.9: Your code here. */
	while(1) {
		//printk("va = %x,curenv = %d\n",va,curenv->env_id);
		if(!page_lookup(cur_pgdir,va,&pte) ) {
			passive_alloc(va,cur_pgdir,asid);
		}
		else break;
	}
	return *pte;
}

#if !defined(LAB) || LAB >= 4
/* Overview:
 *   This is the TLB Mod exception handler in kernel.
 *   Our kernel allows user programs to handle TLB Mod exception in user mode, so we copy its
 *   context 'tf' into UXSTACK and modify the EPC to the registered user exception entry.
 *
 * Hints:
 *   'env_user_tlb_mod_entry' is the user space entry registered using
 *   'sys_set_user_tlb_mod_entry'.
 *
 *   The user entry should handle this TLB Mod exception and restore the context.
 */
void do_tlb_mod(struct Trapframe *tf) {
	struct Trapframe tmp_tf = *tf;

	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	*(struct Trapframe *)tf->regs[29] = tmp_tf;

	if (curenv->env_user_tlb_mod_entry) {
		tf->regs[4] = tf->regs[29]; //参数 *tf
		tf->regs[29] -= sizeof(tf->regs[4]);
		// Hint: Set 'cp0_epc' in the context 'tf' to 'curenv->env_user_tlb_mod_entry'.
		/* Exercise 4.11: Your code here. */
		tf->cp0_epc = curenv->env_user_tlb_mod_entry;
	} else {
		panic("TLB Mod but no user handler registered");
	}
}

void do_signal(struct Trapframe *tf){
	u_int signo = 0;
	struct sigqueue *s = &curenv->head[curenv->start];
	//u_int i = 0;
	if (curenv->cnt == -1)
	{
		//no pending signal to do
		//printk("no pending signal to do\n");
		return;
	}else{
		while (1)
		{			
			u_int temp = s->signo;
			if (((curenv->blocked[signo/32]) & (1 << ((temp-1)%32))) == 0)//这个temp信号不被当前进程阻塞
			{
				//printk("in do_signal: deal with signal-%d\n",signo);
				signo = temp;
				curenv->signal[signo/32] &= ~(1 << ((temp-1) %32));
				break;
			}
			if (s->next == -1)
			{
				break;
			}
			s = &curenv->head[s->next];
		}
		
	}
	//printk("in do_signal: deal with signal-%d\n",signo);
	if (signo == 0)
	{
		return;//未决信号集都是被阻塞的
	}
	//printk("in do_signal: deal with signal-%d\n",signo);
	//找到就将它从未决信号链表删除 并更新signal位图  所以貌似我的signal位图并没有用 只需链表即可
	//因为是单向链表所以不好进行 查到就删除
	struct sigqueue *ss = &curenv->head[curenv->start];
	if (ss->signo == s->signo)
	{
		if (ss->next == -1)
		{
			//仅一个
			curenv->cnt = -1;//第一个也是空的
			curenv->start = 0;
			curenv->head[curenv->start].next = -1;
		}else{
			curenv->start = ss->next;
		}
	}else{
		while (1)
		{
			if (ss->next == 0||ss->next>=1024)
			{
				panic("Error:do_signal函数对Env结构体的数组链表的信号删除出现mistake!\n");
			}
			if (curenv->head[ss->next].signo == s->signo)
			{
				ss->next = s->next;
				//printk("删掉一个信号-%d,s.next = %d,ss.next = %d\n",s->signo,s->next,ss->next);
				break;
			}
			
			
			ss = &curenv->head[ss->next];
		}
	}
	
	//printk("1-2\n");
	struct Trapframe tmp_tf = *tf;

	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	//printk("1-3-1\n");
	*(struct Trapframe *)tf->regs[29] = tmp_tf;///写入内容 需要进入系统调用的！！！！！！  所以要对jal do_signal进行EPC的约束
	//printk("1-3-2\n");
	if (curenv->env_user_signal_entry) {
		//共5个参数传给用户态的signal_entry		
		tf->regs[4] = tf->regs[29]; //参数 *tf   不能先把这个tf->regs[29]改了！！！！
		tf->regs[29] -= sizeof(tf->regs[4])*5;//压栈5个参数u_int
		tf->regs[5] = signo;
		//signo为1----->下标为0
		//传信号处理函数的sa_mask
		tf->regs[6] = (curenv->info[signo-1]).sa_mask.sig[0];
		tf->regs[7] = (curenv->info[signo-1]).sa_mask.sig[1];
		//arg5 = *((u_int *)(tf->regs[29] + 20));
		//printk("1-1\n");
		*((u_int *)(tf->regs[29] + 16)) = (curenv->info[signo-1]).sa_handler;
		//printk("in tlbex.c/do_signal-- 用户自定义函数的地址(curenv->info[signo-1]).sa_handler:%x\n",(curenv->info[signo-1]).sa_handler);
		// Hint: Set 'cp0_epc' in the context 'tf' to 'curenv->env_user_tlb_mod_entry'.
		/* Exercise 4.11: Your code here. */
		//printk("has a valid signal-%d to do\n",signo);
		tf->cp0_epc = curenv->env_user_signal_entry;
		//printk("get here till no exception 's from kernel?\n");
	} else {
		panic("Try to handle signal but curenv no handler registered");
	}
}
#endif

