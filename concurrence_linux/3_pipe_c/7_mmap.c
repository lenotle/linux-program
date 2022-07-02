#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>

// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
// int munmap(void *addr, size_t length);


int main()
{
	int fd = -1;
	void *addr = NULL;

	fd = open("test", O_RDWR);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}
	
	addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}

	close(fd);
	
	printf("开始写文件..\n");	
	// write file 
	const char *str  = "hesdfsdfdsfdfdfjojsafsdfdsf";
	memcpy(addr, str, strlen(str));

	// 断开存储映射
 	munmap(addr, 1024);

	return 0;
}
