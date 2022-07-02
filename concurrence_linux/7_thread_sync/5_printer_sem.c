#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
 *初始化
 *      int sem_init(sem_t *sem, int pshared, unsigned int value);
 *               创建一个信号量并初始化它的值。一个无名信号量在被使用前必须先初始化。
 *                   sem：信号量的地址。
 *                   pshared：等于 0，信号量在线程间共享（常用）；不等于0，信号量在进程间共享。
 *                   value：信号量的初始值。
 * 销毁
 *      int sem_destroy(sem_t *sem);
 *
 *
 * 减一 
 *       int sem_wait(sem_t *sem); sem 等于0会阻塞 相当于加锁
 *      
 *       int sem_trywait(sem_t *sem);   以非阻塞的方式来对信号量进行减 1 操作。
 *        
 *       int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout); 
 *
 * 加一
 *       int sem_post(sem_t *sem); 将信号量的值加 1 并发出信号唤醒等待线程      
 */

sem_t sem;

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
	
	sem_wait(&sem);
	printer(str);
	sem_post(&sem);
}

void *fun2(void *arg)
{
	char *str = "world";
	
	sem_wait(&sem);
	printer(str);
	sem_post(&sem);
}

int main()
{
	pthread_t tid1, tid2;
	
	// sem init
	if (0 != sem_init(&sem, 0, 1))
	{
		printf("sem_init faild\n");
		return 1;
	}	

	pthread_create(&tid1, NULL, fun1, NULL);
	pthread_create(&tid2, NULL, fun2, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	printf("main thread exit....");
	
	// destroy 
	sem_destroy(&sem);
	return 0;
}
