#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/wait.h>
// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// int munmap(void *addr, size_t length);

// 匿名存储映射实现父子进程通信
int main()
{
	pid_t pid = -1;
	void *addr = NULL;

	addr = mmap(NULL, 1024 * 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}

	pid = fork();
	if (-1 == pid)
	{
		perror("fork");
		munmap(addr, 4096);
		return 1;
	}

	if (0 == pid)
	{
		const char *s = "int munmap void addr size_t length";
		memcpy(addr, s, strlen(s));
	}
	else 
	{
		wait(NULL);
		printf("parent process read data: %s\n", addr);
	}

	munmap(addr, 4096);


	return 0;
}
