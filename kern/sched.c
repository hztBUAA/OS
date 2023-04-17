#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env   static only init once
	struct Env *e = curenv;
	static int user_time[5];
	/* We always decrease the 'count' by 1.
	 *
	 * If 'yield' is set, or 'count' has been decreased to 0, or 'e' (previous 'curenv') is
	 * 'NULL', or 'e' is not runnable, then we pick up a new env from 'env_sched_list' (list of
	 * all runnable envs), set 'count' to its priority, and schedule it with 'env_run'. **Panic
	 * if that list is empty**.
	 *
	 * (Note that if 'e' is still a runnable env, we should move it to the tail of
	 * 'env_sched_list' before picking up another env from its head, or we will schedule the
	 * head env repeatedly.)
	 *
	 * Otherwise, we simply schedule 'e' again.
	 *
	 * You may want to use macros below:
	 *   'TAILQ_FIRST', 'TAILQ_REMOVE', 'TAILQ_INSERT_TAIL'
	 */
	/* Exercise 3.12: Your code here. */
	//遍历调度队列，找到所有拥有至少一个可运行进程的用户；
	struct Env * var;
	int a[5]={-1,-1,-1,-1,-1};
	int size= 0 ;
	int flag;
	TAILQ_FOREACH(var, &env_sched_list, env_sched_link)                                                  {
		flag =0;
		for(int i = 0;i<size;i++)
		{
			if(a[i] == var->env_user){
				flag  =1;
				break;
			}
		}		
		if(flag == 0){
			a[size++]=var->env_user;
		}
	}

	if (yield || count == 0 || !e|| e->env_status != ENV_RUNNABLE)
	{
		if (curenv && curenv->env_status == ENV_RUNNABLE)
		{
			TAILQ_REMOVE(&env_sched_list, curenv, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, curenv, env_sched_link);
			//该进程所属用户的累计运行时间片数增加，增加数为进程的优先级
			user_time[curenv->env_user]+=curenv->env_pri;

		}
		
		/* code */
		panic_on(TAILQ_EMPTY(&env_sched_list)== 1);//如果是空 错误出
//遍历所有拥有至少一个可运行进程的用户，根据累计运行时间片的数目和用户号，找到下一个要执行的用户u；
		int mintime = 9999999;
		int id  = 999999;
		for(int i =0;i<size;i++){
			if(mintime > user_time[a[i]]){
				mintime = user_time[a[i]];
				id = a[i]; 
			}
			if(mintime == user_time[a[i]]){
				if(a[i]< id){
					id  =a[i];
				}
			}

		}
		struct Env * new = NULL;
		TAILQ_FOREACH(var, &env_sched_list, env_sched_link)  
		{
			if(var->env_user == id){
				new = var;
				break;
			}
			
		}

		//struct Env *new = TAILQ_FIRST(&env_sched_list);
		count = new->env_pri;
		count--;
		env_run(new);
	}
	else{
		count--;
		env_run(e);
	}




}
