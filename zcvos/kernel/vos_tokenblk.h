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
#include "vos_hashblk.h"

typedef struct _vos_tokenblk
{
    vos_hashblk              hblk;
    vos_i32                  type;
    vos_str                  name;
    vos_str                 value;
    struct _vos_tokenblk    *left;
    struct _vos_tokenblk   *right;
    struct _vos_tokenblk    *next;
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
    vos_hashblk_m             hblk_m;
    vos_tokenblk               *root;
    vos_i32                   status;
} vos_tokenblk_m;

void * token_init(void *p);

void token_fini(void *p);

vos_int token_m_init(void *p);

vos_int vos_add_token(void *dm, vos_tokenblk *tok);

#define vos_del_token(p)       vos_del_hashblk(p)

vos_tokenblk * vos_get_token(void *dm, vos_str name);

#define instance_vos_tokenblk_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_tokenblk_m), count, VOS_TOKENBLK_LEN, arg); \
    mm->meta_init = token_init; \
    mm->meta_fini = token_fini; \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = token_m_init; \
    mm->mng_fini = vos_hashblk_m_fini; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
} while(0)

#endif /* VOS_TOKEN_BLOCK_H_ */
