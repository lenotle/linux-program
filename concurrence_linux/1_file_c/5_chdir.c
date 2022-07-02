#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define SIZE 128
int main()
{
	// 获取当前进程的工作目录
	char buf[SIZE];
	memset(buf, 0, SIZE);
	if (getcwd(buf, SIZE) == NULL)
	{
		perror("getcwd");
		return 1;
	}	
	puts(buf);
	
	// 改变当前工作目录
	if (chdir("/usr/local") == -1)
	{
		perror("chdir");
		return 1;
	}

	// 获取当前进程的工作目录
	memset(buf, 0, SIZE);
	if (getcwd(buf, SIZE) == NULL)
	{
		perror("getcwd");
		return 1;
	}	
	puts(buf);
	



	return 0;

}

