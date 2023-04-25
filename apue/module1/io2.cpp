#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

const int MAX_LEN = 1024;

int main()
{
    char buf[1024];
    pid_t pid;
    int status;

    printf("%% ");
    while (fgets(buf, MAX_LEN, stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
        {
            buf[strlen(buf) - 1] = 0;
        }
        if ((pid = fork()) < 0)
        {
            printf("fork error\n");
        }
        else if (pid == 0)
        {
            execlp(buf, buf, (char*)0);    
            printf("child\n");
            exit(127);
        }

        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            printf("wait error\n");
        }

        printf("%% ");
    }

    return 0;

}