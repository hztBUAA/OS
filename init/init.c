#include <asm/asm.h>
#include <env.h>
#include <kclock.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

// When build with 'make test lab=?_?', we will replace your 'mips_init' with a generated one from
// 'tests/lab?_?'.
#ifdef MOS_INIT_OVERRIDDEN
#include <generated/init_override.h>
#else

void mips_init() {
	printk("init.c:\tmips_init() is called\n");

	// lab2:
	 mips_detect_memory();
	 mips_vm_init();
	 page_init();

	// lab3:
	 env_init();

	// lab3:
	 ENV_CREATE_PRIORITY(user_bare_loop, 1);
	 ENV_CREATE_PRIORITY(user_bare_loop, 2);

	// lab4:
	 ENV_CREATE(user_tltest);
<<<<<<< HEAD
	 ENV_CREATE(user_fktest);
	// ENV_CREATE(user_pingpong);
=======
	 //ENV_CREATE(user_fktest);
	 //ENV_CREATE(user_pingpong);
>>>>>>> 9a355cf5a2fc316f9c2ed5b8c32a3b5e9791067d

	// lab6:
	// ENV_CREATE(user_icode);  // This must be the first env!

	// lab5:
	// ENV_CREATE(user_fstest);
	// ENV_CREATE(fs_serv);  // This must be the second env!
	// ENV_CREATE(user_devtst);

	// lab3:
	 kclock_init();
	 enable_irq();
	while (1) {
	}
}

#endif
