#include <lib.h>

struct sigset_t set2;

int main(int argc, char **argv) {
    syscall_set_user_signal_entry(0,signal_entry);
    struct sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 1);
    sigaddset(&set, 2);
    panic_on(sigprocmask(0, &set, NULL));
    sigdelset(&set, 2);
    int ret = fork();
    if (ret != 0) {
        panic_on(sigprocmask(0, &set2, &set));
        debugf("Father: %d.\nset:%d\n", sigismember(&set, 2),set);
    } else {
        debugf("Child: %d.\nset:%d\n", sigismember(&set, 2),set);
    }
    return 0;
}