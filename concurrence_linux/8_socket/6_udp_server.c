#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socklen_t len = sizeof(addr);
	if (bind(fd, (struct sockaddr *)&addr, len) != 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in cliaddr;
	len = sizeof(cliaddr);
	char buf[1024];
	while(1)
	{
		memset(buf, 0, sizeof(buf));
		int n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
		if (n < 0)
		{
			perror("recvfrom");
			break;
		}
		else
		{
			write(STDOUT_FILENO, buf, n);
			sendto(fd, buf, n, 0, (struct sockaddr *)&cliaddr, len);
		}
	
	
	}
	close(fd);
	return 0;
}
