#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler(int signum, siginfo_t *siginfo, void *ucontext);
void init_sigaction(struct sigaction *act);

int main(int argc, char *argv[])
{
    struct sigaction act;
    init_sigaction(&act);

    printf("pid is: [%d]\n", getpid());

    if (sigaction(SIGQUIT, &act, NULL) == -1) {
        perror("sigaction error");
        exit(1);
    }
    raise(SIGQUIT);             /* send SIGQUIT to itself */
    printf("exiting...\n");
    return 0;
}

void init_sigaction(struct sigaction *act)
{
    act->sa_sigaction = handler;
    sigemptyset(&act->sa_mask);
    act->sa_flags     = SA_SIGINFO;
}

void handler(int signum, siginfo_t *siginfo, void *ucontext)
{
    printf("handler called\n");
    printf("signum(SIGQUIT) is: %d\n", signum);
    printf("information in siginfo is:\n");
    printf("\tsi_signo:  %d\n", siginfo->si_signo );
    printf("\tsi_errno:  %d\n", siginfo->si_errno );
    printf("\tsi_code:   %d\n", siginfo->si_code  );
    /* printf("\tsi_trapno: %d\n", siginfo->si_trapno); */
    printf("\tsi_pid:    %d\n", siginfo->si_pid   );
    printf("\tsi_uid:    %d\n", siginfo->si_uid   );
    printf("\tsi_status: %d\n", siginfo->si_status);
    printf("\tsi_utime:  %ld\n", siginfo->si_utime );
    printf("\tsi_stime:  %ld\n", siginfo->si_stime );
    /* printf("\tsi_value:  %d\n", siginfo->si_value ); */
    printf("handler exiting...\n");
}