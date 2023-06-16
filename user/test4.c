#include <lib.h>
/**
 * 在课程组的写时复制特性上增加了注释以及对信号继承的测试
*/
void handler(int num) {
    //u_int id = syscall_getenvid;
    debugf ("---Reach handler, now the signum is %d!----\n" ,num);
    //kill(0,2);//不断处理不断发送  测试健壮性

}
struct sigset_t set2;

int main(int argc, char **argv) {
    syscall_set_user_signal_entry(0,signal_entry);

    //安装信号处理函数 1 2----》handler
    struct sigaction sig1;
    struct sigset_t set1;
    sigemptyset(&set1);
    sig1.sa_handler = handler;
    sig1.sa_mask = set1;
    panic_on(sigaction(1 , &sig1 , NULL));
    panic_on(sigaction(2 , &sig1 , NULL));

    //信号集set 用来检测COW
    struct sigset_t set;
    sigemptyset(&set);
    sigemptyset(&set2);
    sigaddset(&set, 1);
    sigaddset(&set, 2);
    panic_on(sigprocmask(0, &set, NULL));
    debugf("-----------------父进程将阻塞signal-1&signal-2-----------------------------\n");
    kill(0,1);
    kill(0,2);
    sigdelset(&set, 2);
    int ret = fork();
    if (ret != 0) {
        debugf("------------------父进程,初始set = %d--------------------------\n",set);
        debugf("-----------------父进程将解除对所有信号的阻塞,开始处理signal-1&2,并把自己的局部变量set从原始的1赋值为3(二进制的11)-----------------------------\n");
        panic_on(sigprocmask(2, &set2, &set));
        debugf("Father: %d.\nset:%d\n", sigismember(&set, 2),set);
    } else {
        debugf("------------------子进程,初始set = %d--------------------------\n",set);
        debugf("-----------------子进程从父进程继承了待处理的signal-1&signal-2以及对应信号处理函数,同时也都被阻塞了-----------------------------\n");
        debugf("-----------------子进程正在等待信号的阻塞被解除----------------------\n");
        debugf("Child: %d.\nset:%d\n", sigismember(&set, 2),set);
        debugf("-----------------子进程将解除对所有信号的阻塞,开始处理signal-1&2-----------------------------\n");
        panic_on(sigprocmask(2, &set2, NULL));
    }
    return 0;
}