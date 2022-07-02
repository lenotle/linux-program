#include <stdio.h>
#include <stdlib.h>
#include <event.h>
#include "wrap.h"

#define _EVENT_SIZE 1024

typedef struct fd_event
{
	int fd;
	struct event *ev;
}xevs;

xevs xevent[_EVENT_SIZE];

void init_xevent()
{
	int i = 0;
	for (; i < _EVENT_SIZE; i++)
	{
		xevent[i].fd = -1;
		xevent[i].ev = NULL;
	}
}

void destroy_xevent()
{
	int i = 0;
	for (; i < _EVENT_SIZE; i++)
	{
		xevent[i].fd = -1;
		free(xevent[i].ev);
		xevent[i].ev = NULL;
	}
}

int find_index()
{
	int i = 0;
	for (i = 0; i < _EVENT_SIZE; i++)
	{
		if (xevent[i].fd < 0)
		{
			break;
		}
	}
	return (i == _EVENT_SIZE) ? -1 : i;
}

void xevent_add(int fd,  struct event * ev)
{
	int i = find_index();
	if (i < 0)
	{
		printf("xevent full...\n");
		return;
	}
	xevent[i].fd = fd;
	xevent[i].ev = ev;

}

int find_fd(int fd)
{
	int i = 0;
	for (i = 0; i < _EVENT_SIZE; i++)
	{
		if (xevent[i].fd == fd)
		{
			break;
		}
	}
	
	// check ....
	return i;
}
void c_cb(int fd, short events, void *arg)
{
	printf("begin %d call %s...\n",fd,  __FUNCTION__);
	char buf[1500] = {'0'};

	int n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0)
	{
		Close(fd);
		// event_del
		event_del(xevent[find_fd(fd)].ev);
		xevent[find_fd(fd)].fd = -1;
		free(xevent[find_fd(fd)].ev);
		printf("client %d closed....\n", fd);
	}
	else 
	{
		write(STDOUT_FILENO, buf, n);
		send(fd, buf, n, 0);
	}
}


void s_cb(int fd, short events, void *arg)
{
	printf("begin %d call %s...\n",fd,  __FUNCTION__);
	struct event_base *base = (struct event_base *)arg;

	int cfd = Accept(fd, NULL, NULL);

	struct event *ev = event_new(base, cfd, EV_READ | EV_PERSIST, c_cb, NULL);
	event_add(ev, NULL);

	xevent_add(cfd, ev);
}


int main()
{
	// create socket and bind 
	int lfd = tcp4bind(8080, NULL);

	// listen 
	Listen(lfd, 128);

	// create event_base
	struct event_base *base = event_base_new();

	init_xevent();

	// init event 
	struct event *ev = event_new(base, lfd, EV_READ | EV_PERSIST, s_cb, base);
	
	// add ev 
	event_add(ev, NULL);

	// add lfd to xevent 
	xevent_add(lfd, ev);

	event_base_dispatch(base);

	Close(lfd);
	destroy_xevent();
	event_base_free(base);
	return 0;

}
