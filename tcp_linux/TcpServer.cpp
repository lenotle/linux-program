#include "TcpServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

TcpServer::TcpServer()
{

}
TcpServer::~TcpServer()
{

}

// 设置服务端Socket
int TcpServer::setListen(unsigned short port = PORT)
{
    int ret;
    struct sockaddr_in servaddr;
    int on = 1;

    // 1. 创建socket 
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd != 0)
    {
        ret = errno;
        printf("%s socket() err: %d\n", ret);
        return ret;
    }
   
	// 2. 设置端口复用
	ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

	// 3. 监听的套接字绑定本地IP和端口
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(m_lfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

    // 4. 监听
    ret = listen(m_lfd, 128);
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

	return ret;

}

// 监听客户端，提取客户端连接
TcpSocket *TcpServer::clientAccept()
{
    int ret;
	if (wait_seconds > 0)
	{
		fd_set accept_fdset;
		struct timeval timeout;
		FD_ZERO(&accept_fdset);
		FD_SET(lfd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			// 检测读集合
			ret = select(lfd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);	// 被信号中断, 再次进入循环
		if (ret <= 0)
		{
			return NULL;
		}
	}

	// 一但检测出 有select事件发生，表示对等方完成了三次握手，客户端有新连接建立
	// 此时再调用accept将不会堵塞
	struct sockaddr_in addrCli;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int connfd = accept(lfd, (struct sockaddr*)&addrCli, &addrlen); //返回已连接套接字
	if (connfd == -1)
	{
		return NULL;
	}

	return new TcpSocket(connfd);
}

void TcpServer::DisServerFd()
{
    if (this->lfd != 0)
    {
        close(this->lfd)
    }
}