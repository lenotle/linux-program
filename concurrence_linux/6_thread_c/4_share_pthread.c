#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int num = 100;

void* fun(void *arg)
{
	int* n = (int *)arg;
       	printf("before num = %d, *p = %d\n", num, *n);
	num++;
	(*n)++;
	printf("after num = %d, *p = %d\n", num , *n);
	return NULL;	

}


// 共享资源，代码段 数据段 堆 文件描述符...
int main()
{
	pthread_t tid = 0;
	int ret = -1;
	int *p = NULL;
	
	p = malloc(sizeof(int));
	if (NULL == p)
	{
		printf("malloc failed....\n");
		return 1;
	}

	*p = 90;

	ret = pthread_create(&tid, NULL, fun, (void*)p);
	if (0 != ret)
	{
		printf("pthread_create fail...\n");
	}

	printf("press any key to continue...\n");
	getchar();
	
	num--;
	(*p)--;
	printf("main num = %d, *p = %d\n", num, *p);
	
	free(p);
	return 0;
}
