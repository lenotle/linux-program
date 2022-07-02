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
	char buf[4]={192,168,1,2};
	int num = *(int *)buf;
	int sum = htonl(num);
	unsigned char *p = (unsigned char*)&sum;

	printf("%d %d %d %d\n",*p,*(p+1),*(p+2),*(p+3));

	unsigned short s = 0x0abc;
	unsigned short res = htons(s);
	p = (unsigned char*)&res;
	printf("%x %x\n", *p, *(p+1));

	return 0;
}
