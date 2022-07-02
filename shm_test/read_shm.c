#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
	// 1. 打开或创建一块共享内存
	int shmid = shmget(100, 1024 *4, IPC_CREAT|0644);
	// 2.跟进程建立连接
	void *ptr = shmat(shmid, NULL, 0);

	// 3. 读写数据
	char s[1024];
	memcpy(s, ptr, 1024);
	printf("%s\n", s);

	puts("按任意键删除共享内存\n");
	getchar();
	// 4. 断开连接
	shmdt(ptr);
	// 5.标记删除
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
