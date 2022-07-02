#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/*
 *
 *	#include <signal.h>  
 *	int sigemptyset(sigset_t *set);       //将set集合置空
 *	int sigfillset(sigset_t *set)；          //将所有信号加入set集合
 *	int sigaddset(sigset_t *set, int signo);  //将signo信号加入到set集合
 *	int sigdelset(sigset_t *set, int signo);   //从set集合中移除signo信号
 *	int sigismember(const sigset_t *set, int signo); //判断信号是否存在
 *
 *
 */
void show_sigset(sigset_t *set)
{
	int i = 0;
	for (i = 1; i <=32; i++)
	{
		if (sigismember(set, i))
		{
			printf("1");
		}
		else 
		{
			printf("0");
		}
	}
	putchar('\n');

}


int main()
{
	sigset_t set;
	int i = 0;

	// 1. 将集合置空
	sigemptyset(&set);

	// 2. 判断信号是否存在
	show_sigset(&set);
	
	// 3. 添加1-32信号
	sigfillset(&set);
	show_sigset(&set);
	
	// 4. 删除 5号信号
	sigdelset(&set, 5);
	show_sigset(&set);

	// 5. add five signal
	sigaddset(&set, 5);
	show_sigset(&set);

	return 0;
}

