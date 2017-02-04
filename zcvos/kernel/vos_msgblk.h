/*
 * vos_msgblk.h
 *
 *     message block for tasks in an entity
 *
 *  Created on: 2014-1-27
 *      Author: kevin
 */

#ifndef VOS_MSG_BLOCK_H_
#define VOS_MSG_BLOCK_H_

#include "vos_types.h"
#include "vos_dblk.h"
#include "vos_list.h"
#include "vos_pool_m.h"

#define MSG_P_APP           16

typedef vos_pool_m vos_msgblk_m;


typedef struct
{
    vos_metablk       meta;     // meta for msgblock itself
    //struct plist_node   pn;     // priored list node for msg queue
    struct list_head    pn;
    //vos_dblk         *buff;     // data block for content
    vos_metablk      *buff;     // data block for content
} vos_msgblk;

#define VOS_MSGBLK_LEN         sizeof(vos_msgblk)

#define BASE_PTR(mb)    (((vos_dblk*)mb->buff)->base)
#define   RD_PTR(mb)    (((vos_dblk*)mb->buff)->rd_ptr)
#define   WR_PTR(mb)    (((vos_dblk*)mb->buff)->wr_ptr)

/*
 * free its dblk
 */
void msg_fini(void *p);

/*
 * get length of data block in a msg block
 *
 * @mb - pointer to a msg block
 *
 */
//vos_i32 vos_msg_get_len(vos_msgblk *mb);

//vos_i32 vos_msg_ncopy(vos_msgblk *mb, const vos_str src, vos_i32 n);

//vos_i32 vos_msg_snprint(vos_msgblk *mb, vos_str dst, vos_i32 n);

//void vos_msg_reset(vos_msgblk *mb);

/*
 * create a vos_msgblk cloing data from another vos_msgblk.
 *  reference count of vos_dblk would be increased accordingly.
 */
vos_msgblk * vos_msg_clone(vos_msgblk *msg, void *arg);

/*
 * using memory dynamic msgblk manger in default
 */
#define instance_vos_msgblk_m(mm, TYPE, count, size, arg) \
do { \
    mm = vos_mtm_create(sizeof(vos_pool_m), count, size, arg); \
    mm->meta_fini = msg_fini; \
    mm->malloc_meta = pool_malloc; \
    mm->malloc_w_meta = vos_mt_malloc_wait; \
    mm->free_meta = pool_free; \
    mm->mng_init = pool_init; \
    mm->mng_fini = pool_fini; \
    mm->mng_hm = pool_hm; \
    mm->mng_reset = pool_reset; \
    mm->mng_print = pool_print; \
} while(0)

#endif /* VOS_MSG_BLOCK_H_ */

