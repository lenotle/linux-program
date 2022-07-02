#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 线程创建 
// int pthread_create(pthread_t *thread,               线程创建成功传出的线程号 
// 		      const pthread_attr_t *attr,      线程属性结构体地址，通常设置为 NULL。
//                    void *(*start_routine) (void *), 回调函数
//                    void *arg 	               回调函数参数
//                    );

void *fun(void *arg)
{
	printf("child thread run, tid: %lu\n", pthread_self());
	
}

void *fun1(void *arg)
{
	// int res = (int)arg // error
	// int res = *((int *)arg); // 段错误 
	
	int res = (int)(long)arg; // x86架构一个指针 8字节，因此先转为long占8字节，再转为int, 不会出现字节丢失
	printf("res: %d, tid: %d\n", res, pthread_self());
}


int main()
{
	pthread_t tid = 0;
	int ret = -1;

	ret = pthread_create(&tid, NULL, fun, NULL);
	if (0 != ret)
	{
		printf("pthread_create fail");
		return 1;
	
	}

	ret = pthread_create(&tid, NULL, fun1, (void *)0x3);
	if (0 != ret)
	{
		printf("pthread_create fail");
		return 1;
	}


	printf("main thread run, tid: %lu\n", pthread_self());
	printf("preass any key to continue...\n");
	getchar();
	return 0;
}
