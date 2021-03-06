#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtab.h"
#include "hash.h"

/* Creation new hashtable, allocation of tables and filling fields */
struct hashtab_t *hashtab_init(const size_t size)
{
    struct hashtab_t*tab;

    if (size < 1) {
        return NULL;
    }

    tab = malloc(sizeof(struct hashtab_t));
    if (!tab) {
        return NULL;
    }

    tab->nodes = (struct hashtab_node**) calloc(size,
                            sizeof(struct hashtab_node*));

    if (!tab->nodes) {
        return NULL;
    }

    tab->inodes = (struct hashtab_inode**) calloc(size,
                            sizeof(struct hashtab_inode*));

    if (!tab->inodes) {
        return NULL;
    }

    tab->tab_size = size;
    tab->count = 0;
    tab->del_count = 0;
    tab->val_count = 0;

    return tab;
}

/* Search of node in the hashtable */
static struct hashtab_node *get_node(struct hashtab_t *tab, const char *key,
        size_t key_len, size_t *i)
{

    size_t index = (size_t) my_hash(key, key_len) % tab->tab_size;
    *i = index;

    /* Search node with equal key */
    struct hashtab_node *node = tab->nodes[index];
    while (node) {
        if(!strcmp(node->key, key)) {
            break;
        }
        node = node->next;
    }

    return node;
}

/* Add new key or change old equal key */
static struct hashtab_node *add_node(struct hashtab_t *tab,
        char *key, struct hashtab_inode *inode)
{
    size_t key_len = strlen(key) + 1;
    size_t index;
    struct hashtab_node *node = get_node(tab, key, key_len, &index);

    /* Modify or create new node */
    if (!node) {
        /* New node */
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
            /* If value links = 0 it's means "lazy delete" */
            /* Value will be deleted on a real_delete stage */
            //delete_value(inode);
        }
        node->value = inode;
        /* If node already exist but was removed */
        tab->del_count -= node->del ? 1 : 0;
        node->del = 0;
    }
    return node;
}

/* Add new value or merge equal value */
static struct hashtab_inode *add_inode(struct hashtab_t *tab, char *val)
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
        tab->val_count++;
    } else {
        /* If val already exist in table */
        free(val);
        inode->links++;
    }
    return inode;
}

/* Push to table */
struct hashtab_node * const hashtab_push(struct hashtab_t *tab,
                            char *key, char *val)
{
    /* Add or merge value */
    struct hashtab_inode *inode = add_inode(tab, val);
    if (!inode) {
        return NULL;
    }

    /* Add or merge key */
    struct hashtab_node *node = add_node(tab, key, inode);
    if (!node) {
        return NULL;
    }

    return node;
}

/* Returns const list of all keys */
const char **hashtab_list(struct hashtab_t *tab, size_t *n)
{
    size_t count = 0;
    const char **list;

    if (!(list = malloc(tab->count * sizeof(char*))))
        return NULL;

    for (size_t i = 0; i < tab->tab_size; i++) {
        struct hashtab_node *node = tab->nodes[i];
        while (node) {
            if (!node->del) {
                list[count] = node->key;
                count++;
            }
            node = node->next;
        }
    }

    *n = count;
    return list;
}

/* Returns const pointer to value string */
char const * const hashtab_get_value(struct hashtab_t *tab, const char* key)
{
    size_t key_len = strlen(key) + 1;
    size_t index;
    struct hashtab_node *node = get_node(tab, key, key_len, &index);

    if (!node || node->del)
        return NULL;

    return node->value->value;
}

/* Mark the node like deleted */
int hashtab_lazy_delete(struct hashtab_t *tab, const char *key)
{
    size_t key_len = strlen(key) + 1;
    size_t index;
    struct hashtab_node *node = get_node(tab, key, key_len, &index);

    if (!node)
        return 1;

    if (node->del == 0) {
        node->del = 1;
        tab->del_count++;
        tab->count--;
    }
    return 0;
}

/* Delete all "trash" nodes and values */
void hashtab_real_delete(struct hashtab_t *tab)
{
    //TODO: integrate rewrite file
    /* Delete nodes */
    struct hashtab_node *old_node;
    struct hashtab_node *node;
    struct hashtab_node *del_node;
    int del_flg;

    for (size_t i = 0; i < tab->tab_size; i++) {

        old_node = NULL;
        node = tab->nodes[i];

        while (node) {
            /* free each deleted nodes */
            del_flg = 0;
            if (node->del) {
                if (old_node) {
                    old_node->next = node->next;
                    del_node = node;
                    node = old_node;
                } else {
                    tab->nodes[i] = node->next;
                    del_node = node;
                    node = tab->nodes[i];
                }

                del_node->value->links--;
                free(del_node->key);
                free(del_node);
                tab->del_count--;
                del_flg++;
            }

            if (del_flg && node == tab->nodes[i]) {
                old_node = NULL;
            } else {
                old_node = node;
                node = node->next;
            }
        }
    }

    /* Delete inodes */
    struct hashtab_inode *old_inode;
    struct hashtab_inode *inode;
    struct hashtab_inode *del_inode;

    for (size_t i = 0; i < tab->tab_size; i++) {
        old_inode = NULL;
        inode = tab->inodes[i];
        while (inode) {
            /* free each deleted inodes */
            del_flg = 0;
            if (inode->links <= 0) {
                if (old_inode) {
                    old_inode->next = inode->next;
                    del_inode = inode;
                    inode = old_inode;
                } else {
                    tab->inodes[i] = inode->next;
                    del_inode = inode;
                    inode = tab->inodes[i];
                }

                free(del_inode->value);
                free(del_inode);
                del_flg++;
            }

            if (del_flg && inode == tab->inodes[i]) {
                old_inode = NULL;
            } else {
                old_inode = inode;
                inode = inode->next;
            }
        }
    }
}

/* free hashtable struct and all nodes and inodes */
void hashtab_free(struct hashtab_t *tab)
{
    /* free nodes */
    struct hashtab_node *node;
    struct hashtab_node *free_node;

    for (size_t i = 0; i < tab->tab_size; i++) {
        node = tab->nodes[i];
        while (node) {
            free_node = node;
            node = node->next;
            free(free_node->key);
            free(free_node);
        }
    }

    /* free inodes */
    struct hashtab_inode *inode;
    struct hashtab_inode *free_inode;

    for (size_t i = 0; i < tab->tab_size; i++) {
        inode = tab->inodes[i];
        while (inode) {
            free_inode = inode;
            inode = inode->next;
            free(free_inode->value);
            free(free_inode);
        }
    }

    /* free tab */
    free(tab->nodes);
    free(tab->inodes);
    free(tab);
}
