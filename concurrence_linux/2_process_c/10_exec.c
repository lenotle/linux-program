#include<stdio.h>
#include<unistd.h>

// int execl(const char *path, const char *arg, .../* (char  *) NULL */);

// int execle(const char *path, const char *arg, .../*, (char *) NULL, char * const envp[] */);

// int execv(const char *path, char *const argv[]);


// int execlp(const char *file, const char *arg, ... /* (char  *) NULL */);

// int execvp(const char *file, char *const argv[]);

// int execvpe(const char *file, char *const argv[], char *const envp[]);



int main()
{
	char *arg[] = {"ls", "-al", "/usr/local/learning/proccess_c", NULL};
	char *envp[] = {"ADDRESS=BEIJING", NULL};
	printf("main start...\n");
	
	// path: 可执行文件路径
	// arg: 参数，最后一个参数必须是NULL
	// envp: 环境变量
	// execl("/bin/ls", "ls", "-al", "/usr/local/learning/proccess_c", NULL);	
	// execle("/bin/ls", "ls", "-al", "/usr/local", NULL, envp);
	// execv("/bin/ls", arg);
	

	//execlp("ls","ls",  "-al", "/usr/local", NULL);
	execvp("ls", arg);
	


	printf("main end...\n");
	return 0;
}

