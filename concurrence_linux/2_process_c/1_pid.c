#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
	pid_t pid = -1;

	// 获取pid
	pid = getpid();
	printf("pid = %d\n", pid);

	// parent pid 
	pid = getppid();
	printf("parent pid = %d\n", pid);

	//进程组pid 
	pid = getpgid(getpid());
	printf("group pid  = %d\n", pid);
	
	return 0;
}
