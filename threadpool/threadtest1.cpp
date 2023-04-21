#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct node_s
{
    int data;
    struct node_s *next;
} node_t;

node_t *head = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void cleanup_handler(void *arg)
{
    printf("cleanup_handler is running.\n");
    free(arg);
    pthread_mutex_unlock(&mutex);
}

void *thread_func(void *arg)
{
    node_t *p = NULL;
    pthread_cleanup_push(cleanup_handler, p);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (NULL == head)
            pthread_cond_wait(&condition, &mutex);
        p = head;
        head = head->next;
        printf("process %d node.\n", p->data);
        free(p);
        pthread_mutex_unlock(&mutex);
    }
    pthread_cleanup_pop(0);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    node_t *temp = NULL;
    int i;
    pthread_create(&tid, NULL, thread_func, NULL);
    for (i = 0; i < 5; i++)
    {
        temp = (node_t *)malloc(sizeof(node_t));
        pthread_mutex_lock(&mutex);
        temp->data = i;
        temp->next = head;
        head = temp;
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    return 0;
}