#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <sys/socket.h>

#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

#define PORT 8888
static const char MESSAGE[] = "Hello, World!\n";

static void listener_cb(struct evconnlistener*, evutil_socket_t, struct sockaddr *, int socklen, void *);
static void read_cb(struct bufferevent *bev, void *ctx);
static void write_cb(struct bufferevent *bev, void *ctx);
static void event_cb(struct bufferevent *bev, short what, void *ctx);
static void signal_cb(evutil_socket_t sig, short events, void *ptr);

// bufferevent实现server 回射功能
int main(int argc, char *argv[])
{
	// 创建根节点
	struct event_base *base = event_base_new();

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// 创建、绑定、监听、提取连接
	// 创建bufferevent 链接监听器
	struct evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, (void *)base, 
			LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (struct sockaddr *)&server_addr, 
			sizeof(server_addr));
	if (!listener)
	{
		fprintf(stderr, "Could not create evconnlistener\n");
		return 1;
	}

	// 创建信号节点 ctrl +c 
	struct event *signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	if (!signal_event || event_add(signal_event, NULL) < 0)
	{
		fprintf(stderr, "Could not create/add signal event\n");
		return 1;
	}

	// 循环监听
	event_base_dispatch(base);

	event_free(signal_event);
	evconnlistener_free(listener);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void listener_cb(struct evconnlistener *evl, evutil_socket_t fd, struct sockaddr *cliaddr, int socklen, void *ptr)
{
	struct event_base *base = (struct event_base *)ptr;

	// 新建bufferevent 
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Erroring Constructing Bufferevent, %s\n", strerror(errno));
		event_base_loopbreak(base);
		return;
	}

	// 设置回调
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, (void *)base);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	// 给客户端发送消息
	bufferevent_write(bev, MESSAGE, sizeof(MESSAGE));
	struct sockaddr_in *sin = (struct sockaddr_in*)cliaddr;
	printf("client %d, connect successfully\n", ntohs(sin->sin_port));
}



static void write_cb(struct bufferevent *bev, void *ctx)
{
	// 获取缓冲区类型
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0)
	{
		printf("send succ\n");
	}
}

static void read_cb(struct bufferevent *bev, void *ctx)
{
	char buf[1500] = {'0'};
	bufferevent_read(bev, buf, sizeof(buf));
	printf("recv: %s\n", buf);

	// 回射
	bufferevent_write(bev, buf, sizeof(buf));
}

static void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (what == BEV_EVENT_EOF)
	{
		printf("client closed connection\n");
	}
	else if (what == BEV_EVENT_ERROR)
	{
		printf("Got an error on the connection: %s\n", strerror(errno));
	}

	bufferevent_free(bev);

}

// 信号回调函数
static void signal_cb(evutil_socket_t sig, short events, void *ptr)
{
	struct event_base *base = (struct event_base *)ptr;
	struct timeval delay = { 2, 0 };

	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);//退出循环监听
}
