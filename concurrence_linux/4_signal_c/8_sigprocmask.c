#include <stdio.h>
#include <signal.h>

// int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
// 功能：
//    检查或修改信号阻塞集，根据 how 指定的方法对进程的阻塞集合进行修改，
//    新的信号阻塞集由 set 指定，而原先的信号阻塞集合由 oldset 保存。


// int sigpending(sigset_t *set);
// 功能：读取当前进程的未决信号集

void fun(int signum)
{
	printf("捕捉到%d信号\n", signum);
}

int main()
{
	sigset_t set;
	sigset_t old_set;

	// 捕捉2号信号
	signal(SIGINT, fun);
	
	// 按下任意键阻塞2号信号
	printf("按下任意键阻塞2号信号\n");
	getchar();

	sigemptyset(&set);
	sigemptyset(&old_set);
	sigaddset(&set, SIGINT);

	int ret = sigprocmask(SIG_BLOCK, &set, &old_set);
	if (0 != ret)
	{
		perror("sigprocmask");
		return 1;
	}
	printf("阻塞2号信号成功\n");
	
	/////////////////////////////////////////////////////////
	printf("按下任意键读取当前进程未决信号集\n");
	getchar();

	sigemptyset(&set);
	ret = sigpending(&set);
	if (-1 == ret)
	{
		perror("sigpending");
		return 1;
	}
	int i = 0;
	for (i = 1; i <= 32; i++)
	{
		if (sigismember(&set, i))
		{
			printf("%d ", i);
		}
	}
	putchar('\n');
	///////////////////////////////////////////////////////
	
	// 按下任意键解除屏蔽
	printf("按下任意键解除屏蔽\n");
	getchar();
	
	sigemptyset(&set);
	ret = sigprocmask(SIG_SETMASK, &old_set, &set);
	if (0 != ret)
	{
		perror("sigprocmask");
		return 1;
	}
	printf("解除屏蔽成功\n");

	printf("按下任意键退出\n");
	getchar();
	return 0;
}
