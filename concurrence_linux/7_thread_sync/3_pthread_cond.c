#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 初始化
 * int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
 *
 * 销毁
 * int pthread_cond_destroy(pthread_cond_t *cond);
 *
 * 阻塞等待一个条件变量 
 * int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
 *    阻塞等待条件变量cond（参1）满足 
 *    释放已掌握的互斥锁（解锁互斥量）相当于pthread_mutex_unlock(&mutex);
 *    当被唤醒，pthread_cond_wait函数返回时，解除阻塞并重新申请获取互斥锁
 * 
 *  限时等待一个条件变量
 * int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex,
 *   const struct *restrict abstime);
 *
 * 唤醒至少一个线程
 * int pthread_cond_signal(pthread_cond_t *cond);
 * 
 * 唤醒全部wait线程
 * int pthread_cond_broadcast(pthread_cond_t *cond);
 *
 *
 */
pthread_mutex_t mutex;
pthread_cond_t condition;
int flag = 0;

// signal thread 2 to process 
void *fun1(void *arg)
{
	while(1)
	{
	
		pthread_mutex_lock(&mutex);
		flag = 1;
		printf("thread 1 update flag...\n");
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}	
}

void *fun2(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if (0 == flag)
		{
			pthread_cond_wait(&condition, &mutex);
		}
		printf("thread 2 is sigal....\n");
		flag = 0;
		pthread_mutex_unlock(&mutex);
	
	}
}


int main()
{
	pthread_t tid[2];
	int i = 0;
	
	// init 
	if (0 != pthread_mutex_init(&mutex, NULL))
	{
		printf("pthread_mutex_init faild...\n");
		goto err;
	}

	if (0 != pthread_cond_init(&condition, NULL))
	{
		printf("pthread_cond_init faild...\n");
		goto err;
	}

	memset(tid, 0, sizeof(tid));
	pthread_create(&tid[0], NULL, fun1, NULL);
	pthread_create(&tid[1], NULL, fun2, NULL);
	
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	// destory
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condition);
	return 0;
err:
	return 1;
}
