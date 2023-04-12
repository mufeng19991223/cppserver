#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <stdio.h>


#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 4

// 将文件描述符设置成非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// 将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核事件表中，参数enable_et指定是否对fd启用ET模式
void addfd(int epollfd, int fd, bool enable_et)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et)
    {
        event.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//LT
void lt(epoll_event* events, int number, int epollfd, int listenfd)
{
    char buf[ BUFFER_SIZE ];
    for (int i = 0; i < number; ++i)
    {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);
            addfd(epollfd, connfd, false);
        }
        else if (events[i].events & EPOLLIN)
        {
            // 只要socket读缓存中还有未读出的数据，这段代码就被触发
            printf("event trigger once\n");
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);  // ？
            if (ret <= 0)
            {
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n", ret, buf);
        }
        else
        {
            printf("something else happened\n");
        }
    }
}

// ET
void et(epoll_event* events, int number, int epollfd, int listenfd)
{
    char buf[ BUFFER_SIZE ];
    for (int i = 0; i < number; ++i)
    {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlen = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlen);
            addfd(epollfd, connfd, true);
        }
        else if (events[i].events & EPOLLIN)
        {
            // 这段代码不会被重复触发，所以我们循环读取数据，以确保把socket读缓存中的所有数据读出
            printf("event trigger once\n");
            while (1)
            {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);  // ？
                if (ret < 0)
                {
                    // 对于非阻塞IO,下面的条件成立表示数据已经全部读取完毕。此后，epoll就能再次触发sockfd上的
                    // EPOLLIN事件，以驱动下一次读操作
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    continue;
                }
                else if (ret == 0)
                {
                    close(sockfd);
                }
                else
                {
                    printf("get %d bytes of content: %s\n", ret, buf);
                }
            }
        }
        else
        {
            printf("something else happened\n");
        }
    }
}

int main()
{
    const char* ip = "192.168.152.128";
    int port = 6666;

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);
    
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd, true);

    while (1)
    {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0)
        {
            printf("epoll failure\n");
            break;
        }
        lt(events, ret, epollfd, listenfd);
        // et(events, ret, epollfd, listenfd);
    }
    
    close(listenfd);
    return 0;
}