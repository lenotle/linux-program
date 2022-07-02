#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>



/*
       #include <sys/types.h>
       #include <sys/wait.h>
       
       等待子进程终止，如果子进程终止了，此函数会回收子进程的资源
       pid_t waitpid(pid_t pid, int *wstatus, int options);
       参数说明：
       		pid:
	       		 > 0  等待进程 ID 等于 pid 的子进程。
      		
			 = 0  等待同一个进程组中的任何子进程，如果子进程已经加入了别的进程组，waitpid 不会等待它。
     		
			 = -1 等待任一子进程，此时 waitpid 和 wait 作用一样。
      		
			 < -1 等待指定进程组中的任何子进程，这个进程组的 ID 等于 pid 的绝对值。	
		
		status : 进程退出时的状态信息
		
		options : options 提供了一些额外的选项来控制 waitpid()。
           		 0：同 wait()，阻塞父进程，等待子进程退出。
            		 
			 WNOHANG：没有任何已经结束的子进程，则立即返回。
            		 
			 WUNTRACED：如果子进程暂停了则此函数马上返回，并且不予以理会子进程的结束状态。

		返回值：
			当正常返回的时候，waitpid() 返回收集到的已经回收子进程的进程号；

			如果设置了选项 WNOHANG，而调用中 waitpid() 发现没有已退出的子进程可等待，则返回 0

			如果调用中出错，则返回-1，这时 errno 会被设置成相应的值以指示错误所在，如：
		当 pid 所对应的子进程不存在，或此进程存在，但不是调用进程的子进程，waitpid() 就会出错返回，
		这时 errno 被设置为 ECHILD；
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
	
	//ret = wait(&status);
	//ret = waitpid(-1, &status, 0); // equal wait 
	ret = waitpid(-1, &status, WNOHANG); // nonblock
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

