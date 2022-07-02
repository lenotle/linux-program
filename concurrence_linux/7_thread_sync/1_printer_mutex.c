#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*
 * 初始化--销毁
 * 第二个变量一般为空，成功返回0
 * int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
 * int pthread_mutex_destroy(pthread_mutex_t *mutex);
 * 
 * 加锁 succ 0
 * int pthread_mutex_lock(pthread_mutex_t *mutex);
 * int pthread_mutex_trylock(pthread_mutex_t *mutex);尝试加锁，若没加锁，则加锁，若已经加锁，则失败 succ 0
 *
 *  解锁
 *  int pthread_mutex_unlock(pthread_mutex_t *mutex);
 */

pthread_mutex_t mutex;

void printer(char *str)
{
	while(*str != '\0')
	{
		putchar(*str);
		fflush(stdout);
		++str;
		usleep(100000);
	}
	putchar('\n');
}

void *fun1(void *arg)
{
	char *str = "hello";
	
	pthread_mutex_lock(&mutex);
	printer(str);
	pthread_mutex_unlock(&mutex);
}

void *fun2(void *arg)
{
	char *str = "world";
	
	pthread_mutex_lock(&mutex);
	printer(str);
	pthread_mutex_unlock(&mutex);
}

// 互斥锁---打印机问题（打印大小写字母交叉出现）
int main()
{
	pthread_t tid1, tid2;
	
	// mutex init 
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&tid1, NULL, fun1, NULL);
	pthread_create(&tid2, NULL, fun2, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	printf("main thread exit....");

	pthread_mutex_destroy(&mutex);
	return 0;
}
