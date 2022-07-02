#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "wrap.h"

typedef struct client_info
{
	int cfd;
	struct sockaddr_in addr;
}CINFO;

void *client_fun(void *arg);

// pthread server
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("arg < 2 <----a.out\n");
		exit(EXIT_FAILURE);
	}
	
	short port = atoi(argv[1]);
	
	// socket bind
	int fd = tcp4bind(port, NULL);
	Listen(fd, 128);

	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	CINFO *info;

	// set pthread detached 
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while(1)
	{
		int cfd = Accept(fd, (struct sockaddr *)&cliaddr, &len);

		pthread_t pid;

		info = malloc(sizeof(CINFO));
		if (info == NULL)
		{
			printf("malloc failed\n");
			continue;
		}

		info->cfd = cfd;
		info->addr = cliaddr;
		
		if (pthread_create(&pid, &attr, client_fun, info) != 0)
		{
			printf("pthread_create failed...\n");
			break;
		}
	
	}

	pthread_attr_destroy(&attr);
	close(fd);
	
	return 0;
}

void *client_fun(void *arg)
{
	CINFO *info = (CINFO *)arg;
	char buf[1024];
	int n = 0;
	char ip[16] = {'0'};

	printf("client->  port:%d, ip:%s\n", ntohs(info->addr.sin_port), inet_ntop(AF_INET, &(info->addr.sin_addr.s_addr), ip, 16));
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		n = read(info->cfd, buf, sizeof(buf));
		if (n <= 0)
		{
			if (n < 0)
				printf("read failed...\n");
			if (n == 0)
				printf("client closed...\n");
			break;
		}
		else 
		{
			printf("recv: %s\n", buf);
			write(info->cfd, buf, n);
		}
	
	}
	close(info->cfd);
	free(info);
	pthread_exit(NULL);
}
