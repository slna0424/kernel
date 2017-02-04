/*
 ============================================================================
 Name        : task_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <assert.h>

#include "../../kernel/vos_dblk.h"
#include "../../kernel/vos_task.h"
#include "../../kernel/vos_env.h"

static inline void *produce_svc(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_task *ptask = (vos_task *)arg;


    int tid = (int)pthread_self();
    printf ("<%s> thread %d enter produce svc\n", ptask->task_name, tid);
    printf ("<%s> thread %d left msg=%d, count=%d\n", ptask->task_name, tid, ptask->dblk_m->left, ptask->dblk_m->count);
    int count = 0;
    while (1)
    {
        if (0 == count % 10) printf ("<%s> thread %d left msg=%d, count=%d\n", ptask->task_name, tid, ptask->dblk_m->left, ptask->dblk_m->count);
        if (0 == count % 50)
        {
            vos_msgblk *msg = NULL;
            for (int i = 0; i < 16; i++)
            {
                msg = (vos_msgblk *)vos_msg_malloc_wait(ptask, 20, 10, NULL);
                if(NULL == msg) break;
                vos_task_put(ptask->next, msg);
                vos_mt_free(msg);
            }
        }
        else
        {
            vos_msgblk *msg = (vos_msgblk *)vos_msg_malloc_wait(ptask, 20, 10, NULL);
            assert(NULL != msg);
            char data[]="inner msg!";
            int len = strlen(data);
            assert(0 == vos_dblk_ncopy((vos_dblk *)msg->buff, data, len));

            vos_task_put(ptask->next, msg);
            vos_mt_free(msg);
            /*printf("-----------produce dblk_m--------------\n");
            vos_mtm_print(ptask->dblk_m);
            printf("-----------produce msg_m--------------\n");
            vos_mtm_print(ptask->msg_m);*/
        }
        count++;
        sleep(1);
    }
    pthread_exit(NULL);
}

static inline void *consume_svc(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_task *ptask = (vos_task *)arg;
    vos_msgblk *mb = NULL;

    int tid = (int)pthread_self();
    printf ("<%s> thread %d enter consume svc\n", ptask->task_name, tid);
    struct timespec wait = {.tv_sec = 0, .tv_nsec = 200 * 1000 * 1000};
    int count = 0; int errors = 0;
    while (1)
    {
        // in running
        //if (VOS_OK != vos_msg_queue_popwait(ptask->queue_m, &mb, 100000))
        if (VOS_OK != vos_msg_queue_pop(ptask->queue_m, &mb))
        {
            //printf ("<%s> thread %d NO msg. count=%d\n", ptask->task_name, tid, count);
            errors++;
            if (0 == errors % 100) printf ("<%s> thread %d NO msg. count=%d\n", ptask->task_name, tid, count);
            nanosleep(&wait, NULL);
            continue;
        }
        count++;
        //printf ("<%s> thread %d find a msg %p. count=%d\n", ptask->task_name, tid, mb, count);
        if (NULL == mb)
        {
            //printf("<%s> thread %d msg queue failed to pop msg.\n", ptask->task_name, tid);
            nanosleep(&wait, NULL);
            continue;
        }
        else
        {
            printf("<%s> msg thread %d queue pop a msg, mb=%p. count=%d\n", ptask->task_name, tid, mb, count);
            vos_mt_free(mb);
        }
    }
    pthread_exit(NULL);
}

static inline int task_proc1(vos_task *t, vos_msgblk *mb)
{
    sleep(1);
    printf("--->task [%s] ref [%d][%d] received \"%s\"\n",
            t->task_name, mb->meta.count, mb->buff->count, ((vos_dblk *)mb->buff)->rd_ptr);

    vos_msgblk *msg = (vos_msgblk *)vos_msg_malloc_wait(t, 20, 10, NULL);
    assert(NULL != msg);
    char data[]="inner msg!";
    int len = strlen(data);
    assert(0 == vos_dblk_ncopy((vos_dblk *)msg->buff, data, len));

    vos_task_put(t->next, msg);
    vos_mt_free(msg);
    printf("-----------proc1 dblk_m--------------\n");
    vos_mtm_print(t->dblk_m);
    printf("-----------proc1 msg_m--------------\n");
    vos_mtm_print(t->msg_m);
    return 0;
}

static inline int task_proc2(vos_task *t, vos_msgblk *mb)
{
    sleep(10);
    printf("--->task [%s] ref [%d][%d] received \"%s\"\n",
            t->task_name, mb->meta.count, mb->buff->count, ((vos_dblk *)mb->buff)->rd_ptr);

    vos_msgblk *msg = (vos_msgblk *)vos_msg_malloc_wait(t, 20, 10, NULL);
    assert(NULL != msg);
    char data[]="inner msg!";
    int len = strlen(data);
    assert(0 == vos_dblk_ncopy((vos_dblk *)msg->buff, data, len));

    vos_task_put(t->next, msg);
    vos_mt_free(msg);
    printf("-----------proc2 dblk_m--------------\n");
    vos_mtm_print(t->dblk_m);
    printf("-----------proc2 msg_m--------------\n");
    vos_mtm_print(t->msg_m);
    return 0;
}

int main(int argc, char *argv[])
{
    vos_debug("cbs_c started....");
#ifdef RUN_DAEMON
    daemon(1,0);
#endif
    char fname[255] = "\0";
    snprintf(fname, 255, "%s.conf", argv[0]);
    if (VOS_OK != init_env(fname, NULL))
    {
        vos_debug("cbs_c failed to init env");
        return -1;
    }

    // log task
    //DEFINE_SYS_TASK("logtask", 256, log_task, 1, NULL, SYS_TASK_LOG);

    // app task
    DEFINE_APP_TASK("task1", 256, vos_task, 1, NULL, "task2");
    DEFINE_APP_TASK("task2", 256, vos_task, 20, NULL, "task1");

    vos_task *t1 = vos_get_apptask("task1");
    assert(NULL != t1);
    vos_m_instance(t1->dblk_m, vos_dblk_pool_m, 16, 256, NULL);
    assert(NULL != t1->dblk_m);
    assert(VOS_OK == vos_mtm_open(t1->dblk_m));
    vos_task *t2 = vos_get_apptask("task2");
    assert(NULL != t2);
    vos_m_instance(t2->dblk_m, vos_dblk_pool_m, 16, 256, NULL);
    assert(NULL != t2->dblk_m);
    assert(VOS_OK == vos_mtm_open(t2->dblk_m));
    printf ("\nmtm left =%d, count=%d\n", t1->dblk_m->left, t1->dblk_m->count);
    t1->process = task_proc1;
    t2->process = task_proc2;
    t1->loop = produce_svc;
    t2->loop = consume_svc;
    printf("--------------task init------------------\n");

/*    vos_msgblk *mb = (vos_msgblk *)vos_msg_malloc(t1, 20, NULL);
    assert(NULL != mb);
    char data[]="hello world!";
    int len = strlen(data);
    assert(0 == vos_dblk_ncopy((vos_dblk *)mb->buff, data, len));
    assert(0 == vos_task_put(t1, mb));
    printf("--------------msg init------------------\n");
    */
    t1->cond = TASK_STATE_RUN;
    t2->cond = TASK_STATE_RUN;
    return open_env(NULL);
}
