#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <arpa/inet.h>
#include "wrap.h"

#define MAXLINE 128 
#define OPEN_MAX 1024 

int main(int argc, char *argv[])
{
	int i, n, maxi, listenfd, connfd, sockfd;
	int nready;                                     /* poll 返回监听个数*/
	struct sockaddr_in cliaddr;
	socklen_t clilen;

	char str[INET_ADDRSTRLEN], buf[MAXLINE];
	struct pollfd client[OPEN_MAX];

	listenfd = tcp4bind(8080, NULL);
	Listen(listenfd, 128);

	client[0].fd = listenfd;                            /* 要监听的第一个事件*/
	client[0].events = POLLIN;

	for (i = 1; i < OPEN_MAX; i++)
	{
		client[i].fd = -1;                         /* 初始化client集合， -1 代表不监听*/
	}

	maxi = 0;                                         /* client数组有效元素中最大元素下标*/

	for( ; ; )
	{
		nready = poll(client, maxi + 1, -1);      // 阻塞监听是否有客户端连接请求

		if (client[0].revents & POLLIN)           // listenfd有读事件就绪
		{
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);      // 接受客户端请求
			printf("received from %s at PORT %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
					ntohs(cliaddr.sin_port));

			for (i = 1; i < OPEN_MAX; i++)
			{
				if (client[i].fd < 0)
				{
					client[i].fd = connfd;                // 找到client[]中空闲的位置，存储accept返回值
					client[i].events = POLLIN;            // 设置刚才监听的事件为读事件
					break;
				}
			}

			if (i == OPEN_MAX)                     // 客户端段连接数超过了最大连接数
			{
				perr_exit("too many clients");
			}

			
			if (i > maxi)                         // 更新client数据最大有效值
			{
				maxi = i;
			}

			if (--nready <= 0)
			{
				continue;
			}
		}

		for (i = 1; i <= maxi; i++)
		{
			if ((sockfd = client[i].fd) < 0)
			{
				continue;
			}

			if (client[i].revents & POLLIN)
			{
				memset(buf, 0, sizeof(buf));
				if ((n = read(sockfd, buf, MAXLINE)) < 0)
				{
					if (errno == ECONNRESET) 
					{
						// 收到RST标志
						printf("client[%d] aborted connection\n", i);
						Close(sockfd);
						client[i].fd = -1;
					}
					else
					{
						perr_exit("read error");
					}	
				}
				else if (n == 0)
				{
					printf("client[%d] closed connection\n", i);
					Close(sockfd);
					client[i].fd = -1;
				}
				else 
				{
					write(STDOUT_FILENO, buf, n);
					write(sockfd, buf, n);
				}

				if (--nready <= 0)
				{
					break;
				}
			}
		}
	}	
	Close(listenfd);
	return 0;
}
