#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int fd = -1;
	fd = open("1.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}

	printf("fd=%d\n", fd);
	close(fd);
	return 0;


}

