#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int val;
    if ( (val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
    {
        printf("fcntl error\n");
    }

    switch(val & O_ACCMODE)
    {
        case O_RDONLY:
        {
            printf("read only\n");
            break;
        }
        case O_WRONLY:
        {
            printf("write only\n");
            break;
        }   
        case O_RDWR:
        {
            printf("read write\n");
            break;
        }
        default:
        {
            printf("unknown access mode\n");
        }
    }

    if (val& O_APPEND)
    {
        printf(", append");
    }
    if (val& O_NONBLOCK)
    {
        printf(", nonblocking");
    }
    if (val& O_SYNC)
    {
        printf(", synchronous writes");
    }
    #if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
        if (val & O_FSYNC)
            printf(", suschronous writes");
    #endif

    putchar('\n');

    return 0;
}