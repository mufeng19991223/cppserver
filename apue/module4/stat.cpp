#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    struct stat buf;
    if (lstat(argv[1], &buf) < 0)
    {
        printf("lstat error\n");
    }

    if (S_ISREG(buf.st_mode))
    {
        printf("regular\n");
    }
    else if (S_ISDIR(buf.st_mode))
    {
        printf("directory\n");
    }
    else if (S_ISCHR(buf.st_mode))
    {
        printf("character sepcial\n");
    }
    else if (S_ISBLK(buf.st_mode))
    {
        printf("block sepcial\n");
    }
    else if (S_ISFIFO(buf.st_mode))
    {
        printf("FIFO\n");
    }
    else if (S_ISLNK(buf.st_mode))
    {
        printf("symbolic linke\n");
    }
    else if (S_ISSOCK(buf.st_mode))
    {
        printf("socket\n");
    }
    else
        printf("noknown\n");

    return 0;
}