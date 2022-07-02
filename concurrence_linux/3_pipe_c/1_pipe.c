#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int main()
{
	int fds[2] = {0};
	int ret = -1;
	
	ret = pipe(fds);
	if (ret == -1)
	{
		perror("pipe");
		exit(1);
	}

	printf("fds[0] = %d, fds[1] == %d\n", fds[0], fds[1]);

	close(fds[0]);
	close(fds[1]);

	return 0;
}
