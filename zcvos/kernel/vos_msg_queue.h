/*
 * vos_msg_queue.h
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 */

#ifndef VOS_MSG_QUEUE_H_
#define VOS_MSG_QUEUE_H_

#include "vos_types.h"
#include "vos_msgblk.h"
#include "vos_list.h"

typedef struct
{
//    pthread_rwlock_t           lock;

#ifdef MUTEX_IN_USE
    pthread_mutex_t             lock;   // lock for data mng
#else
    pthread_spinlock_t          lock;
#endif

    //struct plist_head queue;    // priored list head
    struct list_head          queue;
    vos_u32                 msg_num;    // msg count in queue
    vos_u32              high_water;    // high water level

    pthread_mutex_t           c_lock;   // mutex for condition
    pthread_mutexattr_t       c_attr;   // attribute for conditon
    pthread_cond_t            c_cond;   // condition for wait

} vos_msg_queue_m;

/*
 * destroy a msg queue, free all msg blocks
 *
 *
 * @mq     - pointer to a msg queue
 *
 */
void vos_msg_queue_destory(vos_msg_queue_m *mq);

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
vos_msg_queue_m* vos_msg_queue_create(vos_u32 high_water);

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
vos_i32 vos_msg_queue_push(vos_msg_queue_m *mq, vos_msgblk *mb);

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
vos_i32 vos_msg_queue_pop(vos_msg_queue_m *mq, vos_msgblk **mb);

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
vos_i32 vos_msg_queue_popwait(vos_msg_queue_m *mq, vos_msgblk **mb, vos_u32 ms);

#endif /* VOS_MSG_QUEUE_H_ */
