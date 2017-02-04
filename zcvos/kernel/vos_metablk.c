/*
 * vos_metablk.c
 *
 *  Created on: 2013-2-20
 *      Author: kevin
 *
 *   meta data block and manager
 *      NOT safe for multi processes or threads
 *   
 */

#include "vos_types.h"
#include "vos_list.h"
#include "vos_metablk.h"

/*
 * increase metablk reference count
 *
 * @block - pointer to msgblk
 *
 */
void vos_mt_ref(vos_metablk *mt)
{
    if (NULL == mt) return;
    if ((vos_u32)-1 == mt->count)
    {
        vos_debug ("meta at %p count overflow.\n", mt);
        return;
    }
    mt->count++;
}

/*
 * decrease metablk reference count
 *
 * @block - pointer to msgblk
 *
 */
void vos_mt_unref(vos_metablk *mt)
{
    if (NULL == mt) return;
    vos_debug ("count is %d, meta at %p\n", mt->count, mt);
    if (0 >= --mt->count)
    {
        vos_debug ("count is 0, free meta at %p\n", mt);
        // clear meta block & free it
        vos_meta_m *mtm = (vos_meta_m *)(mt->mtm);
        vos_debug("vos mt free mtm=%p, mtm_size=%d. meta=%p\n", mtm, mtm->size, mt);
        if (NULL == mtm) return;
        if (NULL != mtm->meta_fini) mtm->meta_fini(mt);
#ifdef MUTEX_IN_USE
    pthread_mutex_lock(&mtm->lock);
#else
    pthread_spin_lock(&mtm->lock);
#endif
        if (NULL != mtm->free_meta) mtm->free_meta(mt);
        mtm->left++;
#ifdef MUTEX_IN_USE
    pthread_mutex_unlock(&mtm->lock);
#else
    pthread_spin_unlock(&mtm->lock);
#endif
    pthread_cond_broadcast(&mtm->c_cond);
    }
}

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
void *vos_mt_malloc(void *p, vos_size size, void *arg)
{
    if (NULL == p) return NULL;

    vos_meta_m *mtm = (vos_meta_m *)p;

    vos_debug("vos mt malloc begin mtm=%p, size=%d, mtm_size=%d.\n", p, size, mtm->size);
    if (NULL == mtm || size > mtm->size) return NULL;

#ifdef MUTEX_IN_USE
    pthread_mutex_lock(&mtm->lock);
#else
    pthread_spin_lock(&mtm->lock);
#endif
    vos_metablk *meta = (NULL == mtm->malloc_meta) ? NULL : mtm->malloc_meta(p, size, arg);
    if (NULL == meta) goto end_malloc;
    mtm->left--;
#ifdef MUTEX_IN_USE
    pthread_mutex_unlock(&mtm->lock);
#else
    pthread_spin_unlock(&mtm->lock);
#endif
    meta->mtm = mtm;
    meta->count = 1;
    time(&(meta->tick));
    vos_debug("vos mt malloc end. meta=%p\n", meta);
    return (NULL == mtm->meta_init) ? (void *)meta : mtm->meta_init(meta, arg);
end_malloc:
    vos_debug("vos mt malloc fail end.\n");
#ifdef MUTEX_IN_USE
    pthread_mutex_unlock(&mtm->lock);
#else
    pthread_spin_unlock(&mtm->lock);
#endif
    return NULL;
}

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
void * vos_mt_malloc_wait(void *p, vos_size size, vos_u32 ms, void *arg)
{
    if (NULL == p) return NULL;

    vos_meta_m *mtm = (vos_meta_m *)p;

    struct timeval now;
    struct timespec timeout;
    int res = 0;

    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + ms / 1000;
    timeout.tv_nsec = now.tv_usec * 1000 + (ms % 1000) * 1000000;

    pthread_mutex_lock(&mtm->c_lock);
    if (mtm->left <= 0)
    {
        res = pthread_cond_timedwait(&mtm->c_cond, &mtm->c_lock, &timeout);
    }

    pthread_mutex_unlock(&mtm->c_lock);
    return (ETIME == res) ? NULL : vos_mt_malloc(p, size, arg);
}


