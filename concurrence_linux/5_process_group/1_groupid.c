#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

// pid_t getpgrp(void);   获取当前进程的进程组ID

// pid_t getpgid(pid_t pid); 获取指定进程的进程组ID 如果pid = 0，那么该函数作用和getpgrp一样

// int setpgid(pid_t pid, pid_t pgid); 改变进程默认所属的进程组。通常可用来加入一个现有的进程组或创建一个新进程组。
// 将参1对应的进程，加入参2对应的进程组中 
int main()
{
	pid_t pid = -1;

	pid = getpgrp();
	if (-1 == pid)
	{
		perror("getpgrp");
		return 1;
	}
	printf("current process gid: %d\n", pid);

	pid = getpgid(0);
	if (-1 == pid)
	{
		perror("getpgid");
		return 1;
	}
	printf("0 pgid: %d\n", pid);


	return 0;
}
