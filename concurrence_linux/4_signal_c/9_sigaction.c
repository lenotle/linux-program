#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
//        struct sigaction {
//               void     (*sa_handler)(int);
//               void     (*sa_sigaction)(int, siginfo_t *, void *);
//               sigset_t   sa_mask;
//               int        sa_flags;
//               void     (*sa_restorer)(void);
//           };

void fun(int signum)
{
	printf("捕捉到2号信号\n");
}

void fun1(int signum, siginfo_t *sig, void *context)
{
	printf("捕捉到2号信号\n");
}

int main()
{
	int ret = -1;
	
	struct sigaction act;
#if 0
	act.sa_handler = fun;
	act.sa_flags = 0; // default 
#else
	act.sa_sigaction = fun1;
	act.sa_flags = SA_SIGINFO;
#endif	
	ret = sigaction(SIGINT, &act, NULL);
	if (-1 == ret)
	{
		perror("sigaction");
		return 1;
	}

	while(1)
	{
		getchar();
	}	
	return 0;
}
