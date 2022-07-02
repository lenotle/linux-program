#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>

#include "wrap.h"
#include "pub.h"

#define PORT 8888
#define true 1
#define false 0
#define _EVENT_SIZE 1024

void change_word_dir();
void process_http_request(int epfd, struct epoll_event *evs);
void send_header(int cfd, int code, const char *status, const char *mimeType, int length);
void send_file(int epfd, int cfd, struct epoll_event *ev, const char *path, int flags);

/* 
    epoll 实现web服务器
        1. 获取http请求头信息
        2. 根据请求头信息，请求方式，请求文件等进行响应
        3. 响应具体数据
 */
int main(int argc, char const * argv[])
{
    change_word_dir();
    signal(SIGPIPE, SIG_IGN);
    int lfd;
    int epfd;
    struct epoll_event ev, evs[_EVENT_SIZE];
    int nready;
    int i;

    // 创建socket, bind
    lfd = tcp4bind(PORT, NULL);
    // 监听
    Listen(lfd, 128);

    // 创建epoll根节点
    epfd = epoll_create(1);

    // lfd添加到epoll 
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    while(true)
    {
        // 循环监听epoll 
        nready = epoll_wait(epfd, evs, _EVENT_SIZE, -1);
        if (nready < 0)
        {
            perror("epoll_wait");
            break;
        }
        else // 监听到了事件 
        {
           for (i = 0; i < nready; i++)
           {
               if (lfd == evs[i].data.fd && evs[i].events & EPOLLIN)
               {
                   struct sockaddr_in sin;
                   socklen_t len = sizeof(sin);
                   char ip[16] = {0};
                   // 提取新连接
                   int cfd  = Accept(evs[i].data.fd, (struct sockaddr *)&sin, &len);
                   printf("client connected: %s:%d\n", inet_ntop(AF_INET, &sin.sin_addr.s_addr, ip, sizeof(ip)), ntohs(sin.sin_port));

                    // 设置为非阻塞
                    int flags = fcntl(cfd, F_GETFL);
                    flags |= O_NONBLOCK;
                    fcntl(cfd, F_SETFL, flags);

                    ev.data.fd = cfd;
                    ev.events = EPOLLIN;
                   // 添加到EPoll
                   epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
               }
               else if (evs[i].events & EPOLLIN)
               {
                   process_http_request(epfd, &evs[i]);
               }
           }
        }
    }
    Close(lfd);
    return 0;
}

// 改变当前工作目录
void change_word_dir()
{
    char pwd_path[256] = {0};
    char *buf = getenv("PWD");
    strcpy(pwd_path, buf);
    strcat(pwd_path,"/web-http");
	chdir(pwd_path);
}

// 处理HTTP请求方式
void process_http_request(int epfd, struct epoll_event *evs)
{
    char buf[256] = {0};
    char temp[1024] = {0}; 
    char method[256], content[256], protocol[256];

    int ret = 0;

    // 1. 读取请求行 GET /a.txt  HTTP/1.1\R\N
    ret = recv(evs->data.fd, buf, sizeof(buf), 0);
    if (ret <= 0)
    {
        perror("Closed or Error---->recv");
        Close(evs->data.fd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, evs->data.fd, evs);
        return;
    }
    printf("[%s]\n", buf);

    // 2. 依次丢弃请求头、请求空行
    while(recv(evs->data.fd, temp, sizeof(temp), 0) > 0);

    // 3. 解析请求行
    memset(method, 0, sizeof(method));
    memset(content, 0, sizeof(content));
    memset(protocol, 0, sizeof(protocol));
    ret = sscanf(buf, "%[^ ] %[^ ] %[^ \r\n]", method, content, protocol);
    printf("[%s]  [%s]  [%s]\n", method, content, protocol);
    
    // 4. 请求方式  GET /a.txt  HTTP/1.1\R\N
    if (strcasecmp(method, "get") == 0)
    {
        // a.txt 可能为Unicode编码 解码
        char *str = content + 1;  // content = /a.txt --> str = a.txt 
        strdecode(str, str);

        if (*str == 0)  // 没有请求文件，则选择当前目录
        {
            str = "./";
        }

        // 判断请求文件是否存在
        struct stat s;
        if (stat(str, &s) != 0)
        {
            printf("%s not found\n", str);
            // 发送请求行，请求头，空行
            send_header(evs->data.fd, 404, "Not Found", get_mime_type(".html"), 0);
            // 发送404文件
            send_file(epfd, evs->data.fd, evs, "error.html", 1);
        }
        else 
        {
            // 请求文件是一个目录
            if (S_ISDIR(s.st_mode))
            {
                printf("dir...\n");
                // 发送请求行，请求头，空行
                send_header(evs->data.fd, 200, "OK", get_mime_type("*.html"), 0);

                //发送header.html
				send_file(epfd, evs->data.fd, evs, "dir_header.html", 0);

                // 读取目录
                struct dirent **namelist = NULL;
                int len = 0;
                int n = scandir(str, &namelist, NULL, alphasort);
                char buf[1024] = {0};
                int i = 0;
                for (i = 0; i < n; i++)
                {
                    // 判断每一个文件是否是目录
                    if (namelist[i]->d_type == DT_DIR)
                    {
                        len = sprintf(buf, "<li><a href=%s/ >%s</a></li><br>", namelist[i]->d_name, namelist[i]->d_name);
                    }
                    else 
                    {
                        len = sprintf(buf, "<li><a href=%s >%s</a></li><br>", namelist[i]->d_name, namelist[i]->d_name);
                    }
                    send(evs->data.fd, buf, len, 0);
                    free(namelist[i]);
                }
                free(namelist);

                // 发送尾部
				send_file(epfd, evs->data.fd, evs, "dir_tail.html", 1);

            }
            else if (S_ISREG(s.st_mode))// 请求文件是普通文件
            {
                printf("regular file...\n");
                // 发送请求行，请求头，空行
                send_header(evs->data.fd, 200, "OK", get_mime_type(str), 0);
                // 发送请求文件
                send_file(epfd, evs->data.fd, evs, str, 1);
            }
        }
    }

}

// 发送HTTP响应头：响应行、响应头、响应空行
// 响应行：  HTTP/1.1 200 OK\r\n
// 响应头： Content-type,length
void send_header(int cfd, int code, const char *status, const char *mimeType, int length)
{
    char buf[256] = {0};
    int ret = 0;

    // 发送响应行
    ret = sprintf(buf, "HTTP/1.1 %d %s\r\n", code, status);
    send(cfd, buf, ret, 0);

    // 发送响应头
    ret = sprintf(buf, "Content-Type:%s\r\n", mimeType);
    send(cfd, buf, ret, 0);
    if (length > 0)
    {
        ret = sprintf(buf, "Content-length:%d\r\n", length);
        send(cfd, buf, ret, 0);
    }

    // 发送空行
    send(cfd, "\r\n", 2, 0);
}


// 发送文件
void send_file(int epfd, int cfd, struct epoll_event *ev, const char *path, int flags)
{
    int fd;
    char buf[1024] = {0};
    int ret = 0;

    do
    {
        fd = open(path, O_RDONLY);
        if (fd < 0)
        {
            perror("open...");
            break;
        }

        while(true)
        {
            ret = read(fd, buf, sizeof(buf));
            if (ret <= 0)
            {
                perror("read");
                break;
            }
            printf("send %d bytes\n", send(cfd, buf, ret, 0));
        }
    } while (0);
    
    close(fd);
    if (flags)
    {
        Close(cfd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, ev);
    }
}