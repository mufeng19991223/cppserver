#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

int main()
{
    int fd = open("mmap.txt", O_RDWR | O_CREAT, 0777);
    assert(fd != -1);
    truncate("mmap.txt", 1024);

    char* addr = (char*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(addr != MAP_FAILED);

    for (int i = 0; i < 10; ++i)
    {
        snprintf(addr, 1024, "I am number %d", i);
        sleep(1);
    }

    munmap(addr, 1024);
    return 0;

}