#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>

#define SIZE 128

int main()
{
	int fd = -1;
	pid_t pid = -1;
	char buf[SIZE];
	int status = -1;

	if ((fd = open("1.txt", O_RDWR |O_CREAT, 0644)) == -1)
	{
		perror("open");
		return 1;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	if (pid == 0)
	{
		printf("child proccss write data...\n");
		
		if (write(fd, "abcdefg", 7) == -1)
		{
			perror("write");
			exit(1);
		}
		close(fd);
		exit(1);
	}

	if (wait(&status) == -1)
	{
		perror("wait");
		return 1;
	}

	printf("parent process read file...\n");
	lseek(fd, 0, SEEK_SET);
	memset(buf, 0, SIZE);
	
	read(fd, buf, SIZE);
	printf("%s\n", buf);

	close(fd);
	return 0;
}

