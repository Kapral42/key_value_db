#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "names.h"
#include "database.h"
#include "hashtab.h"

struct mydb_t *mydb_init(const char *fname_data, const char *fname_mdata, int extract)
{
    struct mydb_t *db = malloc(sizeof(struct mydb_t));
    if (!db)
        return NULL;

    db->fname_data = fname_data;
    db->fname_mdata = fname_mdata;
    db->size = 0;

    db->fd = io_fd_init(fname_data, fname_mdata);
    if (!db->fd) {
        free(db);
        return NULL;
    }

    if (extract && !access(fname_mdata, F_OK)
            && !access(fname_data, F_OK)) {
        mydb_extract(db);
        return db;
    }

    db->tab = hashtab_init(HASHTAB_SIZE);
    if (!db->tab) {
        free(db);
        return NULL;
    }

    return db;
}

char const **mydb_list(struct mydb_t* db, size_t *count)
{
    return hashtab_list(db->tab, count);
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

    size_t key_count = db->tab->count;
    size_t val_count = db->tab->val_count;

    struct hashtab_node *node = hashtab_push(db->tab, key_copy, value_copy);

    /* Write to file */
    int offset;
    if (key_count < db->tab->count) {
        offset = io_write(db->fd->f_data, node->key, node->key_size);
        printf("offset %s %d\n", node->key, offset);
        if (offset < 0) {
            //TODO: we can't just exit
            return 1;
        }
        node->key_offset = offset;
    }

    if (val_count < db->tab->val_count) {
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

char const *mydb_get(struct mydb_t *db, const char *key)
{
    return hashtab_get_value(db->tab, key);
}

int mydb_erase(struct mydb_t *db, const char *key)
{
    hashtab_lazy_delete(db->tab, key);
    if (db->tab->del_count > MAX_DELETED_ELEM) {
        hashtab_real_delete(db->tab);
    }
    return 0;
}

int mydb_save_mdata(struct mydb_t *db, int rewrite_dat)
{
    struct hashtab_t *tab = db->tab;
    FILE *mdata = db->fd->f_mdata;
    FILE *data = db->fd->f_data;
    int size = sizeof(size_t);

    io_file_clear(mdata, db->fname_mdata);

    if (rewrite_dat) {
        /* Clear data file */
        io_file_clear(data, db->fname_data);

        /* Rewrite all values */
        struct hashtab_inode *inode;
        for (size_t i = 0; i < tab->tab_size; i++) {
            inode = tab->inodes[i];
            while (inode) {
                inode->offset = io_write(data, inode->value, inode->size);
                inode = inode->next;
            }
        }
    }

    /* Save count of bytes in a number */
    unsigned char elem_size = (char) size;
    fwrite(&elem_size, 1, 1, mdata);

    /* Save count of hashtable elemrnts */
    fwrite(&tab->count, size, 1, mdata);

    /* Save offsets and lens of keys and values */
    struct hashtab_node *node;
    for (size_t i = 0; i < tab->tab_size; i++) {
        node = tab->nodes[i];
        while (node) {
            if (rewrite_dat) {
                node->key_offset = io_write(data, node->key, node->key_size);
            }
            fwrite(&node->key_offset, size, 1, mdata);
            fwrite(&node->key_size, size, 1, mdata);
            fwrite(&node->value->offset, size, 1, mdata);
            fwrite(&node->value->size, size, 1, mdata);
            node = node->next;
        }
    }
    fflush(mdata);
    return 0;
}

int mydb_extract(struct mydb_t *db)
{
    FILE *data = db->fd->f_data;
    FILE *mdata = db->fd->f_mdata;

    int elem_size = 0;
    fread(&elem_size, 1, 1, mdata);

    size_t count = 0;
    fread(&count, elem_size, 1, mdata);

    db->tab = hashtab_init(HASHTAB_SIZE);
    if (!db->tab) {
        return 1;
    }

    size_t key_offset, key_size, val_offset, val_size;
    for (size_t i = 0; i < count; i++) {
        fread(&key_offset, elem_size, 1, mdata);
        fread(&key_size, elem_size, 1, mdata);
        fread(&val_offset, elem_size, 1, mdata);
        fread(&val_size, elem_size, 1, mdata);

        char *key = (char*) malloc(sizeof(char) * key_size);
        char *value = (char*) malloc(sizeof(char) * val_size);
        if (!key || !value) {
            free(key);
            free(value);
            return 1;
        }
        if (io_offset_read(data, key, key_size, key_offset) <= 0 ||
            io_offset_read(data, value, val_size, val_offset) <= 0) {
            free(key);
            free(value);
            return 1;
        }

        hashtab_push(db->tab, key, value);
    }

    return db->tab->count == count ? 0 : 1;
}


void mydb_close(struct mydb_t *db)
{
    /* Save mdata and free everything */
    int rw_flg = 0;
    if (db->tab->del_count) {
        hashtab_real_delete(db->tab);
        rw_flg = 1;
    }
    mydb_save_mdata(db, rw_flg);
    io_close(db->fd);
    hashtab_free(db->tab);
    //free(db->fname_data);
    //free(db->fname_mdata);
    free(db);
}

#if 0
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
int main(int argc, const char *argv[])
{
    struct mydb_t *db = mydb_init(FILE_DATA, FILE_MDATA, 1);
    if (!db) {
        printf("DB not created\n");
        return 1;
    }
    printf("List after init:\n");
    mydb_list(db);
    printf("---\n");
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
    //hashtab_real_delete(db->tab);
    mydb_list(db);

  //  printf("get val (key:%s) %s\n", key, mydb_get(db, key));
    mydb_close(db);
    return 0;
}
#endif
