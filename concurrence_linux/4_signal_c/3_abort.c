#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// void abort(void); 给自己发送一个异常终止的信号 6: SIGABRT

int main()
{
	int i = 0;
	
	while(1)
	{
		printf("doing work %d times\n", i);
		if (4 == i)
		{
			abort();
		}
		i++;
		sleep(1);
	
	}
	return 0;
}
