#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

int main(int argc, char *argv[])
{

	MD5_CTX c;
	MD5_Init(&c);

	const char *str = "hello";

	MD5_Update(&c, str, strlen(str));

	unsigned char md[16] = {0};
	MD5_Final(md, &c);

	char res[33] = {0};
	int i;

	for (i = 0; i < 16; i++)
	{
		sprintf(&res[i *2], "%02x", md[i]);
	}

	printf("%s", res);


	return 0;
}
