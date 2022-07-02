#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/types.h>
#include <sys/stat.h>

// 创建有名管道 
// int mkfifo(const char *pathname, mode_t mode);

int main()
{
	int ret = -1;
	ret = mkfifo("fifo", 0644);
	if (ret == -1)
	{
		perror("mkfifo");
		return 1;
	}	
	printf("fifo succ..\n");

	return 0;
}
