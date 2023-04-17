#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <assert.h>
#include <iostream>

int main()
{
    int fd = open("mmap.txt", O_RDONLY);
    assert(fd != -1);

    char* addr = (char*)mmap(NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);
    assert(addr != MAP_FAILED);

    while (1)
    {
        sleep(1);
        std::cout << addr << std::endl;
    }

    munmap(addr, 1024);

    return 0;
}