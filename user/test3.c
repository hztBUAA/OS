#include <lib.h>
/**
 * SIGSEGV的空指针报错正常检测 panic在用户态
*/
int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int *test = NULL;
void sgv_handler(int num) {
    debugf("Segment fault appear!\n");
    test = &a[1];
    debugf("test = %d.\n", *test);
    exit();
}

int main(int argc, char **argv) {
    int r = 0;
    r = syscall_set_user_signal_entry(0,signal_entry);
    struct sigset_t set;
    sigemptyset(&set);
    struct sigaction sig;
    sig.sa_handler = sgv_handler;
    sig.sa_mask = set;
    panic_on(sigaction(11, &sig, NULL));
    *test = 10;
    debugf("test = %d.\n", *test);
    return 0;
}