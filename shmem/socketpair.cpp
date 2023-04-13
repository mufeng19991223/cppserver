#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <error.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <stdlib.h> 


const char* time = "2023年4月13日18:25:36";

int main()
{
    // 同一进程
    // int pipefd[2];
    // char buf[1024] = {0};

    // if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd) == -1)
    // {
    //     printf("Error, socketpair create failed, errno(%d): %s\n", errno, strerror(errno));
    //     return EXIT_FAILURE; 
    // }

    // int s = write(pipefd[0], time, strlen(time));
    // read(pipefd[1], buf, s);
    // printf("time = %s\n", buf);

    // return 0;

    // 不同进程

    int pipefd[2];
    char buf[1024] = {0};
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd) == -1)
    {
        printf("Error, socketpair create failed, errno(%d): %s\n", errno, strerror(errno));
        return EXIT_FAILURE; 
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        printf("Error, fork failed, errno(%d): %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    else if (pid > 0)
    {
        close(pipefd[1]);
        write(pipefd[0], time, strlen(time));
        printf("write success %d\n", getpid());
    }
    else 
    {
        close(pipefd[0]);
        read(pipefd[1], buf, sizeof(buf));
        printf("time = %s %d\n", buf, getpid());
    }
    return 0;
}