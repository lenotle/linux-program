#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void *fun1(void *arg)
{
	int i = 0;
	for (i = 'A'; i <= 'Z'; i++)
	{
		putchar(i);
		fflush(stdout);

		usleep(100000); // 100ms
	}

	return NULL;
}

void *fun2(void *arg)
{
	int i = 0;
	for (i = 'a'; i <= 'z'; i++)
	{
		putchar(i);
		fflush(stdout);
		usleep(100000); // 100ms
	}

	return NULL;
}


// 模拟打印机 
int main()
{
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, fun1, NULL);
	pthread_create(&tid2, NULL, fun2, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	putchar('\n');
	return 0;
}
