#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

// pid_t getsid(pid_t pid); 获取进程所属的会话ID , pid：进程号，pid为0表示查看当前进程session ID
//
// pid_t setsid(void);  创建一个会话，并以自己的ID设置进程组ID，
// 同时也是新会话的ID。调用了setsid函数的进程，既是新的会长，也是新的组长。
//
// tips: 调用进程是组长进程，则出错返回 
int main()
{
	pid_t pid = -1;

	pid = getsid(0);
	if (-1 == pid)
	{
		perror("getsid");
		return 1;
	}
	printf("getsid(0)=%d\n", pid);


	pid = setsid();
	if (-1 == pid)
	{
		perror("setsid");
		return 1;
	}
	return 0;
}
