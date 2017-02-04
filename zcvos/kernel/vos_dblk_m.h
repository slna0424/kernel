/*
 ============================================================================
 Name        : vos_dblk_m.h
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#ifndef VOS_DBLK_M_H_
#define VOS_DBLK_M_H_

#include "vos_types.h"
#include "vos_dblk.h"
#include "vos_pool_m.h"
#include "vos_mm_m.h"

typedef vos_pool_m vos_dblk_pool_m;

#define instance_vos_dblk_pool_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_pool_m), count, size, arg); \
    mm->meta_init = vos_dblk_init; \
    mm->meta_fini = vos_dblk_fini; \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = pool_init; \
    mm->mng_fini = pool_fini; \
    mm->mng_hm = pool_hm; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
} while(0)

typedef vos_mm_m vos_dblk_mm_m;

#define instance_vos_dblk_mm_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_mm_m), count, size, arg); \
    mm->meta_init = vos_dblk_init; \
    mm->meta_fini = vos_dblk_fini; \
    mm->malloc_meta = mm_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = mm_free; \
    mm->mng_init = mm_init; \
    mm->mng_fini = mm_fini; \
    mm->mng_hm = mm_hm; \
    mm->mng_reset = mm_reset; \
    mm->mng_print = NULL; \
} while(0)



#endif /* VOS_DBLK_M_H_ */
