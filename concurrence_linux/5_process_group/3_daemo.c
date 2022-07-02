#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#define SIZE 50

void write_currrent_time(int signum)
{

	time_t rawtime;
	struct tm *pT;
	int fd = -1;
	char name[SIZE];

	// 获取时间
	time(&rawtime);

	// 转换为本地时间
	pT = localtime(&rawtime);
	if (NULL == pT)
	{
		perror("localtime");
		return;
	}		

	memset(name, 0, SIZE);
	sprintf(name, "%s%d%d%d%d%d%d.log", "touch /tmp/log/",  pT->tm_year + 1900, pT->tm_mon + 1, pT->tm_mday, pT->tm_hour,     pT->tm_min, pT->tm_sec);
	// printf("name: %s\n", name);

	system(name);
}





// 每隔一秒在指定目录下创建文件（文件名以时间开头）
int main()
{
	pid_t pid = -1;
	int ret = -1;
	struct sigaction act;

	// 1. 创建子进程，父进程出
	pid = fork();
	if (-1 == pid)
	{
		perror("fork");
		return 1;
	}	
	if (pid > 0)
	{
		exit(0);
	}
	else 
	{
		// 2 . 在子进程中创建新会话
		pid = setsid();
		if (-1 == pid)
		{
			perror("setsid");
			return 1;
		}

		// 3. 改变当前目录 （可选
		ret = chdir("/");
		if (-1 == ret)
		{
			perror("chdir");
			return 1;
		}

		// 4. 重设文件掩码 (可选)
		umask(0);

		// 5. 关闭文件描述符
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDIN_FILENO);

		// 6. working
		act.sa_flags = 0;
		act.sa_handler = write_currrent_time;
		sigemptyset(&act.sa_mask);
		sigaction(SIGALRM, &act, NULL);

		struct itimerval timer;
		timer.it_value.tv_sec = 1;
		timer.it_value.tv_usec = 0;
		timer.it_interval.tv_sec = 2;
		timer.it_interval.tv_usec = 0;

		ret = setitimer(ITIMER_REAL, &timer, NULL);
		if (-1 == ret)
		{
			perror("setitmer");
			return 1;
		}
		while(1);
	}	

	return 0;
}
