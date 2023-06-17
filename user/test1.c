// //#include </user/include/lib.h>
//  #include <stdlib.h>
//  #include <stdio.h>
// void my_function(int arg)
// {
// 	int i = 5;
// 	printf("this is %s \n", __FUNCTION__);
// 	for(; i > 0; i--)
// 	{
// 		printf("倒计时 [%d] second \n", i);
// 		// sleep(1);		
// 	}
// 	asm ("j f1;");
// }

// void f1(){
// 	printf("test for asm -jr\n");
// }
// int main(void)
// {
// 	//pid_t pid;
// 	// int stat;
// 	// struct sigaction new;
// 	// struct sigaction old;
	
// 	// new.sa_handler = my_function;
// 	// //new.sa_flags = 0;
// 	// for (size_t i = 0; i < 6; i++)
// 	// {
// 	// 	fork1();
// 	// }
	
// 	// //to exam the _set function in ipc.c
// 	// sigemptyset(&new.sa_mask);
// 	// debugf("%d\n",new.sa_mask.sig[0]);
// 	// sigaddset(&new.sa_mask, SIGINT);
// 	// debugf("%d\n",new.sa_mask.sig[0]);

// 	//to exam the procmask syscall  should prepare the sigaction
// 	//syscall_sigprocmask(SIG_BLOCK,);

// 	//sigaction(SIGQUIT, &new, &old);
// 	// debugf("按下 (CTR + '\') 发送信号\n");
// 	// int r  =0 ;
// 	// if (( r = syscall_mem_alloc(0,UCOW,PTE_V))<0)
// 	// {
// 	// 	debugf("ok1");
// 	// 	/* code */
// 	// }else{
// 	// 	debugf("okk1");
// 	// }
	
// 	// for(;;)
// 	// {
// 	// 	pause();		
// 	// }
 

//  int a = 5;
//     int b = 10;
//     int result;

    // asm ("sub %%ebx, %%eax;" // 汇编指令
    //      : "=a" (result)      // 输出操作数
    //      : "a" (a), "b" (b)); // 输入操作数

//     printf("The result is: %d\n", result);
// 	exit(-1);
// }
 
//  #include <stdio.h>

// void foo() {
//     printf("Inside foo()\n");
// }

// int main() {
//     void (*functionPtr)() = foo;

//     asm volatile (
//         "call *%0"
//         :
//         : "r" (functionPtr)
//     );

//     printf("Back to main()\n");
//     return 0;
// }
// #include <lib.h>

// int global = 0;
// void handler(int num) {
//     debugf("Reach handler, now the signum is %d!\n", num);
//     global = 1;
// }

// #define TEST_NUM 5
// int main(int argc, char **argv) {
//     int r;
//     r = fork();
//     //syscall_set_user_signal_entry(0,signal_entry);
//     if (r == 0)
//     {
//         struct sigset_t set;
//         sigemptyset(&set);
//         struct sigaction sig;
//         sig.sa_handler = handler;
//         sig.sa_mask = set;
//         panic_on(sigaction(TEST_NUM, &sig, NULL));
//         debugf("before sigaddset:%d\n",set);
//         sigaddset(&set, TEST_NUM);
//         debugf("after sigaddset:%d\n",set);
//         panic_on(sigprocmask(0, &set, NULL));
//         //kill(0, 9);
//         kill(0, TEST_NUM);
//         //kill(0,2);
//         int ans = 0;
//         for (int i = 0; i < 1000; i++) {
//             ans += i;
//         }
//         panic_on(sigprocmask(1, &set, NULL));
//         //debugf("ans = %d\n",ans);
//         debugf("global = %d.\n", global);
//         return 0;
//     }else{
//         int ans = 0;
//         for (int i = 0; i < 100; i++) {
//             ans += i;
//         }
//         debugf("ans = %d\n",ans);
//     }
    
    
// }
#include <lib.h>
/**
 * 简单的信号测试 
*/
int global = 0;
struct sigset_t set1;
void handler(int num) {
    debugf("Reach handler, now the signum is %d,在此之前global为 %d!\n", num,global);
    global = 1;
    debugf("-----------对5号信号的处理使得全局变量变为1----------\n");
}

#define TEST_NUM 5
int main(int argc, char **argv) {
    int r = 0;
    r = syscall_set_user_signal_entry(0,signal_entry);
    debugf("r = %d\n",r);
    struct sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,2);
    debugf("----------after add 2 into set,set = %d-------\n",set);
    sigdelset(&set,2);
    debugf("-----------after del 2 from set,set = %d-------\n",set);
    sigaddset(&set,2);
    debugf("----------now,after add 2 into set,set = %d-------\n",set);
    struct sigaction sig;
    sig.sa_handler = handler;
    sig.sa_mask = set;
    panic_on(sigaction(TEST_NUM, &sig, NULL));
    sigaddset(&set, TEST_NUM);
    debugf("---------进程的掩码将被设置为set,即2、5号均会被阻塞--------\n");
    panic_on(sigprocmask(0, &set, NULL));
    debugf("----------进程给自己发送一个5号信号------------\n");
    kill(0, TEST_NUM);
    //debugf("after send signal\n");
    int ans = 0;
    for (int i = 0; i < 100; i++) {
        ans += i;
    }
    debugf("cal the ans:%d\n",ans);
    debugf("--------now the signal is not blocked,进程的掩码移除了5号,并把全局变量设置为进程原来的掩码,也就是应该是十进制的18--------\n");
    panic_on(sigprocmask(1, &set, &set1));
    debugf("global = %d,进程原来的掩码是set1 = %d.\n", global,set1);
    return 0;
}