/*
 * recycle the specific meta block
 *
 * [NOTE] this function is NOT thread-safe or process-safe
 *
 * @data   - meta block
 *
 */
void vos_mt_free(void *data)
{
    if (NULL == data) return;

    return vos_mt_unref((vos_metablk *)data);
}

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
vos_meta_m* vos_mtm_create(vos_u32 msize, vos_i32 count, vos_u32 size, void *arg)
{
    vos_meta_m *mtm = (vos_meta_m *)malloc(msize);
    if (NULL == mtm)
    {
        vos_debug("create vos_meta_m failed\n");
        return NULL;
    }
#ifdef MUTEX_IN_USE
    pthread_mutex_init(&mtm->lock, NULL);
#else
    pthread_spin_init(&mtm->lock,0);
#endif
    pthread_mutexattr_init(&mtm->c_attr);
    pthread_mutexattr_settype(&mtm->c_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&mtm->c_lock, NULL);
    pthread_cond_init(&mtm->c_cond, NULL);

    mtm->count = count;
    mtm->size = size;
    mtm->arg = arg;
    mtm->left = count;

    mtm->meta_init = NULL;
    mtm->meta_fini = NULL;
    mtm->malloc_meta = NULL;
    mtm->malloc_w_meta = NULL;
    mtm->free_meta = NULL;
    mtm->mng_init = NULL;
    mtm->mng_fini = NULL;
    mtm->mng_hm = NULL;
    mtm->mng_reset = NULL;
    mtm->mng_print = NULL;

    return mtm;
}

/*
 * destroy a meta manager
 *    this function would try to call r_func to recycle meta blocks.
 *
 *
 * @dm     -pointer to a meta manager
 *
 */
void vos_mtm_destory(void *dm)
{
    vos_meta_m *mtm = (vos_meta_m *)dm;
    if (NULL == mtm) return;

    // clear meta manager
    if (NULL != mtm->mng_fini) mtm->mng_fini(dm);
#ifdef MUTEX_IN_USE
    pthread_mutex_destroy(&mtm->lock);
#else
    pthread_spin_destroy(&mtm->lock);
#endif
    pthread_mutex_destroy(&mtm->c_lock);
    pthread_cond_destroy(&mtm->c_cond);
    free(mtm);
}

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
vos_i32 vos_mtm_open(void *p)
{
    if (NULL == p) return VOS_OK;
    vos_meta_m *mtm = (vos_meta_m *)p;

    return (NULL == mtm->mng_init) ? VOS_ERROR : mtm->mng_init(p);
}

/*
 * do healthy maintenance on  a meta manager
 *    recycle all used meta blocks lived too long
 *
 *
 * @dm     - pointer to a meta manager
 * @age    - living seconds
 *
 */
void vos_mtm_hm(void *dm, vos_int age)
{
    vos_meta_m *mtm = (vos_meta_m *)dm;
    if (NULL == mtm) return;

    if (NULL != mtm->mng_hm) mtm->mng_hm(dm, age);
}

/*
 * reset a meta manager
 *    recycle all used meta blocks and make them free to be used
 *
 *
 * @dm     - pointer to a meta manager
 *
 */
void vos_mtm_reset(void *dm)
{
    vos_meta_m *mtm = (vos_meta_m *)dm;
    if (NULL == mtm) return;

    if (NULL != mtm->mng_reset) mtm->mng_reset(dm);
}

/*
 *  print info about a meta manager
 */
void vos_mtm_print(void *dm)
{
    vos_meta_m *mtm = (vos_meta_m *)dm;
    if (NULL == mtm) return;

    if (NULL != mtm->mng_print) mtm->mng_print(dm);
}

