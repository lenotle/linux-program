#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SIZE 1024
/*
 * 创建socket 
 * int socket(int domain, int type, int protocol);
 * 	domain: 协议
 * 		AF_INET: IPV4
 * 		AF_INET6
 * 		AF_INET, AF_LOCAL: 客户端和服务器都在同一机器上使用
 * 	
 * 	type: 类型
 * 		SOCK_STREAM: 按照顺序的、可靠的、数据完整的基于字节流的连接。这个socket是使用TCP来进行传输。
 * 		SOCK_DGRAM: 无连接的、固定长度的传输调用。该协议是不可靠的，使用UDP来进行它的连接
 * 		SOCK_SEQPACKET: 双线路的、可靠的连接，发送固定长度的数据包进行传输。必须把这个包完整的接受才能进行读取
 * 		SOCK_RAW: 供单一的网络访问，这个socket类型使用ICMP公共协议, ping traceroute 使用该协议 
 *      
 *      protocol: 传0表示采用默认协议，一般为0
 *
 *      reutrn value: 成功返回文件描述符，失败返回-1
 *
 *
 * 建立连接
 * int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 * 	addr: IPV4结构体强转
 * 	return value： 成功返回0
 *
 */ 
// 客户端
int main()
{
	int socket_fd = -1;
	// IPV4 
	struct sockaddr_in addr;
	char buf[SIZE];

	// 1. 创建Socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_fd)
	{
		printf("socket failed\n");
		goto err;
	}

	// 2. 建立连接
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	inet_pton(AF_INET, "10.0.16.10", &addr.sin_addr.s_addr);

	if (-1 == connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)))
	{
		perror("connect failed");
		goto err;
	}

	// 3. 通信
	memset(buf, 0, SIZE);
	while(1)
	{
		read(STDIN_FILENO, buf, SIZE);
		write(socket_fd, buf, sizeof(buf));
	        putchar('\n');	
		
		int n = read(socket_fd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, n);
		putchar('\n');

	}

	// 4. close 
	close(socket_fd);
	return 0;
err:
	if (-1 != socket_fd)
	{
		close(socket_fd);
	}
	exit(EXIT_FAILURE);
}
