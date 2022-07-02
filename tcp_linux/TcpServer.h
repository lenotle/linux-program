#pragma once
#include <iostream>
#include "TcpSocket.h"
using namespace std;

#define PORT 8888

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();

    // 设置服务端Socket
    int setListen(unsigned short port = PORT);
    // 监听客户端，提取客户端连接
    TcpSocket *clientAccept();
    void DisServerFd();

private:
    int lfd; // 服务端文件句柄

};