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
	return NULL;
}

// test thread detach 
// int pthread_detach(pthread_t tid) 0 succ non-0 fail
// 线程分离后，资源由系统内核回收
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

	// 测试分离线程是否可以被join
	if (pthread_join(tid, NULL) != 0)
	{
		printf("pthread_join faild\n");
	}

	printf("press any key to continue....\n");
	getchar();
	return 0;
}
