#pragma once

/* Invert node, need for follow deduplication */
struct hashtab_inode {
    char *value;
    size_t offset;
    size_t links;
    size_t size;
    struct hashtab_inode *next;
};

/* Hashtable node, value contain in to inode */
struct hashtab_node {
    char *key;
    size_t key_offset;
    size_t key_size;
    struct hashtab_inode *value;
    struct hashtab_node *next;
    unsigned char del;
};

struct hashtab_t {
    struct hashtab_node **nodes;
    struct hashtab_inode **inodes;
    size_t tab_size;
    size_t count;
    size_t del_count;
};

struct hashtab_t *hashtab_init(size_t n);
struct hashtab_node * const hashtab_push(struct hashtab_t *tab, const char *key, const char *val);
char const * const hashtab_get_value(struct hashtab_t *tab, const char* key);
int hashtab_lazy_delete(struct hashtab_t *tab, const char *key);
void hashtab_free(struct hashtab_t *tab);
int hashtab_print_keys(struct hashtab_t *tab);
void hashtab_real_delete(struct hashtab_t *tab);
