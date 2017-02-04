/*
 * vos_msg_queue.c
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 */

#include "vos_types.h"
#include "vos_msgblk.h"
#include "vos_list.h"
#include "vos_msg_queue.h"

/*
 * destroy a msg queue, free all msg blocks
 *
 *
 * @mq     - pointer to a msg queue
 *
 */
void vos_msg_queue_destory(vos_msg_queue_m *mq)
{
    if (NULL == mq) return;
    //pthread_rwlock_wrlock(&mq->lock);
    struct list_head *plist, *tmp;
    vos_msgblk *pnode = NULL;
    if(list_empty(&mq->queue))
    {
        list_for_each_safe(plist, tmp, &mq->queue)
        {
            pnode = list_entry(plist, vos_msgblk, pn);
            vos_list_del(&pnode->pn);
            vos_mt_free(pnode);
        }
    }
    //pthread_rwlock_unlock(&mq->lock);
    //pthread_rwlock_destroy(&mq->lock);
#ifdef MUTEX_IN_USE
    pthread_mutex_destroy(&mq->lock);
#else
    pthread_spin_destroy(&mq->lock);
#endif
    pthread_mutex_destroy(&mq->c_lock);
    pthread_cond_destroy(&mq->c_cond);
    free(mq);
}

/*
 * create a msg queue
 *
 *
 * @high_water  - high water level
 *
 * @return
 *    NULL - failed
 *    others - pointer to a msg queue
 *
 */
vos_msg_queue_m* vos_msg_queue_create(vos_u32 high_water)
{
    vos_msg_queue_m *mq = (vos_msg_queue_m *)malloc(sizeof(vos_msg_queue_m));
    if (NULL == mq)
    {
        vos_debug ("create vos_msg_queue_m failed\n");
        return NULL;
    }
    //pthread_rwlock_init(&mq->lock, NULL);
#ifdef MUTEX_IN_USE
    pthread_mutex_init(&mq->lock, NULL);
#else
    pthread_spin_init(&mq->lock,0);
#endif
    mq->high_water = high_water;
    mq->msg_num = 0;
    INIT_LIST_HEAD(&mq->queue);

    pthread_mutexattr_init(&mq->c_attr);
    pthread_mutexattr_settype(&mq->c_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&mq->c_lock, NULL);
    pthread_cond_init(&mq->c_cond, NULL);
    return mq;
}

/*
 * push a msg in the queue
 *
 * @mq     - pointer to a msg queue
 * @msg    - pointer to a msg block
 *
 * @return
 *     VOS_OK - success
 *     VOS_ERROR  - failed
 *
 */
vos_i32 vos_msg_queue_push(vos_msg_queue_m *mq, vos_msgblk *mb)
{
    if (NULL == mb) return -1;
    if (NULL == mq) return -1;

    int res = VOS_ERROR;
    //pthread_rwlock_wrlock(&mq->lock);
#ifdef MUTEX_IN_USE
    pthread_mutex_lock(&mq->lock);
#else
    pthread_spin_lock(&mq->lock);
#endif
    if (mq->msg_num < mq->high_water)
    {
        vos_list_add_tail(&mb->pn, &mq->queue);
        mq->msg_num++;
        vos_mt_ref((vos_metablk *)mb);
        //if (NULL != mb->buff) vos_mt_ref((vos_metablk *)(mb->buff));
        res = VOS_OK;
    }
    else
    {
        vos_debug ("reach high water, push msg %p failed to queue %p.\n", mb, mq);
        res = VOS_MSGQUEUE_OVERFLOW;
    }
    //pthread_rwlock_unlock(&mq->lock);
#ifdef MUTEX_IN_USE
    pthread_mutex_unlock(&mq->lock);
#else
    pthread_spin_unlock(&mq->lock);
#endif
    if (VOS_OK == res)  pthread_cond_broadcast(&mq->c_cond);
    return res;
}

/*
 * pop a msg from the queue
 *
 * @mq     - pointer to a msg queue
 * @msg    - pointer to pointer to a msg block
 *
 * @return
 *     VOS_OK - success
 *     VOS_ERROR  - failed
 *
 */
vos_i32 vos_msg_queue_pop(vos_msg_queue_m *mq, vos_msgblk **mb)
{
    if (NULL == mq || NULL == mb) return -1;

    int res = VOS_ERROR;
    //pthread_rwlock_rdlock(&mq->lock);
#ifdef MUTEX_IN_USE
    pthread_mutex_lock(&mq->lock);
#else
    pthread_spin_lock(&mq->lock);
#endif
    if(!list_empty(&mq->queue))
    {
        *mb = list_first_entry(&mq->queue, vos_msgblk, pn);
        vos_list_del(&(*mb)->pn);
        mq->msg_num--;
        // let caller  do this unless msgblk would be free before access it
        //vos_mt_unref((vos_metablk *)mb);
        res = VOS_OK;
        vos_debug("msg queue pop a msg %p, num=%d\n", *mb, mq->msg_num);
    }
    //pthread_rwlock_unlock(&mq->lock);
#ifdef MUTEX_IN_USE
    pthread_mutex_unlock(&mq->lock);
#else
    pthread_spin_unlock(&mq->lock);
#endif
    return res;
}

/*
 * pop a msg from the queue, wait until success
 *
 * @mq     - pointer to a msg queue
 * @msg    - pointer to pointer to a msg block
 * @ms     - wait time in msr
 *
 * @return
 *     VOS_OK - success
 *     VOS_ERROR  - failed
 *
 */
vos_i32 vos_msg_queue_popwait(vos_msg_queue_m *mq, vos_msgblk **mb, vos_u32 ms)
{
    if (NULL == mq || NULL == mb) return VOS_ERROR;
    
    struct timeval now;
    struct timespec timeout;
    int res = 0;

    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + ms / 1000;
    timeout.tv_nsec = now.tv_usec * 1000 + (ms % 1000) * 1000000;

    pthread_mutex_lock(&mq->c_lock);
    if (list_empty(&mq->queue))
    {
        res = pthread_cond_timedwait(&mq->c_cond, &mq->c_lock, &timeout);
    }

    pthread_mutex_unlock(&mq->c_lock);
    return (ETIME == res) ? VOS_ERROR : vos_msg_queue_pop(mq, mb);
}
