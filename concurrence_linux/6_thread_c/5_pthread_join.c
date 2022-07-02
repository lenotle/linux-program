#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// int pthread_join(pthread_t tid, 被等待的线程号
// 		    void **retval，用来存储线程推出的状态的指针
// 		    );

void* fun(void *arg)
{
	int i = 0;
	for (;i < 5; i++)
	{
		printf("child thread doing work %d times\n", i);
		sleep(1);
	}

	return (void*)0x3;
}

// 等待线程结束 类似于wait 会阻塞
int main()
{
	pthread_t tid = 0;
	void *retval = NULL;

	if (pthread_create(&tid, NULL, fun, NULL) != 0)
	{
		printf("pthread_create failed....\n");
		goto err;
	}

	printf("main thread runing....\n");

	if (pthread_join(tid, &retval) != 0)
	{
		printf("pthread_join failed....\n");
		goto err;
	}

	printf("main thread end, retval = %p\n", retval);
	return 0;
err:
	return 1;
	
}
