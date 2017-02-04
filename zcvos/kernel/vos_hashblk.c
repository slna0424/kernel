/*
 ============================================================================
 Name        : vos_hashblk.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include "vos_hashblk.h"


/*
 * called by vos_mt_malloc
 */
void * vos_hashblk_init(void *p, void *arg)
{
    if (NULL == p) return NULL;

    vos_hashblk *info = (vos_hashblk *)p;
    INIT_HLIST_NODE(&info->hlist);
    return p;
}

/*
 * called by vos_mt_unref
 */
void vos_hashblk_fini(void *p)
{
    if (NULL == p) return;

    vos_hashblk *info = (vos_hashblk *)p;
    hlist_del(&info->hlist);
}

/*
 *  called by open
 */
vos_i32 vos_hashblk_m_init(void *p)
{
    if (VOS_OK != pool_init(p)) return VOS_ERROR;

    vos_hashblk_m *info_m = (vos_hashblk_m *)p;

    if (NULL == info_m) return -1;
    info_m->h_size = 1 << 7;
    info_m->h_links = (struct hlist_head *)malloc(sizeof(struct hlist_head)
            * info_m->h_size);
    if (NULL == info_m->h_links)
    {
        return VOS_ERROR;
    }
    for(vos_int i = 0; i < info_m->h_size; i++)
        INIT_HLIST_HEAD(&info_m->h_links[i]);
    return VOS_OK;
}

/*
 * called by close
 *
 */
void vos_hashblk_m_fini(void *dm)
{
    vos_hashblk_m *info_m = (vos_hashblk_m *)dm;
    if (NULL == info_m) return;

    if (NULL != info_m->h_links) free(info_m->h_links);
    pool_fini(dm);
}

vos_int vos_add_hashblk(void *dm, void *p, const void *key, vos_u32 length)
{
    if (NULL == dm || NULL == p) return VOS_ERROR;
    if (NULL == key || 0 == length) return VOS_ERROR;

    vos_hashblk_m *info_m = (vos_hashblk_m *)dm;
    vos_hashblk *link = (vos_hashblk *)p;
    struct hlist_head *head = NULL;
    vos_u32 hash = jhash(key, length, 0);
    head = &info_m->h_links[hash & (info_m->h_size - 1)];
    hlist_add_head(&link->hlist, head);
    return VOS_OK;
}

void vos_del_hashblk(void *p)
{
    if (NULL == p) return;

    // remove it from hash
    vos_hashblk *link = (vos_hashblk *)p;
    hlist_del(&link->hlist);
}

vos_hashblk *vos_get_hashblk(void *dm, cmp_hashblk cmp_func, const void *key, vos_u32 length)
{
    if (NULL == dm || NULL == key) return NULL;

    if (NULL == cmp_func || 0 == length) return NULL;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;

    vos_hashblk_m *info_m = (vos_hashblk_m *)dm;
    vos_hashblk *e = NULL;
    vos_u32 hash = jhash(key, length, 0);

    head = &info_m->h_links[hash & (info_m->h_size - 1)];
    hlist_for_each(node, head)
    {
        e = hlist_entry(node, vos_hashblk, hlist);
        if (VOS_OK == cmp_func(e, key, length)) return e;
    }
    return NULL;
}

