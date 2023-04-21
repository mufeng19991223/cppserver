#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

pthread_mutex_t mutex;

void* another(void* arg)
{
    printf("int child thread, lock the mutex\n");
    pthread_mutex_lock(&mutex);
    sleep(5);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void prepare()
{
    pthread_mutex_lock(&mutex);
}

void infork()
{
    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_t id;
    pthread_create(&id, NULL, another, NULL);

    // 父进程的主线程暂停1s,以确保在执行fork操作之前,子线程已经开始运行并获取了互斥变量mutex
    sleep(1);

    // 清理互斥锁状态 
    // pthread_atfork(prepare, infork, infork);

    int pid = fork();
    if (pid < 0)
    {
        pthread_join(id, NULL);
        pthread_mutex_destroy(&mutex);
        return -1;
    }
    else if (pid == 0)
    {
        printf("I am in the child, want to get the lock\n");
        // 子进程从父进程继承了互斥锁mutex的状态,该互斥锁处于锁住的状态,这是由父进程中的
        // 子进程执行pthread_mutex_lock引起的,因此,下面这句加锁操作会一直阻塞。
        pthread_mutex_lock(&mutex);
        printf("not run to here\n");
        pthread_mutex_unlock(&mutex);
        exit(0);
    }
    else
    {
        wait(NULL);
    }

    pthread_join(id, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}

