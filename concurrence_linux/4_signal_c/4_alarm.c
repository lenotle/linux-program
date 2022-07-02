#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// unsigned alarm(unsigned seconds
// 在指定seconds后，内核会给当前进程发送14）SIGALRM信号。进程收到该信号，默认动作终止。每个进程都有且只有唯一的一个定时器
// 闹钟可以被重置
// 返回值为 上一个闹钟剩余的毫秒数 
int main()
{
	int ret = 0;

	ret = alarm(5);
	printf("realtime %d\n", ret);
	
	sleep(2);

	ret = alarm(2);
	printf("realtime %d\n", ret);
	
	return 0;
}
