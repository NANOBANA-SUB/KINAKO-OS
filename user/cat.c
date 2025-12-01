#include "user.h"

int cat()
{
    int fd = open("/hello", 0);
    if (fd < 0)
    {
        puts("open failed\n");
        return -1;
    }

    char buf[64];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0)
    {
        puts("read failed\n");
        close(fd);
        return -1;
    }
    buf[n] = '\0';

    puts(buf);

    close(fd);
    return 0;
}