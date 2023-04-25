#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";

int main(int argc, char* argv[])
{
    // demo1
    // int fd = open(argv[1], O_RDWR | O_CREAT);
    // write(fd, "hello...", 10);
    
    // lseek(fd, 4, SEEK_SET);
    // char buf[10];
    // memset(buf, 0, sizeof(buf));
    // int m_read = read(fd, buf, sizeof(buf));

    // write(STDOUT_FILENO, buf, m_read);

    // demo2

    int fd;
    if ((fd = creat("lseek1.txt", S_IRWXU)) < 0) 
    {
        printf("creat error\n");
    }

    if (write(fd, buf1, 10) != 10)
    {
        printf("buf1 write error\n");
    }

    if (lseek(fd, 16384, SEEK_SET) == -1)
    {
        printf("lseek error\n");
    }

    if (write(fd, buf2, 10) != 10)
    {
        printf("buf1 write error\n");
    }

    // close(fd);

}