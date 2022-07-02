#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int num = 0;
pthread_rwlock_t rw;


void *lock_read(void *arg)
{
	int index = (int)(long)arg;
	int i = 0;
	while(1)
	{
		// lock
		pthread_rwlock_rdlock(&rw);
		printf("thread[%d] read data:   %d\n", index, num);
		// unlock
		pthread_rwlock_unlock(&rw);

		sleep(random() % 2 + 1);
	}

}

void *lock_write(void *arg)
{
	int index = (int)(long)arg;
	while(1)
	{
		pthread_rwlock_wrlock(&rw);
		num++;
		printf("thread[%d] update data: %d\n", index, num);
		pthread_rwlock_unlock(&rw);
		sleep(random() % 2 + 1);
	}

}
// 读写锁
int main()
{

	int i;
	pthread_t tid[8];

	// init rw lock 
	pthread_rwlock_init(&rw, NULL);
	
	for (i = 0; i < 8; i++)
	{
		if (i < 5)
		{
			// read lock 
			pthread_create(&tid[i], NULL, lock_read, (void*)(long)i);
		}
		else
		{
			// write lock 
			pthread_create(&tid[i], NULL, lock_write, (void*)(long)i);
		}
	}

	// wait thread exit 
	for (i = 0; i < 8; i++)
	{
		pthread_join(tid[i], NULL);
	}
	
	// destroy rw lock 
	pthread_rwlock_destroy(&rw);
	return 0;
}
