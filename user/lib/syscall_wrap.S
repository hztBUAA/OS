#include <asm/asm.h>

LEAF(msyscall)
	// Just use 'syscall' instruction and return.

	/* Exercise 4.1: Your code here. */
	syscall
<<<<<<< HEAD
	nop  
	jr ra  
=======
	nop  #不能延迟槽  手动加气泡？  co的只用加一个？why？
	jr ra  #?
	nop
>>>>>>> 9a355cf5a2fc316f9c2ed5b8c32a3b5e9791067d
END(msyscall)
