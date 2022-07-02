#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>



/*
       #include <sys/types.h>
       #include <sys/wait.h>

       pid_t wait(int *wstatus)：
      		 等待任意子进程结束，会阻塞
		return：成功：已经结束子进程的进程号
    		失败： -1

       pid_t waitpid(pid_t pid, int *wstatus, int options);

*/ 

int main()
{
	pid_t pid = -1;
	pid_t ret = -1;
	int status = -1;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	// child process
	if (pid == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			printf("child process do thing %d times, pid = %d\n", i, getpid());
			sleep(1);
		}
		exit(10);
	}

	printf("parent process wait child process...\n");
	ret = wait(&status);
	if (ret == - 1)
	{
		perror("wait");
		return 1;
	}
	
	// process exit status 
	// 正常退出
	if (WIFEXITED(status))
	{
		printf("child process normally exit status: %d\n", WEXITSTATUS(status));
	}
	// 异常退出 
	else if (WIFSIGNALED(status))
	{
		printf("child process exception exit status: %d\n", WTERMSIG(status));
	}
	// 暂停 kill -19 
	else if (WSTOPSIG(status))
	{
		printf("child process was paused status: %d\n", WIFCONTINUED(status));
	}
	return 0;
}

