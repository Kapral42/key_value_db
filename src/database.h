#pragma once

#include "hashtab.h"

struct mydb_t {
    const char *fname_data;
    const char *fname_mdata;
    size_t size;
    struct hashtab_t *tab;
};

struct mydb_t *mydb_init(const char *fname_data, const char *fname_mdata);
int mydb_list(struct mydb_t *db);
int mydb_put(struct mydb_t *db, const char *key, const char *value);
const char * const mydb_get(struct mydb_t *db, const char *key);
int mydb_erase(struct mydb_t *db, const char *key);
