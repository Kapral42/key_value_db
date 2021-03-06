#pragma once

struct io_fd {
    FILE *f_data;
    FILE *f_mdata;
};


struct io_fd *io_fd_init(const char *fname_data, const char *fname_mdata);
void io_close(struct io_fd *fd);
int io_write(FILE *fd, const char *buff, const size_t size);
int io_read(FILE *fd, char *buff, const size_t size);
void io_file_clear(FILE *fd, const char *f_name);
int io_offset_read(FILE *fd, char *buff, const size_t size, const size_t offset);

