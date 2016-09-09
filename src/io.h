#pragma once

struct io_fd {
    int f_data;
    int f_mdata;
};


struct io_fd *io_fd_init(const char *fname_data, const char *fname_mdata);
void io_close(struct io_fd *fd);
int io_write(int fd, const char *buff, const size_t size);
int io_read(int fd, char *buff, const size_t size);
int io_offset_read(int fd, char *buff, const size_t size, const size_t offset);

