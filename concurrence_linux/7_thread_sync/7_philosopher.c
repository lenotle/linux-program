#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mu[5];


void *dine(void *arg)
{
        int num = (int)(long)arg;
        int left, right;

        // 前四个人 右手拿自己筷子，左手拿下一个人的
        if (num < 4)
        {
                right = num;
                left = num + 1;
        }
        else if (num == 4) // 最后一个人 左手拿自己的筷子，右手拿第一个人的
        {
                right = 0;
                left = num;
        }

        while(1)
        {
                pthread_mutex_lock(&mu[right]);
                if (pthread_mutex_trylock(&mu[left]) == 0)
                {
                        printf("%d 正在吃饭.....\n", num);
                        pthread_mutex_unlock(&mu[left]);
                }
                pthread_mutex_unlock(&mu[right]);
                sleep(random() % 5 + 1);
        }

}

int main()
{
        pthread_t p[5];
        int i = 0;

        // init
        for (i = 0; i < 5; i++)
        {
                pthread_mutex_init(&mu[i], NULL);
        }

        for (i = 0 ; i < 5; i++)
        {
                pthread_create(&p[i], NULL, dine, (void*)(long)i);
        }

        for (i = 0; i < 5; i++)
        {
                pthread_join(p[i], NULL);
        }

        // destroy
        for (i = 0; i < 5; i++)
        {
                pthread_mutex_destroy(&mu[i]);
        }

        return 0;
}

