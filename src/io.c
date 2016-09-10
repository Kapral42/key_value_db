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

    /* If files not created */
 /*   fd->f_data = open(fname_data, O_CREAT, S_IRWXU);
    fd->f_mdata = open(fname_mdata, O_CREAT, S_IRWXU);
    close(fd->f_data);
    close(fd->f_mdata);
*/
    fd->f_data = fopen(fname_data, "a+");
    fd->f_mdata = fopen(fname_mdata, "a+");
    if (!fd->f_data || !fd->f_mdata) {
        free(fd);
        return NULL;
    }

    return fd;
}

void io_close(struct io_fd *fd)
{
    fclose(fd->f_data);
    fclose(fd->f_mdata);
    free(fd);
}

/* Write with flushing buffer */
int io_write(FILE *fd, const char *buff, const size_t size)
{
    fseek(fd, 0, SEEK_END);
    int offset = ftell(fd);
    int count = fwrite(buff, 1, size, fd);
    int i;
    /* If flushing error then try two more attempt */
    for (i = 0; i < 3 && fflush(fd) == EOF; i++);
    return i == 3 || !count ? -1 : offset;
}

int io_read(FILE *fd, char *buff, const size_t size)
{
    return fread(buff, 1, size, fd);
}

int io_offset_read(FILE *fd, char *buff, const size_t size, const size_t offset)
{
    fseek(fd, offset, SEEK_SET);
    return fread(buff, 1, size, fd);
}

void io_file_clear(FILE *fd, const char *f_name)
{
    freopen(f_name, "w+", fd);
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
