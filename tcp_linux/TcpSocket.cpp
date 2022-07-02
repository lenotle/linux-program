#include "TcpSocket.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(int connfd)
{
	this->connfd = connfd;
}

TcpSocket::~TcpSocket()
{
}

/**
 * @brief 连接服务器
 * 
 * @param ip ip地址
 * @param port  端口
 * @param timeout  等待时间
 * @return int 错误码，成功连接文件句柄被设置
 */
int TcpSocket::connectServer(string ip, unsigned short port, int timeout)
{
	int ret = -1;
    struct sockaddr_in cliaddr;

	if (port < 0 || port < 1024 || port > 65536 || timeout < 0)
	{
		ret = ParamError;
		return ret;
	}

	/* int socket(int domain, int type, int protocol); */
	this->connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->connfd < 0)
	{
		ret = errno;
        printf("%s -->socket() err: %d\n", __FUNCTION__, ret);
        return ret;
	}

    cliaddr.sin_port = htons(port);
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = inet_addr(ip.data());
    // 建立连接
    int ret = connectTimeout(&cliaddr, timeout);
    // 建立连接失败
    if (ret < 0)
    {
        if (ret == -1 && errno == ETIMEDOUT)
        {
            ret = TimeoutError;
            return ret;
        }
        else 
        {
            printf("%s -->connectTimeout() err: %d\n", __FUNCTION__, errno);
            return errno;
        }
    }
	return ret;
}

/**
 * @brief connect 阻塞函数
 * 
 * @param addr 客户端地址
 * @param wait_seconds  等待时间
 * @return int 成功返回0，失败返回-1
 */
int TcpSocket::connectTimeout(sockaddr_in* addr, unsigned int wait_seconds)
{
    int ret = -1;
    socklen_t socklen = sizeof(addr);

    // 1. 将文件句柄设置好为非阻塞
    if (wait_seconds > 0)
    {
        this->setNonBlock(this->connfd);
    }

    // 2. 建立连接
    /* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);*/
    ret = connect(this->connfd, (struct sockaddr *)addr, socklen);
    // 3. 判断连接是否成功, 非阻塞模式连接, 返回-1, 并且errno为EINPROGRESS, 表示连接正在进行中
    if (ret < 0 && errno = EINPROGRESS)
    {
        fd_set writeSet;
        struct timeval timeout;

        FD_ZERO(&writeSet);
        FD_SET(this->connfd, &writeSet);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        // 利用select 监听文件描述符写缓冲区
        do
        {
            ret = select(this->connfd + 1, NULL, &writeSet, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        
        if (ret < 0)
        {
            // 错误
            return -1;
        }
        else if (ret == 0)
        {
            // 超时
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            /* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
            /*int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);*/
            int err = 0;
            socklen_t len = sizeof(err);
            int sockret = getsockopt(this->connfd, SOL_SOCKET, SO_ERROR, &err, &len);
            
            if (sockret -1)
            {
                return -1;
            }

            if (err == 0)
            {
                return 0; // 建立连接成功
            }
            else 
            {
                errno = err;
                return -1;
            }
        }
    }
    // 4. 将文件句柄修改为阻塞
    if (wait_seconds > 0)
    {
        this->setBlock(this->connfd);
    }
	return ret;
}

void TcpSocket::disConnect()
{
    if (this->connfd != 0)
    {
        close(this->connfd);
    }
}

int TcpSocket::sendMessage(string sendData, int timeout)
{
    int ret = this->writeTimeout(timeout);
    if (ret == 0)
    {
        int writen = 0;
        int len = sendData.size() + 4; // 解决TCP通信头部粘包问题
        unsigned char *data = (unsigned char *)malloc(sizeof(len));
        if (data == NULL)
        {
            ret = MallocError;
            printf("%s -->sendMessage:malloc err: %d\n", __FUNCTION__, ret);
            return ret;
        }

        // 转换为网络字节序
        int netlen = htonl(len);
        memcpy(data, netlen, 4);
        memcpy(data + 4, sendData, sendData.size());

        // 发送数据
        writen = this->writen(data, len);
        if (writen < len)
        {
            if (data != NULL)
            {
                free(data);
                data = NULL;
            }
            return writen;
        }
        if (data != NULL)
        {
             free(data);
             data = NULL;
        }
    }
    else 
    {
        //失败返回-1，超时返回-1并且errno = ETIMEDOUT
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			printf("%s sckClient_send() mlloc Err: %d\n ", __FUNCTION__, ret);
		}
    }
	return ret;
}

int TcpSocket::recvMessage(int timeout)
{
    // 返回0 -> 没超时就接收到了数据, -1, 超时或有异常
	int ret = readTimeout(timeout); 
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			printf("readTimeout(timeout) err: TimeoutError \n");
			return string();
		}
		else
		{
			printf("readTimeout(timeout) err: %d \n", ret);
			return string();
		}
	}

	int netdatalen = 0;
	ret = readn(&netdatalen, 4); //读包头 4个字节
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return string();
	}
	else if (ret < 4)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}

	int n = ntohl(netdatalen);
	// 根据包头中记录的数据大小申请内存, 接收数据
	char* tmpBuf = (char *)malloc(n + 1);
	if (tmpBuf == NULL)
	{
		ret = MallocError;
		printf("malloc() err \n");
		return NULL;
	}

	ret = readn(tmpBuf, n); //根据长度读数据
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return string();
	}
	else if (ret < n)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}

	tmpBuf[n] = '\0'; //多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
	string data = string(tmpBuf);
	// 释放内存
	free(tmpBuf);

	return data;
}

