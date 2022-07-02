#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

// 僵尸进程， 子进程退出，父进程未回收子进程资源  
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
		for (int i = 0; i < 10; i++)
		{
			printf("child doing cul %d\n", i);
			sleep(1);
		}
		exit(1);
	}

	printf("parent sleep...\n");
	sleep(100);



	return 0;
}

