#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
//  int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

void fun(int signum)
{
	printf("铺捉到定时器信号:%d\n", signum);
}

int main()
{
	int ret = -1;
	struct itimerval tmo;

	// first process time 
	tmo.it_value.tv_sec  = 2;
	tmo.it_value.tv_usec = 0;

	// periodic timer 
	tmo.it_interval.tv_sec = 1;
	tmo.it_interval.tv_usec = 0;
	
	// 捕捉定时器信号
	signal(SIGALRM, fun);

	ret = setitimer(ITIMER_REAL, &tmo, NULL);
	if (-1 == ret)
	{
		perror("setitimer");
		return 1;
	}

	printf("press any key to continue....\n");
	getchar();
	return 0;
}
