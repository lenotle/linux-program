#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

#define SIZE 128

// 写管道
int main()
{
	int ret = -1;
	int fd = -1;
	char buf[SIZE];
	int i = 0;

	fd = open("fifo", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}

	while(1)
	{
		memset(buf, 0, sizeof(buf));
		ret = read(fd, buf, sizeof(buf));
		if (ret <= 0)
		{
			perror("read");
			break;
		}
		printf("read data: %s\n", buf);	
	}

	close(fd);
	return 0;
}
