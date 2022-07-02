#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#define SIZE 128

// 使用无名管道进行 父子进程通信
// 父进程写，子进程读
// 由于管道是半双工模式，因此不能同时读和写，二选一
// 如要同时读和写，多个管道配合使
int main()
{
	pid_t pid = -1;
	int ret = -1;
	int fds[2] = {0};
	char buf[SIZE];

	// 1. 创建无名管道
	if (pipe(fds) == -1)
	{
		perror("pipe");
		return 1;
	}

	// 2. 创建子进程 
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return 1;
	}

	// child process read 
	if (pid == 0)
	{
		close(fds[1]);
		
		printf("clild process waiting to read data.\n");
		ret = fcntl(fds[0], F_GETFL);
		ret |= O_NONBLOCK;
		fcntl(fds[0], F_SETFL, ret);
		// read 
		memset(buf, 0, SIZE);
		ret = read(fds[0], buf, SIZE);
		if (ret < 0)
		{
			perror("child process read");
			exit(-1);
		}
		printf("child process read data: %s\n", buf);

		close(fds[0]);
		exit(0);
	}

	// parent process write 
	close(fds[0]);
	sleep(1);
	printf("parent process start write....\n");
	

	// 子进程读端 全部关闭之后，写端会返回一个终止信号，以下代码不会再执行
	ret = write(fds[1], "123456", 6);
	if (ret <0)
	{
		perror("write");
		return 1;
	}
	printf("parent process write %d bytes\n", ret);
	close(fds[1]);

	return 0;
}
