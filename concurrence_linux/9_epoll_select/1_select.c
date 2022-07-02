#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "wrap.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fputs("argc != 2", stderr);
	 	exit(EXIT_FAILURE);
	}

	int fd = tcp4bind(atoi(argv[1]), NULL);

	Listen(fd, 128);

	int maxfd = fd; // 最大文件描述符
	
	fd_set oldset, rset; // 文件描述符集合
	FD_ZERO(&oldset);
	FD_ZERO(&rset);
	FD_SET(fd, &oldset);

	while(1)
	{
		rset = oldset;

		int n = select(maxfd + 1, &rset, NULL, NULL, NULL);  // 监听文件描述符
		if (n < 0)
		{
			perror("select");
			break;
		}
		else if (n == 0)
		{
			continue; // 没有变化
		}
		else 
		{
			// fd 存在变化 有新的客户端连接
			if (FD_ISSET(fd, &rset))
			{
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				char ip[16] = {'0'};

				int cfd = Accept(fd, (struct sockaddr *)&addr, &len);
				printf("new client ip=%s port=%d\n",inet_ntop(AF_INET,&addr.sin_addr.s_addr,ip,16),
						ntohs(addr.sin_port));
				// 将cfd加入 oldset 
				FD_SET(cfd, &oldset);
				
				if (cfd > maxfd) // 更新最大文件描述符数量
				{
					maxfd = cfd;
				}

				if (--n == 0)
				{
					continue;
				}
			}
			
			// cfd存在变化，则遍历文件描述符
			int i;
			char buf[1500];
			for (i = fd + 1; i <= maxfd; i++)
			{

				memset(buf, 0, sizeof(buf));
				if (FD_ISSET(i, &rset))
				{
					int ret = read(i, buf, sizeof(buf));
					if (ret < 0)
					{
						perror("read");
						// 在文件描述符里去掉i 
						FD_CLR(i, &oldset);
						continue;
					}
					else if (ret == 0)
					{
						perror("client closed");
						FD_CLR(i, &oldset);
						continue;
					}
					else 
					{
						write(STDOUT_FILENO, buf, ret);
						write(i, buf, ret);
					}


				}
			}
		}

	}
	Close(fd);
	return 0;

}
