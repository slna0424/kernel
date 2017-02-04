/*
 * vos_tokenblk.h
 *
 *  Created on: 2014-1-27
 *      Author: kevin
 */


#ifndef VOS_TOKEN_BLOCK_H_
#define VOS_TOKEN_BLOCK_H_

#include "vos_types.h"
#include "vos_list.h"
#include "vos_jhash.h"
#include "vos_metablk.h"
#include "vos_pool_m.h"

typedef struct _vos_tokenblk
{
    vos_metablk              meta;
    vos_i32                  type;
    vos_str                  name;
    vos_str                 value;
    struct _vos_tokenblk    *left;
    struct _vos_tokenblk   *right;
    struct _vos_tokenblk    *next;
    struct hlist_node       hlist;
} vos_tokenblk;

#define VOS_TOKENBLK_LEN           sizeof(vos_tokenblk)

#define HASH_SIZE               32

#ifndef DEBUG
#define MAX_TOKEN_NUM           256*1024
#else
#define MAX_TOKEN_NUM           256
#endif

typedef struct
{
    vos_pool_m                pool_m;
    struct hlist_head tab[HASH_SIZE];       // HASH
    vos_tokenblk               *root;
    vos_i32                   status;
} vos_tokenblk_m;

static inline void * token_init(void *p)
{
    if (NULL == p) return NULL;

    vos_tokenblk *pnode = (vos_tokenblk *)p;
    pnode->name = NULL;
    pnode->value = NULL;
    pnode->type = -1;
    pnode->left = NULL;
    pnode->right = NULL;
    pnode->next = NULL;
    INIT_HLIST_NODE(&pnode->hlist);
    return p;
}

static inline void token_fini(void *p)
{
    if (NULL == p) return;
    vos_tokenblk *pnode = (vos_tokenblk *)p;
    if (NULL != pnode->name) free(pnode->name);
    if (NULL != pnode->value) free(pnode->value);
    pnode->name = NULL;
    pnode->value = NULL;
    pnode->type = -1;
    pnode->left = NULL;
    pnode->right = NULL;
    pnode->next = NULL;
}

static inline vos_i32 token_m_init(void *p)
{
    if (NULL == p) return 0;

    if (VOS_OK != pool_init(p)) return VOS_ERROR;

    //vos_meta_m *mtm = (vos_meta_m *)p;
    vos_tokenblk_m *ptm = (vos_tokenblk_m *)p;
    ptm->root = NULL;
    ptm->status = 0;

    for(vos_i32 i = 0; i < HASH_SIZE; i++)
        INIT_HLIST_HEAD(&ptm->tab[i]);
    return 0;
}

static inline int vos_add_token_hash(void *dm, vos_tokenblk *tok)
{
    if (NULL == dm || NULL == tok) return -1;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;
    vos_tokenblk *e = NULL;

    size_t name_len = strlen(tok->name) + 1;
    vos_u32 hash = jhash(tok->name, name_len - 1, 0);
    vos_tokenblk_m *tm = (vos_tokenblk_m *)dm;
    head = &tm->tab[hash & (HASH_SIZE - 1)];
    hlist_for_each(node, head)//hlist_for_each_entry(e, node, head, hlist)
    {
        e = hlist_entry(node, vos_tokenblk, hlist);
        if (!strcmp(tok->name, e->name))
        {
            return VOS_ERROR;
        }
    }

    hlist_add_head(&tok->hlist, head);
    return VOS_OK;
}

static inline vos_i32 vos_get_token(void *dm, vos_str name, vos_tokenblk **ppt)
{
    if (NULL == name) return VOS_OK;
    if (NULL == ppt || NULL == dm) return VOS_ERROR;
    vos_tokenblk_m *ptm = (vos_tokenblk_m *)dm;
    if (NULL == ptm) return VOS_ERROR;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;
    vos_tokenblk *e = NULL;
    vos_u32 hash = jhash(name, strlen(name), 0);

    head = &ptm->tab[hash & (HASH_SIZE - 1)];
    hlist_for_each(node, head)//hlist_for_each_entry(e, node, head, hlist)
    {
        e = hlist_entry(node, vos_tokenblk, hlist);
        if (!strcmp(name, e->name))
        {
            (*ppt) = e;
            return VOS_OK;
        }
    }
    return VOS_ERROR;
}

static inline vos_i32 vos_del_token_hash(vos_tokenblk *tok)
{
    if (NULL == tok) return VOS_OK;
    hlist_del(&tok->hlist);
    return VOS_OK;
}

static inline int vos_get_names_by_type(vos_tokenblk *tok, vos_i32 type, char *delimiter, char *buff, vos_i32 n)
{
    if (NULL == tok || NULL == delimiter || NULL == buff) return VOS_ERROR;
    vos_i32 len = 0, res = n;
    while (NULL != tok && n > 0)
    {
        if (type == tok->type)
        {
            len = snprintf(buff, res, "%s%s", tok->name, delimiter);
            buff += len;
            res -= len;
        }
        tok = tok->next;
    }

    if (res < n)
    {
        len = strlen(delimiter);
        buff -= len;
        *buff = 0x00;
    }
    return VOS_OK;
}

#define instance_vos_tokenblk_m(dm, TYPE, num, arg) \
do { \
    vos_meta_m *mm vos_mtm_create(sizeof(vos_tokenblk_m), num, VOS_TOKENBLK_LEN, arg); \
    mm->meta_init = token_init; \
    mm->meta_fini = token_fini; \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = token_m_init; \
    mm->mng_fini = pool_fini; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
    dm = (TYPE *)mm; \
} while(0)

#endif /* VOS_TOKEN_BLOCK_H_ */
