#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtab.h"
#include "hash.h"

struct hashtab_t *hashtab_init(size_t size)
{
    struct hashtab_t*tab;

    if (size < 1) {
        return NULL;
    }

    tab = malloc(sizeof(struct hashtab_t));
    if (tab == NULL) {
        return NULL;
    }

    tab->nodes = (struct hashtab_node**) calloc(size, sizeof(struct hashtab_node*));
    if (tab->nodes == NULL) {
        return NULL;
    }

    tab->inodes = (struct hashtab_inode**) calloc(size, sizeof(struct hashtab_inode*));
    if (tab->inodes == NULL) {
        return NULL;
    }

    tab->tab_size = size;
    tab->count = 0;
    return tab;
}

/* Add new key or change old equal key */
static struct hashtab_node *add_node(struct hashtab_t *tab, const char *key, struct hashtab_inode *inode)
{
    size_t key_len = strlen(key) + 1;
    size_t index = (size_t) my_hash(key, key_len) % tab->tab_size;

    /* Search node with equal key */
    struct hashtab_node *node = tab->nodes[index];
    while (node) {
        if(!strcmp(node->key, key)) {
            break;
        }
        node = node->next;
    }
    /* Modify or create new node */
    if (!node) {
        if ((node = malloc(sizeof(struct hashtab_node))) == NULL ) {
            return NULL;
        }
        node->key = key;
        node->key_offset = 0;
        node->key_size = key_len;
        node->value = inode;
        node->next = tab->nodes[index];
        tab->nodes[index] = node;
        tab->count++;
    } else {
        /* If key already exist in table */
        if (--node->value->links <= 0) {
            //delete_value(inode);
        }
        node->value = inode;
    }
    return node;
}

/* Add new value or merge equal value */
static struct hashtab_inode *add_inode(struct hashtab_t *tab, const char *val)
{
    size_t val_len = strlen(val) + 1;
    size_t index = (size_t) my_hash(val, val_len) % tab->tab_size;

    /* Search inode with equal value */
    struct hashtab_inode *inode = tab->inodes[index];
    while (inode) {
        if(!strcmp(inode->value, val)) {
            break;
        }
        inode = inode->next;
    }
    /* Modify or create new inode */
    if (!inode) {
        if ((inode = malloc(sizeof(struct hashtab_inode))) == NULL ) {
            return NULL;
        }
        inode->value = val;
        inode->offset = 0;
        inode->links = 1;
        inode->size = val_len;
        inode->next = tab->inodes[index];
        tab->inodes[index] = inode;
    } else {
        /* If val already exist in table */
        free(val);
        inode->links++;
    }
    return inode;
}


struct hashtab_node * const hashtab_push(struct hashtab_t *tab, const char *key, const char *val)
{
    struct hashtab_node *tmp_node;
    int i;

    struct hashtab_inode *inode = add_inode(tab, val);
    if (!inode) {
        return NULL;
    }

    struct hashtab_node *node = add_node(tab, key, inode);
    if (!node) {
        return NULL;
    }

    return node;
}

int hashtab_print_keys(struct hashtab_t *tab)
{
    size_t count = 0;

    for (size_t i = 0; i < tab->tab_size; i++) {
        struct hashtab_node *node = tab->nodes[i];
        while (node) {
            count++;
            printf("%ld. \"%s\"\n", count, node->key);
            node = node->next;
        }
    }
    return (int) count;
}

char const * const hashtab_get_value(struct hashtab_t *tab, const char* key)
{
    size_t key_len = strlen(key) + 1;
    size_t index = (size_t) my_hash(key, key_len) % tab->tab_size;

    /* Search node with equal key */
    struct hashtab_node *node = tab->nodes[index];
    while (node) {
        if(!strcmp(node->key, key))
            break;
        node = node->next;
    }

    if (!node)
        return NULL;

    return node->value->value;
}

long int hashtab_get_index(struct hashtab_t*tab, long int val)
{
/*    long int i;
    struct hashtab_node *tmp_node;
    struct hashtab_node **tab = tab->nodes;

    i = val % tab->size;
    tmp_node = tab[i];
    while(tmp_node != NULL) {
        if (tmp_node->val == val) {
            return tmp_node->index;
        }
        tmp_node = tmp_node->next;
    }*/
    return -1;
}

int hashtab_free(struct hashtab_t*tab)
{
/*    long int i;
    struct hashtab_node *tmp_node;
    struct hashtab_node **tab = tab->nodes;

    for (i = 0; i < tab->size; i++) {
        while(tab[i] != NULL) {
            tmp_node = tab[i];
            tab[i] = tab[i]->next;
            free(tmp_node);
        }
    }
    free(tab);
    free(tab);*/
    return 0;
}
