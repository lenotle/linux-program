#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// int pthread_equal(pthread_t t1, pthread_t t2); 比较两个线程号是否相等

int main()
{
	pthread_t tid = 0;
	
	tid = pthread_self();

	printf("tid: %lu\n", tid);	

	if (pthread_equal(tid, pthread_self()))
	{
		printf("equal\n");
	}
	else 
	{
		printf("not equal\n");

	}
	return 0;
}
