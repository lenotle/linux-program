#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "wrap.h"

#define _BUF_LEN_  1024                     // 读写缓冲区
#define _EVENT_SIZE_ 1024                   // 事件驱动结构体数组最大数量

// 事件驱动结构体
typedef struct _events
{
	int fd;
	int events;
	void *arg;
	void (*callback)(int fd, int events, void *arg);
	char buf[_BUF_LEN_];
	int buflen;
	int epfd;
}xevents;

xevents events_g[_EVENT_SIZE_ + 1];

int epfd_g = 0;  // epoll根节点

int  initsocket();
void initAccept(int fd,int events,void *arg);
void readData(int fd,int events,void *arg);
void sendData(int fd,int events,void *arg);
void eventdel(xevents *ev,int fd,int events);
void eventadd(int fd,int events,void (*callback)(int ,int ,void *),void *arg,xevents *ev);
void eventset(int fd,int events,void (*callback)(int ,int ,void *),void *arg,xevents *ev);

// 初始化套接字 
int initsocket()
{
	// 创建socket 
	int lfd = Socket(AF_INET, SOCK_STREAM, 0); 

	//端口复用
        int opt = 1;
        setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	// bind 
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(lfd, (struct sockaddr *)&addr, sizeof(addr));

	// 监听
	Listen(lfd, 128);
	
	return lfd;
}

void sendData(int fd, int events, void *arg)
{
	printf("begin call %s, fd = %d\n", __FUNCTION__, fd);
	xevents *ev = (xevents *)arg;

	int n = send(fd, ev->buf, ev->buflen, 0);
	if (n <= 0)
	{
		perror("send");
		Close(fd);
		eventdel(ev, fd, events);
		return;
	}
	//events = EPOLLIN | EPOLLET;
	eventset(fd, EPOLLIN, readData, arg, ev);
}

void readData(int fd, int events, void *arg)
{
	printf("begin call %s, fd = %d\n", __FUNCTION__, fd);
	xevents *ev = (xevents *)arg;

	ev->buflen = recv(fd, ev->buf, sizeof(ev->buf), 0);		
	ev->buf[ev->buflen] = '\0';

	if (ev->buflen <= 0)
	{
		perror("recv");
		Close(fd);
		eventdel(ev, fd, events);
		return;
	}
	eventset(fd, EPOLLOUT, sendData, arg, ev);
}

// 新连接处理
void initAccept(int fd, int events, void *arg)
{
	printf("begin call %s, epfd_g = %d, fd = %d\n", __FUNCTION__, epfd_g, fd);

	int i;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	int cfd = Accept(fd, (struct sockaddr *)&addr, &len); 

	// cfd设置为非阻塞
	//int flags = fcntl(cfd, F_GETFL);
	//flags |= O_NONBLOCK;
	//fcntl(fd, F_SETFL, flags); 

	// 查找events_g数组中可用位置
	for (i = 0; i < _EVENT_SIZE_; i++)
	{
		if (events_g[i].fd == 0)
		{
			break;
		}
	}

	//events |= EPOLLET; // 边沿触发

	// 设置读事件
	eventadd(cfd, events, readData, &events_g[i], &events_g[i]);
}

void eventadd(int fd, int events, void (*callback)(int, int, void *), void *arg, xevents *ev)
{
	ev->fd = fd;
	ev->events = events;
	// ev->arg = arg;
	ev->callback = callback;

	struct epoll_event epv;
	epv.events = events;
	epv.data.ptr = ev;    // 反应堆重点 

	epoll_ctl(epfd_g, EPOLL_CTL_ADD, fd, &epv);
}

//修改事件
//eventset(fd,EPOLLOUT,senddata,arg,ev);
void eventset(int fd,int events,void (*callback)(int ,int ,void *),void *arg,xevents *ev)
{
	ev->fd = fd;
	ev->events = events;
	//ev->arg = arg;
	ev->callback = callback;

	struct epoll_event epv;
	epv.events = events;
	epv.data.ptr = ev;
	epoll_ctl(epfd_g, EPOLL_CTL_MOD, fd, &epv);//修改
}


// 删除事件
void eventdel(xevents *ev, int fd, int events)
{
	printf("begn call %s, fd = %d\n", __FUNCTION__, fd);

	ev->fd = 0;
	ev->events = 0;
	ev->callback = NULL;
	// ev->arg = NULL;
	memset(ev->buf, 0, sizeof(ev->buf));

	struct epoll_event epv;
	epv.data.ptr = NULL;
	epv.events = events;

	epoll_ctl(epfd_g, EPOLL_CTL_DEL, fd, &epv);
}

int main(int argc, char *argv[])
{
	// 初始化socket 
	int lfd = initsocket();

	// 创建epoll根节点 
	epfd_g = epoll_create(1);
	printf("epfd_g = %d\n", epfd_g);

	struct epoll_event events[_EVENT_SIZE_];

	// 添加最初事件，将lfd 上树
	eventadd(lfd, EPOLLIN, initAccept, &events_g[_EVENT_SIZE_], &events_g[_EVENT_SIZE_]); 

	while (1)
	{
		int nready = epoll_wait(epfd_g, events, _EVENT_SIZE_, -1);
		if (nready < 0 )
		{
			perr_exit("epoll_wait failed");
		}
		else if (nready > 0)
		{
			int i = 0;
			for (i = 0; i < nready; i++)
			{
				xevents *xe = events[i].data.ptr;
				//printf("epoll_wait-->listen fd=%d\n", xe->fd);

				if (xe->events & events[i].events)
				{
					xe->callback(xe->fd, xe->events, xe);  // 回调事件 
				}
			
			}
		}
	
	
	}
	// 关闭lfd
	Close(lfd);
	return 0;
}
