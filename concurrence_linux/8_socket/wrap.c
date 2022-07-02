#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

void perr_exit(const char *s)
{
	perror(s);
	exit(-1);
}

/*  On  success, a file descriptor for the new socket is returned.  On error, -1 is returned */
int Socket(int family, int type, int protocol)
{
	int n;
	if ((n = socket(family, type, protocol)) == -1)
	{
		perr_exit("socket error");
	}
	return n;
}

int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	if ((n = connect(fd, sa, salen)) < 0)
	{
		perr_exit("connect error");
	}
	return n;

}

int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;

	if ((n = bind(fd, sa, salen)) < 0)
	{	
		perr_exit("bind error");
	}
	return n;
}

int tcp4bind(short port, const char *IP)
{
	struct sockaddr_in addr;
	// create socket 
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));

	if (IP == NULL)
	{
		// IP 为空则使用0.0.0.0， 任意IP可以连接
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (inet_pton(AF_INET, IP, &addr.sin_addr.s_addr) <= 0)
		{
			perr_exit("tcp4bind->inet_pton error");
		}
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	Bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
	return lfd;

}


int Listen(int fd, int backlog)
{
	int n;

	if ((n = listen(fd, backlog)) < 0)
	{
		perr_exit("listen error");
	}
	return n;
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int cld;

again:
	if ((cld = accept(fd, sa, salenptr)) < 0)
	{
		if ((errno == ECONNABORTED) || (errno == EINTR))
		{
			// 如果信号被中断及软件层次中断
			goto again;
		}
		else
		{
			perr_exit("accept error");
		}
	}

	return cld;
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ((n == read(fd, ptr, nbytes)) == -1)
	{
		if (errno == EINTR)
		{
			goto again;
		}
		else
		{
			return -1;
		}
	}
	return n;
}

ssize_t Write(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;

again:
	if ((n == write(fd, ptr, nbytes)) == -1)
	{
		if (errno == EINTR)
		{
			goto again;
		}
		else
		{
			return -1;
		}
	}
	return n;
}


/*参三: 应该读取固定的字节数数据*/
ssize_t Readn(int fd, void *vptr, size_t n)
{
	size_t  nleft;              // usigned int 剩余未读取的字节数
	ssize_t nread;              // int 实际读到的字节数
	char   *ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0)
	{
		if ((nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nread = 0;
			}
			else
			{
				return -1;
			}
		}
		else if (nread == 0)
		{
			break;
		}

		nleft -= nread;
		ptr += nread;
	}

	return n - nleft;
}

/*参三: 应该写入固定的字节数数据*/
ssize_t Writen(int fd, void *vptr, size_t n)
{
	size_t  nleft;              // usigned int 剩余未写入的字节数
	ssize_t nwritten;              // int 实际写入的字节数
	char   *ptr;

	ptr = vptr;
	nleft = n;

	while(nleft > 0)
	{
		if ((nwritten = write(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nwritten = 0;
			}
			else
			{
				return -1;
			}
		}
		else if (nwritten == 0)
		{
			break;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return n - nleft;
}
// 一次读取100字符，然后依次返回100字符
static ssize_t my_read(int fd, char *ptr)
{
	static int read_cnt;
	static char *read_ptr;
	static char read_buf[100];

	// 第一次会进入该if，读取100字
	if (read_cnt <= 0)
	{
again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
		{
			if (errno == EINTR)
			{
				goto again;
			}
			return -1;
		}
		else if (read_cnt == 0)
		{
			return 0;
		}
		read_ptr = read_buf;
	}
	
	read_cnt--;
	*ptr = *read_ptr++;

	return 1;
}

ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char    c, *ptr;

	ptr = vptr;

	for (n = 1; n < maxlen; n++)
	{
		if ((rc = my_read(fd, &c)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
			{
				break;
			}
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return n - 1;
		}
		else 
		{
			return -1;
		}
	}
	*ptr = 0;

	return n;

}

int Close(int fd)
{
	int n;
	if ((n = close(fd)) == -1)
	{
		perr_exit("close error");
	}
	return n;
}


