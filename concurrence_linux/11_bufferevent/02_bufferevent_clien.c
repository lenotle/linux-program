#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <event.h>
#include <event2/listener.h>


void cmd_msg_cb(evutil_socket_t fd, short events, void *arg);
void server_msg_cb(struct bufferevent *bev, void *ctx);
void event_cb(struct bufferevent *bev, short event, void *arg);

// 客户端功能
// 1. 从标准输入读取输入，发送给服务器 ----> 普通event事件
// 2. 接受服务器的消息                 ----> bufferevent 事件
int main()
{

	struct event_base *base;            // 根节点
	struct event      *ev_cmd;          // 监听stdin
	struct bufferevent *bev;            // bufferevent 缓冲区
	struct sockaddr_in sin;             // server info 

	do
	{
		if ((base = event_base_new()) == NULL)
		{
			fprintf(stderr, "Could not create event_base\n");
			break;
		}

		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(8888);
		sin.sin_addr.s_addr = inet_addr("127.0.0.1");

		// 初始化bev缓冲区
		bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
		if (!bev || bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		{
			fprintf(stderr, "Could create bufferevent /connect server, errmsg: %s\n", strerror(errno));
			break;
		}

		//设置buffer的回调函数 主要设置了读回调 server_msg_cb ,传入参数是标准输入的读事件
		bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);
		bufferevent_enable(bev, EV_READ | EV_PERSIST);

		// 初始化 event事件
		if ((ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, cmd_msg_cb, (void *)bev)) == NULL)
		{
			fprintf(stderr, "Could not create event\n");
			break;
		}

		// 节点上树
		event_add(ev_cmd, NULL);


		// 循环监听
		event_base_dispatch(base);
	}while(0);

	event_free(ev_cmd);
	bufferevent_free(bev);
	event_base_free(base);
	return 0;
}

// 从标准输入读取并发送数据到服务器
void cmd_msg_cb(evutil_socket_t fd, short events, void *arg)
{
	char buf[1500] = {'0'};

	int n = read(fd, buf, sizeof(buf));
	if (n < 0)
	{
		fprintf(stderr, "%s read error", __FUNCTION__);
		exit(-1);
	}
	
	struct bufferevent *bev = (struct bufferevent *)arg;
	bufferevent_write(bev, buf, sizeof(buf));

}

// 接受服务器端消息
void server_msg_cb(struct bufferevent *bev, void *ctx)
{
	char buf[1500] = {'0'};
	int len = bufferevent_read(bev, buf, sizeof(buf));
	buf[len] = '\0';
	printf("recv: %s\n", buf);
}

void event_cb(struct bufferevent *bev, short event, void *arg)
{
	if (event & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (event & BEV_EVENT_ERROR)
		printf("some other error\n");
	else if( event & BEV_EVENT_CONNECTED)
	{
		printf("the client has connected to server\n");
		return ;
	}


	//这将自动close套接字和free读写缓冲区
	bufferevent_free(bev);
	//释放event事件 监控读终端
	struct event *ev = (struct event*)arg;
	event_free(ev);
	exit(0);
}
