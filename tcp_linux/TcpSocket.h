#pragma once
#include <iostream>
using namespace std;

static const int TIMEOUT = 10000; // 默认超时时间

class TcpSocket
{
public:
	enum TypeError
	{
		ParamError = 3001, 
		TimeoutError, 
		PeerCloseError, 
		MallocError
	};
	TcpSocket();
	TcpSocket(int connfd);
	~TcpSocket();

	// 连接服务器
	int connectServer(string ip, unsigned short port, int timeout = TIMEOUT);
	// 释放连接
	void disConnect();
	// 发送消息
	int sendMessage(string sendData, int timeout = TIMEOUT);
	// 接受消息
	int recvMessage(int timeout = TIMEOUT);

private:
	// 设置I/O为非阻塞模式
	int setNonBlock(int fd);
	// 设置I/O为阻塞模式
	int setBlock(int fd);
	// 读超时检测函数，不含读操作
	int readTimeout(unsigned int wait_seconds);
	// 写超时检测函数, 不包含写操作
	int writeTimeout(unsigned int wait_seconds);
	// 带连接超时的connect函数
	int connectTimeout(struct sockaddr_in* addr, unsigned int wait_seconds);
	// 每次从缓冲区中读取n个字符
	int readn(void* buf, int count);
	// 每次往缓冲区写入n个字符
	int writen(const void* buf, int count);

private:
	int connfd;  //  客户端文件句柄
};