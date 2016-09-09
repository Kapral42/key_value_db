#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "io.h"

struct io_fd *io_fd_init(const char *fname_data, const char *fname_mdata)
{
    struct io_fd *fd = malloc(sizeof(struct io_fd));
    if (!fd)
        return NULL;

    fd->f_data = open(fname_data, O_RDWR);
    fd->f_mdata = open(fname_mdata, O_RDWR);
    if (fd->f_data < 0 || fd->f_mdata < 0) {
        free(fd);
        return NULL;
    }

    return fd;
}

void io_close(struct io_fd *fd)
{
    close(fd->f_data);
    close(fd->f_mdata);
    free(fd);
}

/* Write with flushing buffer */
int io_write(int fd, const char *buff, const size_t size)
{
    int count = write(fd, buff, size);
    int i;
    /* If flushing error then try two more attempt */
    for (i = 0; i < 3 && fsync(fd) < 0; i++);
    return i == 3 ? -2 : count;
}

int io_read(int fd, char *buff, const size_t size)
{
    return read(fd, buff, size);
}

int io_offset_read(int fd, char *buff, const size_t size, const size_t offset)
{
    lseek(fd, SEEK_SET, offset);
    return read(fd, buff, size);
}

#if 0
int main(int argc, const char *argv[])
{
    struct io_fd fd;
    //fd.str_read = open("str.db", "r");
    //fd.str_write = open("str.db", "")
    int file = open("str.db", O_RDWR);

#pragma omp parallel nthread(4)
{
 //   int thr_num
}

    close(file);
    return 0;
}
#endif
