/*
 * vos_metablk.h
 *
 *  Created on: 2013-2-20
 *      Author: kevin
 *
 *   meta data block and manager
 *      NOT safe for multi processes or threads
 *   
 */

#ifndef VOS_META_BLOCK_H_
#define VOS_META_BLOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "vos_types.h"
#include "vos_list.h"

typedef struct _vos_meta_m vos_meta_m;
typedef struct _vos_metablk vos_metablk;

struct _vos_metablk
{
    struct list_head    list;       // list for meta manager
    vos_u32            count;       // reference counter
    vos_meta_m          *mtm;       // meta manager
    time_t              tick;       // tick on creation
};

#define META_BLOCK_SIZE         sizeof(vos_metablk)

typedef void *(*meta_init_func)(void *, void *); // initial meta when malloc
typedef void (*meta_fini_func)(void *);  // recycle meta when free

typedef void *(*malloc_meta_func)(void *, vos_size, void *); // alloc meta
typedef void *(*malloc_w_meta_func)(void *, vos_size, vos_u32, void*); // alloc meta forcely
typedef void (*free_meta_func)(void *);  // recycle meta

typedef vos_i32 (*mng_init_func)(void*);    // initial manager
typedef void (*mng_fini_func)(void*);   // destory manager
typedef void (*mng_hm_func)(void*, vos_int);     // healthy maintenance

typedef void (*mng_reset_func)(void*);  // reset a manager
typedef void (*mng_print_func)(void*);  // print info

struct _vos_meta_m
{
    vos_i32                     left;   // meta count in lefted

    vos_i32                    count;   // total number
    vos_u32                     size;   // space size for each meta
    void                        *arg;   // customized param for mng

#ifdef MUTEX_IN_USE
    pthread_mutex_t             lock;   // lock for data mng
#else
    pthread_spinlock_t          lock;
#endif

    pthread_mutex_t           c_lock;   // mutex for condition
    pthread_mutexattr_t       c_attr;   // attribute for conditon
    pthread_cond_t            c_cond;   // condition for wait

    meta_init_func         meta_init;   // metablk init when malloc
    meta_fini_func         meta_fini;   // metablk fini when recycle
    
    free_meta_func         free_meta;   // recycle a meta block
    malloc_meta_func     malloc_meta;   // malloc a meta block
    malloc_w_meta_func malloc_w_meta;   // malloc a meta block waited

    mng_init_func           mng_init;   // initializing meta manager itself
    mng_fini_func           mng_fini;   // clearing meta manager itself
    mng_hm_func               mng_hm;   // healthy maintenance

    mng_reset_func         mng_reset;
    mng_print_func         mng_print;
};

#define META_MNG_SIZE           sizeof(vos_meta_m)

/*
 * instance a meta manager, assign params and functions
 *
 * @dm    - pointer to a meta manager
 * @type  - type of inherited meta manager
 * @count - counter of blocks
 * @size  - size of each block in bytes
 * @arg   - customized param
 */
#define vos_m_instance(mtm, type, count, size, arg) instance_##type(mtm, type, count, size, arg)


/*
 * increase metablk reference count
 *
 * @block - pointer to msgblk
 *
 */
void vos_mt_ref(vos_metablk *mt);

/*
 * decrease metablk reference count
 *
 * @block - pointer to msgblk
 *
 */
void vos_mt_unref(vos_metablk *mt);

/*
 * alloc a new meta block
 *
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
void *vos_mt_malloc(void *p, vos_size size, void *arg);

/*
 * alloc a meta block in blocking
 *
 * @mtm -pointer to a meta manager
 * @size - size desired
 * @ms  - wait time in ms
 * @arg - customized parm
 *
 * @return
 *    NULL  - failed
 *    others   - pointer to vos_dblk
 *
 */
void * vos_mt_malloc_wait(void *p, vos_size size, vos_u32 ms, void *arg);


/*
 * recycle the specific meta block
 *
 * [NOTE] this function is NOT thread-safe or process-safe
 *
 * @data   - meta block
 *
 */
void vos_mt_free(void *data);

/*
 * creat a meta manager
 *
 *  call vos_mtm_destroy coresspodingly
 *    
 *     it's possible that caller could assign specified function on meta manager, since function vos_mtm_create just creates a meta manager 
 * without any meta block (it's would be done in function vos_mtm_open).
 *
 * @msize  - real meta manager size in bytes (it makes valid when called as inherited meta manager)
 * @count  - meta block count
 * @size   - meta block size in bytes
 * @arg    - customized param
 *
 * @return
 *    NULL - failed
 *   others  - pointer to a meta manager
 *
 */
vos_meta_m* vos_mtm_create(vos_u32 msize, vos_i32 count, vos_u32 size, void *arg);

/*
 * destroy a meta manager
 *    this function would try to call r_func to recycle meta blocks.
 *
 *
 * @dm     -pointer to a meta manager
 *
 */
void vos_mtm_destory(void *dm);

/*
 * open a meta manager
 *    this funciton would create all meta blocks and assemble related lists.
 *
 *    As an inherited meta manager,  specific function n_func should be implemented.
 *
 * @mtm     -pointer to a meta manager
 *
 * @return:
 *      VOS_OK - success
 *     VOS_ERROR  - failed
 *
 */
vos_i32 vos_mtm_open(void *p);

/*
 * do healthy maintenance on  a meta manager
 *    recycle all used meta blocks lived too long
 *
 *
 * @dm     - pointer to a meta manager
 * @age    - living seconds
 *
 */
void vos_mtm_hm(void *dm, vos_int age);

/*
 * reset a meta manager
 *    recycle all used meta blocks and make them free to be used
 *
 *
 * @dm     - pointer to a meta manager
 *
 */
void vos_mtm_reset(void *dm);

/*
 *  print info about a meta manager
 */
void vos_mtm_print(void *dm);

#endif /* VOS_META_BLOCK_H_ */

