/*
 ============================================================================
 Name        : msgqueue_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <assert.h>
#include <pthread.h>

#include "../../kernel/vos_types.h"
#include "../../kernel/vos_msgblk.h"
#include "../../kernel/vos_msg_queue.h"

vos_msgblk_m *g_msg_m = NULL;

#define COUNT 100
#define SIZE 256

static inline void *svc1(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_msg_queue_m *mq = (vos_msg_queue_m *)arg;

    vos_msgblk *mb = NULL;
    while (1)
    {
        int count = 0;
        while (NULL != (mb = (vos_msgblk *)vos_mt_malloc(g_msg_m, VOS_MSGBLK_LEN, NULL)))
        {
            printf("==>svc1 malloc dblk for index %06d\n", ++count);
            assert (0 == vos_msg_queue_push(mq, mb));
            vos_mt_free(mb);
        }
        sleep (20);
    }
    pthread_exit(NULL);
}

static inline void *svc2(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_msg_queue_m *mq = (vos_msg_queue_m *)arg;

    vos_msgblk *mb = NULL;
    while (1)
    {
        if (0 == vos_msg_queue_popwait(mq, &mb, 2000))
        {
            vos_mt_free(mb);
        }
        else
        {
            printf("==>svc2 timeout %d\n", (int)pthread_self());
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // msg mng
    vos_meta_m *mtm = NULL;
    vos_m_instance(mtm, vos_msgblk_m, COUNT, VOS_MSGBLK_LEN, NULL);
    assert(NULL != mtm);
    assert (VOS_OK == vos_mtm_open(mtm));
    g_msg_m = (vos_msgblk_m *)mtm;
    printf("--------msg mng open------------------\n");
    // msg queue
    vos_msg_queue_m* mq = vos_msg_queue_create(COUNT);
    assert (NULL != mq);
    printf("--------msg queue open------------------\n");

    vos_msgblk *mb = NULL;
    int count = 0;
    while (NULL != (mb = (vos_msgblk *)vos_mt_malloc(g_msg_m, VOS_MSGBLK_LEN, NULL)))
    {
        printf("==>malloc dblk for index %06d\n", ++count);
        assert (0 == vos_msg_queue_push(mq, mb));
        vos_mt_free(mb);
    }
    vos_mtm_print(g_msg_m);
    printf("msg num=%d\n", mq->msg_num);
    assert(COUNT == mq->msg_num);

    while (mq->msg_num > 0)
    {
        assert (0 == vos_msg_queue_pop(mq, &mb));
        assert (NULL != mb);
        vos_mt_free(mb);
    }
    vos_mtm_print(g_msg_m);
    printf("--------msg queue pop wait ------------------\n");
    vos_msg_queue_popwait(mq, &mb, 5000);
    printf("--------msg queue pop wait end------------------\n");
    pthread_t t1, t2, t3, t4, t5;
    pthread_create(&t1, NULL, svc1, mq);
    pthread_create(&t2, NULL, svc2, mq);
    pthread_create(&t3, NULL, svc2, mq);
    pthread_create(&t4, NULL, svc2, mq);
    pthread_create(&t5, NULL, svc2, mq);
    pthread_join(t1, NULL);
    printf("------- destroy msg queue ------------------\n");
    vos_msg_queue_destory(mq);
    vos_mtm_destory(g_msg_m);
    return 0;
}