/**
 * @brief 设置文件描述符为非阻塞
 * 
 * @param fd 
 * @return int 失败返回-1
 */
int TcpSocket::setNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        return flags;
    }
    flags |= O_NONBLOCK;
	return fcntl(fd, F_SETFL, flags);
}

/**
 * @brief 设置文件描述符为阻塞
 * 
 * @param fd 
 * @return int 失败返回-1
 */
int TcpSocket::setBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        return flags;
    }
    flags &= ~O_NONBLOCK;
	return fcntl(fd, F_SETFL, flags);
}

int TcpSocket::readTimeout(unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set readSet;
        struct timeval timeout = {wait_seconds, 0};
        FD_ZERO(&readSet);
        FD_SET(this->connfd, &readSet);

        ret = select(this->connfd +1, &readSet, NULL, NULL, &timeout);
        if (ret == 0)
        {
            // 超时
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            return 0;
        }
    }
	return ret;
}

/**
 * @brief 写操作超时检测，利用select监听文件句柄写缓冲区
 * 
 * @param wait_seconds 
 * @return int 成功：0， 失败：-1
 */
int TcpSocket::writeTimeout(unsigned int wait_seconds)
{
    int ret = 0;
    if (wait_seconds > 0)
    {
        fd_set writeSet;
        struct timeval timeout = {wait_seconds, 0};
        FD_ZERO(&writeSet);
        FD_SET(this->connfd, &writeSet);

        ret = select(this->connfd +1, NULL, &writeSet, NULL, &timeout);
        if (ret == 0)
        {
            // 超时
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            return 0;
        }
    }
	return ret;
}

/**
 * @brief 读取固定长度的数据
 * 
 * @param buf 读取数据缓冲区
 * @param count  读取数据长度
 * @return int 成功返回读取数据字节数，失败返回-1
 */
int TcpSocket::readn(void* buf, int count)
{
    int nleft = count;
    char *bufp = (char *)buf;
    ssize_t nread = 0;

    while(nleft > 0)
    {
        if ((nread = recv(this->connfd, bufp, nleft, 0)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nread == 0)
        {
            return count - nleft;
        }
        bufp += nread;
        nleft -= nread;
    }
	return count;
}

/**
 * @brief 发送固定长度的数据
 * 
 * @param buf 发送数据缓冲区
 * @param count  发送数据长度
 * @return int 成功返回发送数据字节数，失败返回-1
 */
int TcpSocket::writen(const void* buf, int count)
{
    size_t nleft = count;
	ssize_t nwritten;
	char *bufp = (char*)buf;

    while (nleft > 0)
    {
        if ((nwritten = send(this->connfd, bufp, nleft, 0)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nwritten == 0)
        {
            continue;
        }
        bufp += nwritten;
        nleft -= nwritten;
    }
	return nwritten;
}
