#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct node{
	int data;
	struct node *next;

}Node;

Node *head = NULL;

pthread_mutex_t mutex;
pthread_cond_t condition;

void *producer(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		Node *temp = malloc(sizeof(Node));		
		if (NULL == temp)
		{
			printf("malloc faild...\n");
			break;
		}
		memset(temp, 0, sizeof(Node));
		temp->data = random() % 100 + 1;

		temp->next = head;
		head = temp;

		printf("producer: %lu, %d\n", pthread_self(), temp->data);

		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&condition);

		sleep(1);
	}
	pthread_exit(NULL);
}

void *customer(void *arg)
{
	Node *temp = NULL;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if (head == NULL)
		{
			pthread_cond_wait(&condition, &mutex);
		}
		temp = head;
		head = head->next;

		printf("customer: %lu, %d\n", pthread_self(), temp->data);

		free(temp);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

// 生产者消费者类型 + 条件变量 + 互斥锁
int main()
{

	pthread_t p1, p2;

	srandom(getpid());

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

	// create thread 
	if (0 != pthread_create(&p1, NULL, producer, NULL))
	{
		printf("pthread_create faild...");
		goto err;
	}

	if (0 != pthread_create(&p2, NULL, customer, NULL))
	{
		printf("pthread_create failed...\n");
		goto err;
	}

	// wait thread end 
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	// destroy
	pthread_cond_destroy(&condition);
	pthread_mutex_destroy(&mutex);

	return 0;
err:
	return 1;
}
