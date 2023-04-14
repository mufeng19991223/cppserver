#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
typedef void (*sighandler_t)(int);

void sigroutine(int dunno)
{ /* 信号处理例程，其中dunno将会得到信号的值 */
 
    switch (dunno)
    {
        case SIGUSR1:
        {
            std::cout << "SIGUSR1" << std::endl;
            break;
        }
        case SIGUSR2:
        {
            std::cout << "SIGUSR2" << std::endl;
            break;
        }
    }
    return;
}


void printsigset(const sigset_t *set)
{
  for (int i = 1; i <= 64; i++) {
    if (i==33) putchar(' ');
    if (sigismember(set, i) == 1)
      putchar('1');
    else
      putchar('0');
  }
  puts("");
}

void handler(int signo)
{
    std::cout << signo << "我是你大爷..." << std::endl;
    sleep(2);
    std::cout << "-----end-------" << std::endl;
    return;
}

void handler1(int sig) {
  if (sig == SIGTSTP) printf("hello SIGTSTP\n");  
  if (sig == SIGINT) printf("hello SIGINT\n");  
  sleep(2);
  sigset_t st; 
  sigpending(&st);
  printsigset(&st);
}

int main() 
{
    // sighandler_t handler;
    // handler = signal(SIGHUP, sigroutine);
    // handler = signal(SIGINT, sigroutine);
    // handler = signal(SIGQUIT, sigroutine);
    // if (handler == SIG_ERR)
    // {
    //     perror("err");
    //     return -1;
    // }
    // while(1) {}


    // 信号捕捉
    // struct sigaction sig;
    // sig.sa_handler = sigroutine;
    // sig.sa_flags = 0;
    // sigemptyset(&sig.sa_mask);
    // sigaddset(&sig.sa_mask, SIGINT);
    // sigaddset(&sig.sa_mask, SIGQUIT);
    // int ret = sigaction(SIGINT, &sig, NULL);
    // while (1) {}
    
    // SA_RESTART
    
    // char buf[1024];
    // int n;
    // struct sigaction sig;
    // sig.sa_handler = sigroutine;
    // sig.sa_flags = SA_RESTART;
    // sigaction(SIGUSR1, &sig, NULL);
    // sigaction(SIGUSR2, &sig, NULL);

    // printf("pid = %d", getpid());

    // while (1)
    // {
    //     if (n = read(STDIN_FILENO, buf, 1024) == -1)
    //     {
    //         if (errno == EINTR)
    //         {
    //             printf("read is interrupted by signal\n");
    //         }
    //     }
    //     else
    //     {
    //         buf[n] = '\0';
    //         printf("%d bytes read %s\n", n, buf);
    //     }
    // }



    // 阻塞
    printf("I'm %d\n", getpid());
    struct sigaction act, oldact;
    act.sa_handler = handler1; // 设置普通信号处理函数

    // 向 sa_mask 中添加 SIGINT
    sigemptyset(&act.sa_mask); 
    sigaddset(&act.sa_mask, SIGINT);
    act.sa_flags = 0; // 先置 0

    sigaction(SIGTSTP, &act, &oldact);
    sigaction(SIGINT, &act, &oldact);

    while(1) 
    {
        write(STDOUT_FILENO, ".", 1); 
        pause();
    }

    return 0;
}