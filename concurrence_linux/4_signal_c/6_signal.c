#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// typedef void (*sighandler_t)(int);
// sighandler_t signal(int signum, sighandler_t handler);

// 信号捕捉


void fun1(int signum)
{
	printf("ctrl C 捕捉到信号:%d\n", signum);

}

void fun2(int signum)
{
	printf("ctrl /捕捉到信号:%d\n", signum);
}
int main()
{
	// ctrl c 
	signal(SIGINT, fun1);
	// ctrl /
	signal(SIGQUIT, fun2);
	
	while(1)
	{
		getchar();
	}
	return 0;
}
