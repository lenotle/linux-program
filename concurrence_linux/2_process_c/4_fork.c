#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	int *p = NULL;
	int num = 88;
	pid_t pid = -1;

	p = (int *)malloc(sizeof(int));
	if (p == NULL)
	{
		perror("malloc");
		return 1;
	}
	memset(p, 0, sizeof(int));
	
	pid = fork();
	if (pid == -1 )
	{
		perror("fork");
		return 1;
	}

	if (pid == 0)
	{
		sleep(1);
		printf("child proccess before: num  = %d, *P = %d\n", num, *p);
		free(p);
		p = NULL;

	}
	else 
	{
		printf("parent proccess before: num  = %d, *P = %d\n", num, *p);
		num++;
		(*p)++;
		printf("parent proccess after: num  = %d, *P = %d\n", num, *p);
		free(p);
		p = NULL;

	}
	return 0;
}

