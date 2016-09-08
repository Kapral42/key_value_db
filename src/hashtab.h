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
};

struct hashtab_t {
    struct hashtab_node **nodes;
    struct hashtab_inode **inodes;
    size_t tab_size;
    size_t count;
};

struct hashtab_t *hashtab_init(size_t n);
struct hashtab_node * const hashtab_push(struct hashtab_t *tab, const char *key, const char *val);
char const * const hashtab_get_value(struct hashtab_t *tab, const char* key);
long int hashtab_get_index(struct hashtab_t *h_tab, long int val);
int hashtab_free(struct hashtab_t *h_tab);
int hashtab_print_keys(struct hashtab_t *tab);
