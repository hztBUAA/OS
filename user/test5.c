#include <lib.h>


/**
 * 测试fork后的父子进程的对信号的继承和信号处理函数的继承   以及父子进程对局部变量的写时复制特性
 * 可算是信号的强测！！！！！！  结果符合预期 并提供了注释 供助教审核
*/
int global = 0;

void handler (int num) {
    //u_int id = syscall_getenvid;
    debugf ("Reach handler, now the signum is %d!\n" ,num);
    //kill(0,2);//不断处理不断发送  测试健壮性
    global = 1;
}

void handler1 (int num) {
    //u_int id = syscall_getenvid;
    debugf("Test for signal handler signal-%d\n",num);
    //debugf ("Env-%x Reach handler , now the signum is %d!\n" ,id, num);
    //kill(0,2);//不断处理不断发送  测试健壮性
    //global = 1;
}

int main (int argc , char ** argv ) {
    u_int id_f = syscall_getenvid();
    debugf("id_f = %x\n",id_f);
    syscall_set_user_signal_entry(0,signal_entry);
    struct sigset_t set2;
    int var = 0;
    struct sigset_t set;
    struct sigaction sig;
    sigemptyset (&set) ;
    
    sig.sa_handler = handler;
    sig.sa_mask = set;
    /////for signal 3~6  can be done once kill   done sequence : 6 5 4 3
    struct sigaction sig1;
    struct sigset_t set1;
    sigemptyset(&set1);
    sig1.sa_handler = handler1;
    sig1.sa_mask = set1;

    panic_on(sigaction(2 , &sig , NULL));
    panic_on(sigaction(1 , &sig , NULL));
    panic_on(sigaction(3 , &sig1 , NULL));
    panic_on(sigaction(4 , &sig1 , NULL));
    panic_on(sigaction(5 , &sig1 , NULL));
    panic_on(sigaction(6 , &sig1 , NULL));
    sigaddset(&set ,1) ;
    sigaddset(&set ,2) ;
    panic_on(sigprocmask(0 , &set , NULL));//父进程的block为3，局部变量set为3
    debugf("blocking the signal-1&2 in father process\n");
    kill(0,1);
    kill(0,2);
    debugf("---------------------父进程接下来接收到signal-3后就会以signal-4的信号掩码(为空),去处理前面发送的signal-1&2------------------------------------\n");
    kill(0,3);
    debugf("---------------------父进程处理完signal-3,接下来想自己发送signal-4并马上处理,同理signal-4~6\n");
    kill(0,4);
    kill(0,5);
    kill(0,6);

    //在当前进程阻塞1 2 号信号时发送了1 2 号信号给自己
    sigdelset(&set ,1) ;//父进程的局部变量set变为2
    //debugf("1a\n");
    int ret = fork(),temp=0;
    //int ret =1;
    //debugf("1\n");
    if( ret != 0) {
        debugf("------------------父进程开始解除自己的signal-2的信号掩码(但还会阻塞signal-1),但是在此之前已经处理完所有信号了---------------\n");
        sigprocmask(1,&set,NULL);//父进程的block变为2，解除父进程对1号信号的阻塞，使得父进程立即处理1号信号  但2号还在阻塞   并把全局变量set2变为3
        kill(0,1);
        debugf("------------------父进程给自己发送signal-1,,但被阻塞,接下来将解除阻塞---------------\n");
        sigprocmask(2,&set1,NULL);
        kill(ret ,2);//父进程向子进程发送2号，但因为2号信号是不可靠信号，所以，2号仍然只有一个存在于子进程的等待队列中
        for(int i =0;i <10000000; i++) 
            temp+=i;
        //debugf("3-2\n");
        var = 100;
        global = 999;
        debugf("Father : var %d, global %d.\n" , var,global);
    
    } else{
        sigemptyset (&set) ;
        debugf("-----------------子进程现在继承了父进程的信号处理函数,接下来将全部解除阻塞,并处理刚刚父进程发出的signal-2------------------------------------\n");
        sigprocmask(2,&set,NULL);//子进程可以处理接收到的信号 但是只有两个 ：1 & 2  虽然2信号发送了两次 但是不可靠算作一次
        
        for ( int i =0;i <10000000; i++) 
            temp+=i ;
        //debugf("3-1\n");
        var = 1001;
        debugf ("Child : var %d, before op global %d.\n" ,var,global) ;//因为写时复制 所以一开始是0  父亲写的999不影响子进程
        global = 99991;
        debugf ("Child : var %d, after op global %d.\n" ,var,global) ;
    }
    return 0;
}