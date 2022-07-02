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

/*
 * 线程属性初始化及销毁 
 *
 * int pthread_attr_init(pthread_attr_t *attr);
 * int pthread_attr_destroy(pthread_attr_t *attr);
 * 
 * 设置线程分离状态 
 * int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate); 
 * int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
 *  detachstate：    分离状态
 *       PTHREAD_CREATE_DETACHED（分离线程）
 *       PTHREAD _CREATE_JOINABLE（非分离线程）
 *
*/ 
int main()
{
	pthread_t tid = 0;
	pthread_attr_t attr;

	// pthread_attr_init
	if (0 != pthread_attr_init(&attr))
	{
		printf("pthread_attr_init failed\n");
		return 1;
	}

	if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
	{
		printf("pthread_attr_setdetachstate failed");
		return 1;
	}

	if (pthread_create(&tid, &attr, fun, NULL) != 0)
	{
		printf("pthread_create failed\n");
		return 1;
	}

	if (pthread_join(tid, NULL) != 0)
	{
		printf("pthread is detached status\n");
	}

	printf("press any key to continue...\n");
	getchar();

	pthread_attr_destroy(&attr);
	return 0;
}
