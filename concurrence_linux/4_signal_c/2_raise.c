#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
// int raise(int sig); 给当前进程发送信号， 等价于 kill(getpid(), SIGTERM);
int main()
{
	int i = 0;
	while(1)
	{
		printf("do working i: %d\n", i);
		if (i == 4)
		{
			kill(getpid(), SIGTERM);
			//raise(SIGTERM);
		}	
		i++;
		sleep(1);
	}

	return 0;
}
