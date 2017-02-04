/*
 * vos_tokenblk.c
 *
 *  Created on: 2014-1-27
 *      Author: kevin
 */

#include "vos_tokenblk.h"

void * token_init(void *p)
{
    if (NULL == p) return NULL;

    vos_tokenblk *pnode = (vos_tokenblk *)p;
    pnode->name = NULL;
    pnode->value = NULL;
    pnode->type = -1;
    pnode->left = NULL;
    pnode->right = NULL;
    pnode->next = NULL;
    INIT_HLIST_NODE(&pnode->hblk.hlist);
    return p;
}

void token_fini(void *p)
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
    hlist_del(&pnode->hblk.hlist);
}

vos_i32 token_m_init(void *p)
{
    if (NULL == p) return 0;

    if (VOS_OK != vos_hashblk_m_init(p)) return VOS_ERROR;

    //vos_meta_m *mtm = (vos_meta_m *)p;
    vos_tokenblk_m *ptm = (vos_tokenblk_m *)p;
    ptm->root = NULL;
    ptm->status = 0;

    return 0;
}

int vos_add_token(void *dm, vos_tokenblk *tok)
{
    return vos_add_hashblk(dm, tok, tok->name, strlen(tok->name));
}

static inline vos_int cmp_token (void *p, const void *key, vos_u32 length)
{
    if (NULL == p || NULL == key) return VOS_ERROR;
    vos_tokenblk *link = (vos_tokenblk *)p;

    if (!strcmp(link->name, (char *)key))
    {
        return VOS_OK;
    }
    return VOS_ERROR;
}

vos_tokenblk *vos_get_token(void *dm, vos_str name)
{
    if (NULL == dm || NULL == name) return NULL;

    return (vos_tokenblk *)vos_get_hashblk(dm, cmp_token, (void *)name, strlen(name));
}

