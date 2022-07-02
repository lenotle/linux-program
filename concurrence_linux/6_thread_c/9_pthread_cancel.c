#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>



void *fun(void *arg)
{
	int i = 0;
	for (; i < 5; i++)
	{
		printf("doing work %d times\n", i);
		sleep(1);
	}
	pthread_exit(NULL);
	// return NULL;
}

// int pthread_cancel(pthread_t thread); 线程取消 == kill 
int main()
{
	pthread_t tid = 0;

	if (pthread_create(&tid, NULL, fun, NULL) != 0)
	{
		printf("pthread_create failed\n");
		return 1;
	}

	if (pthread_detach(tid) != 0)
	{
		printf("pthread_detach failed\n");
		return 1;
	}
	sleep(3);
	printf("after sleep three seconds, main thread cancel child thread...\n");

	pthread_cancel(tid);
	return 0;
}
