#include <lib.h>
//考察连续fork的正确性 信号处理的顺序
void handler(int signo){
    debugf("----------deal with the signal-%d---------\n",signo);
}
#define TESTSIG 2
int main(){
    struct sigset_t set1;
    sigemptyset(&set1);
    //sigaddset(&set1,2);
    debugf("set1->sig[0] = %d\n",set1.sig[0]);
    struct sigaction action1;
    action1.sa_handler = handler;
    action1.sa_mask = set1;
    sigaction(TESTSIG,&action1,NULL);
    sigaction(1,&action1,NULL);
    sigaction(3,&action1,NULL);
    sigaction(4,&action1,NULL);
    sigaction(5,&action1,NULL);
    sigaction(6,&action1,NULL);
    sigaction(7,&action1,NULL);
    sigfillset(&set1);
    sigprocmask(0,&set1,NULL);

    kill(0,1);
    int r;
    r = fork();
    if (r == 0)
    {
        kill(0,2);
        r = fork();
        if (r == 0)
        {
            kill(0,3);
            r = fork();
            if (r == 0)
            {
                kill(0,4);
                r = fork();
                if (r == 0)
                {
                    kill(0,5);
                    kill(0,6);
                    kill(0,7);
                    debugf("-------- son-4------\n");
                    sigemptyset(&set1);
                    sigprocmask(2,&set1,NULL);

                }else{
                    debugf("---------son-3-------\n");
                    sigemptyset(&set1);
                    sigprocmask(2,&set1,NULL);
                }
                
            }else{
                debugf("--------son-2-----------\n");
                sigemptyset(&set1);
                sigprocmask(2,&set1,NULL);
            }
            
        }else{
            debugf("--------son-1-----------\n");
            sigemptyset(&set1);
            sigprocmask(2,&set1,NULL);
        }
        
    }else{
        debugf("-----------father---------\n");
        sigemptyset(&set1);
        sigprocmask(2,&set1,NULL);
    }
    
}