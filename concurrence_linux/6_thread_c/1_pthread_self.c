#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// pthread_t pthread_self(); 获取当前线程ID

int main()
{
	pthread_t tid = 0;
	
	tid = pthread_self();

	printf("tid: %lu\n", tid);	
	return 0;
}
