#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd = -1;
	char buf[1024];
	int ret = -1;

	fd = open("1.txt", O_RDWR  | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}

	write(fd, "abcd", 4);
	
	ret = lseek(fd, 32, SEEK_SET);

	printf("seek: %d\n", ret);
	write(fd, "jack", 4);

	// read 
	lseek(fd, 0, SEEK_SET);
	memset(buf,0, 1024);
	ret = read(fd, buf, 1024);
	printf("read: %d\n", ret);
	puts(buf);
	close(fd);
	return 0;
}
