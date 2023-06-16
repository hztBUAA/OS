#include <lib.h>
//单进程 多个信号一次性发送（先阻塞） 考察顺序  考察信号处理函数对全局变量的修改
int global = 0;
void handler1(int num) {
    global = 11;
    debugf("Reach handler1, now the signum is %d,global = %d!\n", num,global);
    
}
void handler2(int num) {
    global = 22;
    debugf("Reach handler2, now the signum is %d,global = %d!\n", num,global);
}
#define TEST_NUM1 5
#define TEST_NUM2 8
int main(int argc, char **argv) {
    int r = 0;
    r = syscall_set_user_signal_entry(0,signal_entry);

    //debugf("r = %d\n",r);
    struct sigset_t set1;
    sigemptyset(&set1);
    struct sigaction sig1;
    sig1.sa_handler = handler1;
    sig1.sa_mask = set1;
    panic_on(sigaction(TEST_NUM1, &sig1, NULL));
    panic_on(sigaction(1, &sig1, NULL));
    panic_on(sigaction(2, &sig1, NULL));
    panic_on(sigaction(3, &sig1, NULL));
    panic_on(sigaction(4, &sig1, NULL));


    struct sigset_t set2;
    sigemptyset(&set2);
    struct sigaction sig2;
    sig2.sa_handler = handler2;
    sig2.sa_mask = set2;
    panic_on(sigaction(TEST_NUM2, &sig2, NULL));


    sigaddset(&set1, TEST_NUM1);
    sigaddset(&set1, 1);
    sigaddset(&set1, 2);
    sigaddset(&set1, 3);
    sigaddset(&set1, 4);
    panic_on(sigprocmask(0, &set1, NULL));
    //阻塞了两种信号5 9
    //debugf("before send signal\n");
    kill(0, TEST_NUM1);
    kill(0, 1);
    kill(0, 2);
    kill(0, 3);
    kill(0, 4);
    
    //全阻塞 到时候解除阻塞时 按顺序  4 3 2 1 5
    //debugf("after send signal\n");
    int ans = 0;
    for (int i = 0; i < 100; i++) {
        ans += i;
    }
    debugf("cal the ans:%d\n",ans);
    //debugf("now the signal is not blocked\n");
    panic_on(sigprocmask(1, &set1, NULL));
    //UNBLOCK 立即处理信号
    debugf("before handler2:  global=%d\n",global);
    kill(0,8);
    debugf("after handler2:  global=%d\n",global);
    
    
    //应该为   按顺序  因此global->11->22  输出22
    debugf("global = %d.\n", global);
    return 0;
}
