#pragma once

#include "hashtab.h"
#include "io.h"

struct mydb_t {
    char *fname_data;
    char *fname_mdata;
    size_t size;
    struct hashtab_t *tab;
    struct io_fd *fd;
};

struct mydb_t *mydb_init(const char *fname_data, const char *fname_mdata, int extract);
const char **mydb_list(struct mydb_t *db, size_t *count);
int mydb_put(struct mydb_t *db, const char *key, const char *value);
const char *mydb_get(struct mydb_t *db, const char *key);
int mydb_erase(struct mydb_t *db, const char *key);
int mydb_extract(struct mydb_t *db);
int mydb_save_mdata(struct mydb_t *db, int rewrite_dat);
void mydb_close(struct mydb_t *db);
