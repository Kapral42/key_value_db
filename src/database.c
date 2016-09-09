#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "names.h"
#include "database.h"
#include "hashtab.h"

struct mydb_t *mydb_init(const char *fname_data, const char *fname_mdata)
{
    struct mydb_t *db = malloc(sizeof(struct mydb_t));
    if (!db) {
        return NULL;
    }

    db->fname_data = fname_data;
    db->fname_mdata = fname_mdata;
    db->size = 0;

    db->fd = io_fd_init(fname_data, fname_mdata);
    if (!db->fd) {
        free(db);
        return NULL;
    }

    db->tab = hashtab_init(100);
    if (!db->tab) {
        free(db);
        return NULL;
    }

    return db;
}

int mydb_list(struct mydb_t* db)
{
    return hashtab_print_keys(db->tab);
}

int mydb_put(struct mydb_t *db, const char *key, const char *value)
{
    if (!key || !value) {
        return 1;
    }

    /* Copy strings for safety and assurance of allocation in the heap */
    char *key_copy = (char*) malloc(sizeof(char) * (strlen(key) + 1));
    char *value_copy = (char*) malloc(sizeof(char) * (strlen(value) + 1));
    if (!key_copy || !value_copy) {
        free(key_copy);
        free(value_copy);
        return 1;
    }
    strcpy(key_copy, key);
    strcpy(value_copy, value);

    struct hashtab_node *node = hashtab_push(db->tab, key_copy, value_copy);

    //TODO: write to file
    int offset;
    offset = io_write(db->fd->f_data, node->key, node->key_size);
    printf("offset %s %d\n", node->key, offset);
    if (offset < 0) {
        //TODO: we can't just exit
        return 1;
    }
    node->key_offset = offset;

    if (node->value->links == 1) {
        offset = io_write(db->fd->f_data, node->value->value,
                            node->value->size);
        printf("offset %s %d\n", node->value->value, offset);
        if (offset < 0) {
            //TODO: we can't just exit
            return 1;
        }
        node->value->offset = offset;
    }

    return 0;
}

char const * const mydb_get(struct mydb_t *db, const char *key)
{
    return hashtab_get_value(db->tab, key);
}

int mydb_erase(struct mydb_t *db, const char *key)
{
    hashtab_lazy_delete(db->tab, key);
    if (db->tab->del_count > MAX_DELETED_ELEM) {
        hashtab_real_delete(db->tab);
        //TODO: rewrite files
    }
    return 0;
}

void mydb_close(struct mydb_t *db)
{
    //TODO:save mdata and free everything
    

    hashtab_free(db->tab);
}

#if 1
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
int main(int argc, const char *argv[])
{
    struct mydb_t *db = mydb_init(FILE_DATA, FILE_MDATA);
    if (!db) {
        printf("DB not created\n");
        return 1;
    }
/*    char *key = "123";
    char *value = "456";
    if (!mydb_put(db, key, value)) {
        printf("PUT SUCCESS\n");
    }
    char *key1 = "789";
    char *value1 = "456";
    if (!mydb_put(db, key1, value1)) {
        printf("PUT SUCCESS\n");
    } */
    char *Keys[] = {"01", "02", "03", "04",
                   "05", "06", "07", "08"};
    char *Values[] = {"o1", "o2", "o3", "o4",
                     "o5", "o6", "o7", "o8"};
    for (int i = 0; i < 8; i++) {
        if (!mydb_put(db, Keys[i], Values[i])) {
            printf("%d PUT SUCCESS\n", i);
        }
    }
    mydb_list(db);
    mydb_erase(db, Keys[1]);
    mydb_erase(db, Keys[2]);
    mydb_erase(db, Keys[3]);
    printf("\n");
    hashtab_real_delete(db->tab);
    mydb_list(db);

  //  printf("get val (key:%s) %s\n", key, mydb_get(db, key));
    return 0;
}
#endif
