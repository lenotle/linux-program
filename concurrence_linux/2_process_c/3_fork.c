#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>


int main()
{
	pid_t pid = -1;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return 1;
	}

	if (pid == 0)
	{
		while(1)
		{
			printf("son:\t pid = %d, ppid = %d\n", getpid(), getppid());
			sleep(1);
		}
	}
	else if (pid > 0)
	{
		while(1)
		{
			printf("parent:\t pid = %d, cpid = %d\n", getpid(), pid);
			sleep(1);
		}
	}

	return 0;
}

