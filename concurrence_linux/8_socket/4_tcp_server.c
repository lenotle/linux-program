#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
 * int bind(int socket, const struct sockaddr *address, socklen_t address_len);
 *
 * int listen(int socket, int backlog);
 * 	backlog: 已完成连接队列及未完成连接的和的最大值， 一般取128
 *
 * int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
 * 	address: 返回的客户端IP信息
 * 	adderss_len: 返回的客户端信息长度
 */ 

// socket 服务端

#define SIZE 1024

int main()
{
	int fd = -1;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	socklen_t len;
	int newfd = -1;
	char buf[SIZE];

	// 1. 创建 socket
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == fd)
	{
		perror("socket");
		goto err;
	}
	// 2. 绑定
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	inet_pton(AF_INET, "10.0.16.10", &addr.sin_addr.s_addr);

	if (-1 == bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("bind");
		goto err;
	}
	// 3. 监听
	if (-1 == listen(fd, 128))
	{
		perror("listen");
		goto err;
	}
	// 4. accept
	while(1)
	{
		newfd = accept(fd, (struct sockaddr *)&client_addr, &len);
		if (-1 == newfd)
		{
			printf("accept failed");
			break;
		}
		while(1)
		{

		        memset(buf, 0, SIZE);
			int n = read(newfd, buf, SIZE);
			if (n <= 0)
			{
				printf("connect closed\n");
				break;
			}
			write(STDOUT_FILENO, buf, sizeof(buf));
			n = read(STDIN_FILENO, buf, SIZE);
			write(newfd, buf, n);
		}
		close(newfd);
	}

	close(fd);
	return 0;
err:
	if (-1 == fd)
	{
		close(fd);
	}
	exit(EXIT_FAILURE);
}

