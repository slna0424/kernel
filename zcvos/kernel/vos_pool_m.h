/*
 * vos_pool_m.h
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 */

#ifndef VOS_POOL_M_H_
#define VOS_POOL_M_H_

/*
 * using memory as data pool with malloc and free in default
 *
 * [NOTE] manager is NOT thread or process safe
 *
 */

#include "vos_types.h"
#include "vos_metablk.h"

typedef struct
{
    vos_meta_m          mtm;        // meta manager
    struct list_head free_l;        //  meta block list in free
    struct list_head used_l;        // meta block list in used
} vos_pool_m;

/*
 * alloc a new meta block from pool
 *
 * [NOTE] this function is NOT thread-safe or process-safe
 *
 * @mtm     - pointer to a meta manager
 * @size - size desired
 * @arg - customized parm
 *
 * @return
 *    NULL -  failed
 *    others - pointer to a meta block
 *
 */
void *pool_malloc(void *p, vos_size size, void *arg);

/*
 * recycle the specific meta block in pool
 *
 * [NOTE] this function is NOT thread-safe or process-safe
 *
 * @data   - meta block
 *
 */
void pool_free(void *data);

/*
 *  init pool manager  - called by function vos_mtm_open
 *    before calling this function,  params (size, count, i_func, etc) should be assinged correctly!
 *
 */
vos_i32 pool_init(void *p);

/*
 * destroy a pool manager
 *    this function would try to call r_func to recycle meta blocks.
 *
 *
 * @dm     -pointer to a meta manager
 *
 */
void pool_fini(void *dm);

/*
 * do healthy maintenance on a pool manager
 *    recycle all used blocks lived too long
 *
 *
 * @dm     - pointer to a pool manager
 * @age    - living seconds
 *
 */
void pool_hm(void *dm, vos_int age);

/*
 * reset a pool manager
 *    recycle all used meta blocks and make them free to be used
 *
 *
 * @dm     - pointer to a meta manager
 *
 */
void pool_reset(void *dm);

/*
 *  print info about a meta manager
 */
void pool_print(void *dm);

#define instance_vos_pool_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_pool_m), count, size, arg); \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = pool_init; \
    mm->mng_fini = pool_fini; \
    mm->mng_hm = pool_hm; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
} while(0)

#endif /* VOS_POOL_M_H_ */
