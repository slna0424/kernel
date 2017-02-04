/*
 * vos_mm_m.h
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 *
 */

#ifndef VOS_MM_M_H_
#define VOS_MM_M_H_

//#define __USE_XOPEN2K
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#include "vos_types.h"
#include "vos_bitmap.h"
#include "vos_metablk.h"

/*
 *
 * memory map file structure
 *
 * ----------------------------------------------
 *              | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 *   bitmap     | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 *   (count)    |           .....               |
 *              | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 * ----------------------------------------------
 *              |        metablk             |
 *              |         (data)                |
 *              ---------------------------------
 *              |        metablk             |
 *     buff     |         (data)                |
 *   (count)    ---------------------------------
 *              |           ......              |
 *              ---------------------------------
 *              |        metablk             |
 *              |         (data)                |
 * ----------------------------------------------
 *
 */
 
typedef struct
{
    vos_meta_m         mtm;     // meta manager
    vos_str         f_name;     // file name
    vos_ucptr         addr;     // mmap addr
    vos_bitmap_m       *bm;     // bitmap
    vos_str           buff;     // data buffer
} vos_mm_m;

vos_metablk *mm_index(vos_mm_m *dm, vos_u32 index);

vos_i32 mm_indexof(vos_mm_m *dm, vos_metablk *pnode);

void *mm_malloc(void *p, size_t size, void *arg);

void mm_free(void *p);

vos_i32 mm_init(void *ptr);

void mm_fini(void *p);

void mm_hm(void *p, vos_int age);

vos_i32 mm_sync(void *dm, vos_i32 sync);

void mm_reset(void *dm);

#define instance_vos_mm_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_mm_m), count, size, arg); \
    mm->malloc_meta = mm_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = mm_free; \
    mm->mng_init = mm_init; \
    mm->mng_fini = mm_fini; \
    mm->mng_reset = mm_reset; \
    mm->mng_print = NULL; \
} while(0)

#endif /* VOS_MM_M_H_ */
