#include "threadpoolsimple.h"

//#define num 1000 // 任务编号

ThreadPool *thrPool = NULL;
int num = 0;

void *thrRun(void *arg);

void create_threadpool(int thrnum,int maxtasknum)
{
	printf("begin call %s-----\n", __FUNCTION__);
	thrPool = (ThreadPool*)malloc(sizeof(ThreadPool));
	if (NULL == thrPool)
	{
		perror("malloc");
		return;
	}

	thrPool->thr_num = thrnum; // 线程个数
	thrPool->max_job_num = maxtasknum; // 最大任务个数
	thrPool->job_num = 0; // 实际任务个数
	thrPool->job_push = 0; // 入队位置
	thrPool->job_pop = 0;
	thrPool->shutdown = 0; // 不销毁线程池

	// 任务队列数组
	thrPool->tasks = (PoolTask*)malloc((sizeof(PoolTask)*maxtasknum));
	if (thrPool->tasks == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// 初始化锁
	pthread_mutex_init(&thrPool->pool_lock, NULL);
	// 初始化条件变量
	pthread_cond_init(&thrPool->empty_task, NULL);
	pthread_cond_init(&thrPool->not_empty_task, NULL);

	// 申请线程id空间
	thrPool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thrnum);

	// 线程分离属性
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	int i = 0;
	for (i = 0; i < thrnum; i++)
	{
		pthread_create(&thrPool->threads[i], &attr, thrRun, (void*)thrPool);
	}
}


void destroy_threadpool(ThreadPool *pool)
{
	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->not_empty_task);

	pthread_cond_destroy(&pool->not_empty_task);
	pthread_cond_destroy(&pool->empty_task);
	pthread_mutex_destroy(&pool->pool_lock);

	free(pool->tasks);
	free(pool->threads);
	free(pool);
}

// 添加任务
void addtask(ThreadPool *pool,int fd,struct epoll_event *evs)
{
	printf("begin call %s----\n", __FUNCTION__);
	
	pthread_mutex_lock(&pool->pool_lock);

	// 实际任务总数 大于等于 最大任务数 阻塞
	while(pool->job_num >= pool->max_job_num)
	{
		pthread_cond_wait(&pool->empty_task, &pool->pool_lock);
	}

	int pos = (pool->job_push++) % pool->max_job_num;
	printf("add task %d tasknum==%d\n", pos, num);

	pool->tasks[pos].tasknum = num++;
	pool->tasks[pos].fd = fd;
	pool->tasks[pos].evs = evs;
	pool->tasks[pos].task_func = taskRun;
	pool->tasks[pos].arg = (void*)&pool->tasks[pos];
	pool->job_num++;

	pthread_mutex_unlock(&pool->pool_lock);

	// 通知消费者
	pthread_cond_signal(&pool->not_empty_task);
}

void *thrRun(void *arg)
{
	//printf("begin call %s-----\n",__FUNCTION__);
	int taskpos = 0; // 任务位置
	PoolTask *task = (PoolTask *)malloc(sizeof(PoolTask));

	while(1)
	{
		// 获取``任务
		pthread_mutex_lock(&thrPool->pool_lock);

		// 无任务且不需要销毁线程池则等待
		while(thrPool->job_num <= 0 && !thrPool->shutdown)
		{
			pthread_cond_wait(&thrPool->not_empty_task, &thrPool->pool_lock);
		}

		if (thrPool->job_num)
		{
			// 有任务
			taskpos = (thrPool->job_pop++) % thrPool->max_job_num;
			printf("task out %d...tasknum ==%d\n", taskpos, thrPool->tasks[taskpos].tasknum);
			memcpy(task, &thrPool->tasks[taskpos], sizeof(PoolTask)); // 避免任务被修改
			task->arg = task;
			thrPool->job_num--;

			pthread_cond_signal(&thrPool->empty_task);
		}

		if (thrPool->shutdown)
		{
			pthread_mutex_unlock(&thrPool->pool_lock);
			free(task);
			pthread_exit(NULL);
		}

		// 释放锁
		pthread_mutex_unlock(&thrPool->pool_lock);
		task->task_func(task->arg); // 回调函数
	}
}


void taskRun(void *arg)
{
	PoolTask *task = (PoolTask*)arg;
	char buf[1024]= {'0'};
	int n = read(task->fd , buf,sizeof(buf));
	//printf("read %s, %d bytes\n", buf, n);
	if(n == 0 )
	{
		close(task->fd);//关闭cfd
		epoll_ctl(task->epfd,EPOLL_CTL_DEL,task->fd,task->evs); //将cfd下树
		printf("client close\n");
	}
	else if(n> 0)
	{
		printf("%s\n",buf );
		n = write(task->fd, buf, n);
		//printf("write res: %d bytes\n", n);

	}
	printf("taskRun ending....\n");
}

int main()
{
	create_threadpool(3, 100); // 3线程，最大任务数100

	int i = 0;

	//创建套接字,绑定
	int lfd = tcp4bind(8000,NULL);
	
	//监听
	listen(lfd,128);
	
	//创建树
	int epfd = epoll_create(1);
	struct epoll_event ev, evs[1024];
	ev.data.fd = lfd;
	ev.events = EPOLLIN;//监听读事件
	
	//将ev上树
	epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);

	while(1)
	{
		int nready = epoll_wait(epfd,evs,1024,-1);
		if(nready < 0)
		{
			perr_exit("epoll_wait");
		}
		else if (nready > 0)
		{
			for (i = 0; i < nready; i++)
			{
				if(evs[i].data.fd == lfd && evs[i].events & EPOLLIN)//如果是lfd变化,并且是读事件
				{
					struct sockaddr_in cliaddr;
					char buf_ip[16]="";
					socklen_t len  = sizeof(cliaddr);

					int cfd = Accept(lfd,(struct sockaddr *)&cliaddr,&len);

					printf("client ip=%s port=%d\n",inet_ntop(AF_INET,
								&cliaddr.sin_addr.s_addr,buf_ip,sizeof(buf_ip)),
							ntohs(cliaddr.sin_port));
					ev.data.fd = cfd;//cfd上树
					ev.events = EPOLLIN;//监听读事件

					epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);//将cfd上树
				}
				else if(evs[i].data.fd != lfd && evs[i].events & EPOLLIN)//普通的读事件
				{
					addtask(thrPool,evs[i].data.fd,&evs[i]);
				}
			
			}
		}
	}
	
	Close(lfd);
	destroy_threadpool(thrPool);
	return 0;
}
