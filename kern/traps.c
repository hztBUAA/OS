#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);
void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}
void do_ov(struct Trapframe *tf){
	///
	//
	u_int* va = tf->cp0_epc;
	int op = -1;
	//tf->cp0_epc+=4;
	//printk("cp0_epc:%x,instr:%x\n",tf->cp0_epc,*va);
	curenv->env_ov_cnt++;
	if((((*va)>>26)&0x3f) == 0){
		if(((*va)&0x3f) == 0x20){
			op =0;
		}
		else{
			op = 1;
		}
	}else{
		op = 2;
	}
	int offset = va - ROUNDDOWN(va,BY2PG);
	struct Page * pp;
	if(op == 0){
		pp = page_lookup(curenv->env_pgdir, (u_long )va- offset, NULL);
		u_int addr = page2kva(pp);
		addr += offset;
		printk("addr:%x,offset:%x",addr,offset);
		//printk("before *addr:%x",*(u_int *)addr);
		*(u_int *)addr = (*(u_int *)addr)+1;
		//printk(",after *addr:%x,",*(u_int *)addr);
		printk("add ov handled\n");
	}else if(op == 1){
		pp = page_lookup(curenv->env_pgdir, (u_long )va,NULL);
                 u_int* addr = page2kva(pp)+offset;
		 *(u_int *)addr = (*(u_int *)addr)+1;
		 printk("sub ov handled\n");
	}else{
		printk("addi ov handled\n");
	}
}
