
#include <stdio.h>
#include <arpa/inet.h>

/*
 * 网络字节序都是大端模式
 * 网络字节序和本机字节序相互转换
 * 
 * 本机->网络 l:32bit s:16bit
 *      uint32_t htonl(uint32_t hostlong);
 *  	uint16_t htons(uint16_t hostshort);
 *
 * network --> local 
 *	uint32_t ntohl(uint32_t netlong);
 *	uint16_t ntohs(uint16_t netshort);
 *
 *
 *
 *
 */
int main()
{
	// 网络字节序转换为 本地字节序
	unsigned char arg[4] = {1, 2, 168, 192};
	uint32_t num = ntohl(*(uint32_t *)arg);
	unsigned char *p = (unsigned char*)&num;
	printf("%d %d %d %d\n", *p, *(p+1), *(p+2), *(p+3));

	return 0;
}
