#ifndef __WRAP_H_
#define __WRAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

void perr_exit(const char *s);

// 创建Socket 连接 
int Socket(int family, int type, int protocol);

int Bind(int fd, const struct sockaddr *addr, socklen_t len);

// 绑定tcp4 
int tcp4bind(short port,const char *IP);

// 客户端连接服务器
int Connect(int fd, const struct sockaddr *sa, socklen_t salen);

// 监听
int Listen(int fd, int backlog);

// 接受客户端连接
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

// 一次性读取nbytes数据
ssize_t Read(int fd, void *ptr, size_t nbytes);

// 读取固定的字节数数据
ssize_t Readn(int fd, void *vptr, size_t n);

// 读取一行
ssize_t Readline(int fd, void *vptr, size_t maxlen);

ssize_t my_read(int fd, char *ptr);

// 一次性写nbytes 
ssize_t Write(int fd, const void *ptr, size_t nbytes);

// 写固定字节数
ssize_t Writen(int fd, const void *vptr, size_t n);

// 关闭连接
int Close(int fd);
#endif 
