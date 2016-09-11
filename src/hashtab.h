#pragma once

/* Invert node, need for follow deduplication */
struct hashtab_inode {
    char *value;

    /* Offset in the file */
    size_t offset;

    /* Count of keys contain it value */
    size_t links;

    size_t size;
    struct hashtab_inode *next;
};

/* Hashtable node, value contain in to inode */
struct hashtab_node {
    char *key;

    /* Offset in the file */
    size_t key_offset;

    size_t key_size;
    struct hashtab_inode *value;
    struct hashtab_node *next;
    unsigned char del;
};

/* Hashtable structure */
struct hashtab_t {
    struct hashtab_node **nodes;
    struct hashtab_inode **inodes;

    /* Size of nodes and inodes arrays */
    size_t tab_size;

    /* Count of living nodes in the table */
    size_t count;

    /* Count of deleted nodes in the table */
    size_t del_count;

    /* Count of all values */
    size_t val_count;
};

struct hashtab_t *hashtab_init(size_t n);
struct hashtab_node * const hashtab_push(struct hashtab_t *tab, char *key, char *val);
char const * const hashtab_get_value(struct hashtab_t *tab, const char* key);
int hashtab_lazy_delete(struct hashtab_t *tab, const char *key);
void hashtab_free(struct hashtab_t *tab);
const char **hashtab_list(struct hashtab_t *tab, size_t *count);
void hashtab_real_delete(struct hashtab_t *tab);
