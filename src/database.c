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
    return 0;
}

char const * const mydb_get(struct mydb_t *db, const char *key)
{
    return hashtab_get_value(db->tab, key);
}

int mydb_erase(struct mydb_t *db, const char *key)
{
    return 0;
}

void mydb_free(struct mydb_t *db)
{

}

#if 0
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
int main(int argc, const char *argv[])
{
    struct mydb_t *db = mydb_init("1", "2");
    char *key = "123";
    char *value = "456";
    mydb_put(db, key, value);
    char *key1 = "789";
    char *value1 = "456";
    mydb_put(db, key1, value1);
    mydb_list(db);

    printf("get val (key:%s) %s\n", key, mydb_get(db, key));
    printf("SSIZE_MAX %d\n", SSIZE_MAX);
    return 0;
}
#endif
