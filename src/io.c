#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "io.h"

/* Init file descriptors for DB */
struct io_fd *io_fd_init(const char *fname_data, const char *fname_mdata)
{
    struct io_fd *fd = malloc(sizeof(struct io_fd));
    if (!fd)
        return NULL;

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

    /* If flushing error then try two more attempt */
    int i;
    for (i = 0; i < 3 && fflush(fd) == EOF; i++);

    return i == 3 || !count ? -1 : offset;
}

int io_read(FILE *fd, char *buff, const size_t size)
{
    return fread(buff, 1, size, fd);
}

/* Read from any offset in the file */
int io_offset_read(FILE *fd, char *buff, const size_t size, const size_t offset)
{
    fseek(fd, offset, SEEK_SET);
    return fread(buff, 1, size, fd);
}

/* Clear file */
void io_file_clear(FILE *fd, const char *f_name)
{
    freopen(f_name, "w+", fd);
}
