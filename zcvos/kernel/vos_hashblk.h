/*
 ============================================================================
 Name        : vos_hashblk.h
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#ifndef VOS_HASH_BLOCK_H_
#define VOS_HASH_BLOCK_H_


#include "vos_types.h"
#include "vos_hash.h"
#include "vos_jhash.h"
#include "vos_metablk.h"
#include "vos_pool_m.h"

typedef struct
{
    vos_metablk             meta;       // meta block
    struct hlist_node      hlist;       // hash list
} vos_hashblk;

#define VOS_HASHBLK_LEN         sizeof(vos_hashblk)

typedef struct
{
    vos_pool_m            pool_m;       // pool_m
    vos_u32               h_size;       // size for hash
    struct hlist_head   *h_links;       // hash list for link
} vos_hashblk_m;

void * vos_hashblk_init(void *p, void *arg);

void vos_hashblk_fini(void *p);

vos_i32 vos_hashblk_m_init(void *p);

void vos_hashblk_m_fini(void *dm);

#define instance_vos_hashblk_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_hashblk_m), count, size, arg); \
    mm->meta_init = vos_hashblk_init; \
    mm->meta_fini = vos_hashblk_fini; \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = vos_hashblk_m_init; \
    mm->mng_fini = vos_hashblk_m_fini; \
    mm->mng_hm = pool_hm; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
} while(0)

vos_int vos_add_hashblk(void *dm, void *p, const void *key, vos_u32 length);

void vos_del_hashblk(void *p);

typedef vos_int (*cmp_hashblk)(void *p, const void *key, vos_u32 length);

vos_hashblk *vos_get_hashblk(void *dm, cmp_hashblk cmp_func, const void *key, vos_u32 length);

#endif /* VOS_HASH_BLOCK_H_ */
