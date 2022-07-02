#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd = -1;
	fd = open("1.txt", O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}
	printf("fd = %d\n", fd);

	char *str = "hello jack";
	if (write(fd, str, strlen(str)) == -1)
	{
		perror("write file");
		return 1;
	}

	close(fd);
	return 0;
}
