#include <asm/asm.h>

LEAF(msyscall)
	// Just use 'syscall' instruction and return.

	/* Exercise 4.1: Your code here. */
	syscall
	nop  #不能延迟槽  手动加气泡？  co的只用加一个？why？
	jr ra  #?
	nop
END(msyscall)
