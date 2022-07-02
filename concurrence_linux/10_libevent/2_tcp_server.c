#include <stdio.h>
#include <stdlib.h>
#include <event.h>
#include "wrap.h"

void c_cb(int fd, short events, void *arg)
{
	char buf[1500] = {'0'};

	int n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0)
	{
		Close(fd);
		// event_del
	}
	else 
	{
		write(STDOUT_FILENO, buf, n);
		send(fd, buf, n, 0);
	}
}


void s_cb(int fd, short events, void *arg)
{
	struct event_base *base = (struct event_base *)arg;

	int cfd = Accept(fd, NULL, NULL);

	struct event *ev = event_new(base, cfd, EV_READ | EV_PERSIST, c_cb, NULL);
	event_add(ev, NULL);
}


int main()
{
	// create socket and bind 
	int lfd = tcp4bind(8080, NULL);

	// listen 
	Listen(lfd, 128);

	// create event_base
	struct event_base *base = event_base_new();

	// init event 
	struct event *ev = event_new(base, lfd, EV_READ | EV_PERSIST, s_cb, base);
	
	// add ev 
	event_add(ev, NULL);

	event_base_dispatch(base);

	Close(lfd);
	event_base_free(base);
	return 0;

}
