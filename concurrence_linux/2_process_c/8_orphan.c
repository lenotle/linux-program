#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

// 孤儿进程，父进程退出，子进程还在继续运行，之后会把子进程父进程设置为 init进程，没有危害 
int main()
{
	pid_t pid = -1;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	if (pid > 0)
	{
		printf("parent process exits after a second of sleep\n");
		sleep(1);
		exit(1);
	}

	while(1)
	{
		printf("child process do things..., ppid = %d\n", getppid());
		sleep(1);
	}
	return 0;
}

