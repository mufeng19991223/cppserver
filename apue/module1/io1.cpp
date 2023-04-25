#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int c;

    printf("--------------\n");
    
    while ((c = getc(stdin) != EOF))
    {
        if (putc(c, stdout) == EOF)
        {
            printf("output error\n");
        }
    }

    printf("--------------\n");

    if (ferror(stdin))
    {
        printf("input error");
    }

    exit(0);
}