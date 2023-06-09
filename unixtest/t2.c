#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
int i=0,j=0;
void my_sig(int);

int main(){
    signal(SIGINT,my_sig);
    signal(SIGALRM,my_sig);
    for(;;)if (j>=3)
    {
        printf("alarm %d times .\n",j);
        exit(1);
    }
    
}

void my_sig(int signo){
    if (signo == SIGINT)
    {
        if (j>0)
        {
            exit(-1);
        }
        alarm(1);
        
    }
    else if(signo == SIGALRM){
        j++;
        printf("%d\n",j);
        alarm(1);
    }
{

}    
}

