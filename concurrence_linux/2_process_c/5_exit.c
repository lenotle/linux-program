#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


// exit: stdlib.h, 等价于 return, 会刷新缓冲区，退出注册函数等, 之后再调用 _exit()系统调用退出程序
// _exit: unistd.h, 立刻调用 _exit()系统调用退出程序
// _Exit 等价于 _exit()
int main()
{
	printf("hello world"); // 不加换行 刷新缓冲区
	
	//exit(0); // return 0 
	
	//_exit(0);
	_Exit(0);
	//return 0; // return 0 会刷新缓冲区，退出注册函数等, 之后再调用 _exit()系统调用退出程序

}

