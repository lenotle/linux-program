#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct node{
	int data;
	struct node *next;

}Node;

Node *head = NULL;

sem_t sem_pro, sem_cust;

void *producer(void *arg)
{
	while(1)
	{
		sem_wait(&sem_pro);
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
		sem_post(&sem_cust);

		sleep(1);
	}
	pthread_exit(NULL);
}

void *customer(void *arg)
{
	Node *temp = NULL;
	while(1)
	{
		sem_wait(&sem_cust);
		if (head == NULL)
		{
			printf("no product....\n");
			continue;
		}

		temp = head;
		head = head->next;

		printf("customer: %lu, %d\n", pthread_self(), temp->data);

		free(temp);
		sem_post(&sem_pro);
	}
	pthread_exit(NULL);
}

// 生产者消费者类型 + 条件变量 + 互斥锁
int main()
{

	pthread_t p[8];
	int i = 0;
	srandom(getpid());

	// init 
	if (0 != sem_init(&sem_pro, 0, 10))
	{
		printf("sem_init pro failed\n");
		goto err;
	}
	
	if (0 != sem_init(&sem_cust, 0, 0))
	{
		printf("sem_init pro failed\n");
		goto err;
	}

	// create thread 
	for (i = 0; i < 8; i++)
	{
		if (i < 5)
		{
			pthread_create(&p[i], NULL, producer, NULL);
		}
		else
		{
			
			pthread_create(&p[i], NULL, customer, NULL);
		}
	}
		

	// wait thread end 
	for (i = 0; i < 8; i++)
	{
		pthread_join(p[i], NULL);
	}

	// destroy
	sem_destroy(&sem_cust);
	sem_destroy(&sem_pro);
	return 0;
err:
	return 1;
}
