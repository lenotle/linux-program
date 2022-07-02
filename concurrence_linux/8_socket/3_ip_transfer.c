#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


/*
 * IP 转换为 网络字节序
 * int inet_pton(int af, const char *src, void *dst);  convert IPv4 and IPv6 addresses from text to binary form
 * 	af:
 * 		AF_INET: IPv4
 * 		AF_INET6: IPv6
 * 	src: 需要转换的ip地址
 * 	dst: 传出的转换结果
 * 	
 * 	return value: 1 succ, 0 faild, set errno 
 *
 * 网络字节序 转换为 本地ip 
 * 
 * const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
 *
 */
int main()
{
	// char ip[] = "192.168.1.1";
	if (0 == inet_pton(AF_INET, ip, buf))
	{
		printf("inet_pton failed...\n");
		exit(EXIT_FAILURE);
	}

	printf("network address: %s\n", buf);	

	if (NULL == inet_ntop(AF_INET, buf, str, INET_ADDRSTRLEN))
	{
		printf("inet_ntop failed...\n");
		exit(EXIT_FAILURE);
	}

	printf("ip: %s\n", str);
	return 0;
}
