#include <lib.h>


/**
 * 测试fork后的父子进程的对信号的继承和信号处理函数的继承
*/
int global = 0;
void handler (int num) {
    debugf ("Reach handler , now the signum is %d!\n" , num) ;
    global = 1;
}
struct sigset_t set2 ;
int main (int argc , char ** argv ) {
    struct sigset_t set ;
    struct sigaction sig;
    sigemptyset (&set) ;
    sigaddset (&set , 1) ;
    sigaddset (&set , 2) ;
    sig.sa_handler = handler ;
    sig.sa_mask = set ;
    panic_on(sigaction(2 , &sig , NULL));
    panic_on(sigprocmask(0 , &set , NULL));
    kill(0,1);
    kill(0,2);
    //在当前进程阻塞1 2 号信号时发送了1 2 号信号给自己
    sigdelset(&set ,1) ;
    int ret = fork(),temp=0;
    if( ret != 0) {
        sigprocmask(0,&set,&set2);//解除对2号信号的阻塞  使得立即处理2号信号  但1号还在阻塞
        kill(ret ,2);
        for(int i =0;i <10000000; i++) 
            temp+=i;
        debugf("Father : global %d.\n" , global);
    } else{
        for ( int i =0;i <10000000; i++) 
            temp+=i ;
        debugf ("Child : global %d.\n" ,global) ;
    }
    return 0;
}