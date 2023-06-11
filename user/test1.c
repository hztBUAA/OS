#include <lib.h>
 
void my_function(int arg)
{
	int i = 5;
	printf("this is %s \n", __FUNCTION__);
	for(; i > 0; i--)
	{
		printf("倒计时 [%d] second \n", i);
		// sleep(1);		
	}
 
}
int main(void)
{
	//pid_t pid;
	int stat;
	struct sigaction new;
	struct sigaction old;
	
	new.sa_handler = my_function;
	//new.sa_flags = 0;
	
	sigemptyset(&new.sa_mask);
	//sigaddset(&new.sa_mask, SIGINT);
	//sigaction(SIGQUIT, &new, &old);
	debugf("按下 (CTR + '\') 发送信号\n");
	int r  =0 ;
	if (( r = syscall_mem_alloc(0,UCOW,PTE_V))<0)
	{
		debugf("ok1");
		/* code */
	}else{
		debugf("okk1");
	}
	
	// for(;;)
	// {
	// 	pause();		
	// }
 
	exit();
}
 