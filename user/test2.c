#include <lib.h>

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


    struct sigset_t set2;
    sigemptyset(&set2);
    struct sigaction sig2;
    sig2.sa_handler = handler2;
    sig2.sa_mask = set2;
    panic_on(sigaction(TEST_NUM2, &sig2, NULL));


    sigaddset(&set1, TEST_NUM1);
    sigaddset(&set1, TEST_NUM2);
    panic_on(sigprocmask(0, &set1, NULL));
    //阻塞了两种信号5 9
    //debugf("before send signal\n");
    kill(0, TEST_NUM1);
    kill(0, TEST_NUM2);
    //debugf("after send signal\n");
    int ans = 0;
    for (int i = 0; i < 100; i++) {
        ans += i;
    }
    debugf("cal the ans:%d\n",ans);
    //debugf("now the signal is not blocked\n");
    panic_on(sigprocmask(1, &set1, NULL));
    //UNBLOCK 立即处理信号
    // for (size_t i = 0; i < 3; i++)
    // {
    //     r++;
    // }
    
    //应该为   按顺序最外层  先处理完TEST_NUM2  在TEST_NUM1  因此global->22->11  输出11
    debugf("global = %d.\n", global);
    return 0;
}
