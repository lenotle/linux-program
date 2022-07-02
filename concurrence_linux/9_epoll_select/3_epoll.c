#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include "wrap.h"
int main()
{
	int fd = tcp4bind(8080, NULL);
	Listen(fd, 128);

	// 创建epoll句柄
	int epfd = epoll_create(1);

	struct epoll_event ev, evs[1024];
	// 将fd添加到epoll红黑树中
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

	while(1)
	{
		// epoll 监听
		int nready = epoll_wait(epfd, evs, sizeof(evs), -1);
		printf("nready...\n");
		if (nready < 0)
		{
			perror("epoll_wait");
			break;
		}	
		else if (nready == 0)
		{
			continue;
		}
		else 
		{
			for (int i = 0; i < nready; i++)
			{
				if ((evs[i].data.fd == fd) && (evs[i].events & EPOLLIN))
				{
					// fd 
					struct sockaddr_in addr;
					socklen_t len = sizeof(addr);
					char ip[16] = {'0'};
					int cfd = Accept(fd, (struct sockaddr *)&addr, &len);

					printf("client,ip=%s, port=%d\n",
							inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, 16),
							ntohs(addr.sin_port));	

					// add cfd to epoll 
					ev.events = EPOLLIN;
					ev.data.fd = cfd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
				}
				else if (evs[i].events & EPOLLIN)
				{
					char buf[1024] = {'0'};
					int n = read(evs[i].data.fd, buf, sizeof(buf));
					if (n < 0)
					{
						perror("read");
						Close(evs[i].data.fd);
						epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &evs[i]);
					}
					else if (n == 0)
					{
						printf("client closed\n");
						Close(evs[i].data.fd);
						epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, &evs[i]);
					}
					else 
					{
						write(STDOUT_FILENO, buf, n);
						write(evs[i].data.fd, buf, n);
					}
				}

			}

		}
	}

	close(fd);
	return 0;
}
