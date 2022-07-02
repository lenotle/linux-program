#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <signal.h>
#include<unistd.h>
// int kill(pid_t pid, int sig);
// pid belong four status, equal to waitpid()
		

int main()
{
	pid_t pid = -1;

	pid = fork();

	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	if (pid == 0)
	{
		while(1)
		{
			printf("child process doing work..\n");
			sleep(1);
		}
		exit(0);
	}
	else 
	{
		printf("parent process wait child process...\n");
		sleep(3);
		kill(pid, 15); // 15 SIGTERM
	}

	return 0;
}
