#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd = -1;
	
	string ah = "jack";

	fd = open("1.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}
	printf("fd = %d\n", fd);
	char buf[1024] = {'0'};

	if (read(fd, buf, 1024) == -1)
	{
		perror("write file");
		return 1;
	}

	printf("content is :%s\n", buf);

	close(fd);

	return 0;
}
