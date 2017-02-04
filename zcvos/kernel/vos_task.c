/*
 * vos_task.c
 *
 *  Created on: 2012-6-2
 *      Author: kevin
 */

#include "vos_task.h"

vos_msgblk *vos_msg_malloc(vos_task *t, vos_size size, vos_ptr arg)
{
    if (NULL == t) return NULL;
    if (NULL == t->msg_m || NULL == t->dblk_m) return NULL;
    vos_msgblk *mb = NULL;

    vos_metablk *node = (vos_metablk *)vos_mt_malloc(t->dblk_m, size, arg);
    if (NULL == node) return NULL;

    mb = vos_mt_malloc(t->msg_m, VOS_MSGBLK_LEN, arg);
    if (NULL != mb)
    {
        mb->buff = node;
    }
    else
    {
        vos_mt_free(node);
    }
    return mb;
}

vos_msgblk * vos_msg_malloc_wait(vos_task *t, vos_size size, vos_u32 ms, void *arg)
{
    if (NULL == t) return NULL;
    if (NULL == t->msg_m || NULL == t->dblk_m) return NULL;
    vos_msgblk *mb = NULL;

    vos_metablk *node = (vos_metablk *)vos_mt_malloc_wait(t->dblk_m, size, ms, arg);
    if (NULL == node) return NULL;

    mb = (vos_msgblk *)vos_mt_malloc_wait(t->msg_m, VOS_MSGBLK_LEN, ms, arg);
    if (NULL != mb)
    {
        mb->buff = node;
    }
    else
    {
        vos_mt_free(node);
    }
    return mb;
}

vos_msgblk * vos_msg_dup(vos_task *t, vos_metablk *node, void *arg)
{
    if (NULL == t) return NULL;
    if (NULL == t->msg_m || NULL == t->dblk_m) return NULL;
    vos_msgblk *mb = NULL;

    mb = vos_mt_malloc(t->msg_m, VOS_MSGBLK_LEN, arg);
    if (NULL != mb)
    {
        mb->buff = node;
        vos_mt_ref((vos_metablk *)node);
    }
    else
    {
        vos_mt_free(node);
    }
    return mb;

}

vos_msgblk * vos_msg_dup_wait(vos_task *t, vos_metablk *node, vos_u32 ms, void *arg)
{
    if (NULL == t) return NULL;
    if (NULL == t->msg_m || NULL == t->dblk_m) return NULL;
    vos_msgblk *mb = NULL;

    mb = (vos_msgblk *)vos_mt_malloc_wait(t->msg_m, VOS_MSGBLK_LEN, ms, arg);
    if (NULL != mb)
    {
        mb->buff = node;
        vos_mt_ref((vos_metablk *)node);
    }
    else
    {
        vos_mt_free(node);
    }
    return mb;
}

/*
 * instance a task
 *
 * @ppt    - pointer to a task pointer
 * @name   - task name
 * @high_water - high water for task msg queue
 * @size   - real size for task, especially when inherited task type used
 * @count  - thread number
 * @arg    - customized param
 *
 * @return
 *      <0 - failed
 *  others - success, task id
 *
 */
 //ghr update add real size and count
vos_int vos_task_instance(vos_task **ppt, const vos_str name, vos_u32 high_water, vos_int len, vos_int count, void *arg)
{
    if (NULL == ppt) return -1;
    vos_task *ptask = (vos_task*)malloc(len);
    if (NULL == ptask) return -1;
    memset(ptask,0,len);

    ptask->queue_m = vos_msg_queue_create(high_water);
    if (NULL == ptask->queue_m) return -1;
    ptask->n_threads = count;
    ptask->msecs = 200;
    ptask->task_name = strdup(name);
    ptask->task_id = g_task_id++;
    ptask->cond = TASK_STATE_INIT;
    ptask->next = NULL;
    ptask->n_name = NULL;
    ptask->arg = arg;

    // msg dm
    vos_m_instance(ptask->msg_m, vos_msgblk_m, MAX_MSG_NUM, VOS_MSGBLK_LEN, arg);
    if (NULL == ptask->msg_m)
    {
        vos_debug("failed to create msg dm.\n");
        return VOS_FILE_IO;
    }
    if (VOS_OK != vos_mtm_open(ptask->msg_m)) return VOS_ERROR;
    vos_debug("create msg dm task=%s.\n", ptask->task_name);
    // dblk dm
#ifdef TASK_TEST
    //vos_m_instance(ptask->dblk_m, vos_dblk_mm_m, MAX_DBLK_NUM, MAX_DBLK_SIZE, arg);
    vos_m_instance(ptask->dblk_m, vos_dblk_pool_m, MAX_DBLK_NUM, MAX_DBLK_SIZE, arg);
    if (NULL == ptask->dblk_m)
    {
        vos_debug("failed to create dblk dm.\n");
        return VOS_FILE_IO;
    }
    if (VOS_OK != vos_mtm_open(ptask->dblk_m)) return VOS_ERROR;
#endif
    ptask->init = NULL;
    ptask->fini = NULL;
    ptask->loop = vos_task_svc;
    ptask->process = NULL;
    ptask->idle = NULL;
    INIT_HLIST_NODE(&ptask->hlist);
    *ppt = ptask;
    return 0;
}

/*
 * destroy a task
 *
 *
 * @ptask   - pointer to a task
 *
 */
void vos_task_destroy(vos_task *ptask)
{
    if (NULL == ptask) return;

    if (NULL != ptask->queue_m) vos_msg_queue_destory(ptask->queue_m);
    for (vos_int i = 0; i < ptask->n_threads; i++)
    {
        pthread_cancel(ptask->pids[i]);
    }

    free(ptask->pids);
    free(ptask->task_name);
    if (NULL != ptask->fini) ptask->fini(ptask);
    free(ptask);
}

/*
 * open a task
 *  [NOTE] in default task cond would be set as TASK_STATE_HOLD
 *
 * @ptask   - pointer to a task
 * @arg     - customized para for func task_init
 *
 */
vos_int vos_task_open(vos_task *ptask, void *arg)
{
    if (NULL == ptask) return 0;

    vos_debug ("<%s> task open.\n", ptask->task_name);
    if (NULL != ptask->init)
    {
        // call init if assigned
        if (0 != ptask->init(ptask, arg)) return -1;
    }

    // start thread pool
    ptask->pids = (pthread_t *)malloc(ptask->n_threads * sizeof(pthread_t));
    if (NULL == ptask->pids)
    {
        vos_debug ("create thread pool with n_thread = %d failed.\n", ptask->n_threads);
        return -1;
    }
    for (vos_int i = 0; i < ptask->n_threads; i++)
    {
        vos_debug ("create thread %d.\n", ptask->n_threads);
        pthread_create(&ptask->pids[i], NULL, ptask->loop, ptask);
    }
    return 0;
}

/*
 * close a task
 *
 *
 * @ptask   - pointer to a task
 *
 */
vos_int vos_task_close(vos_task *ptask)
{
    if (NULL == ptask) return 0;

    ptask->cond = TASK_STATE_INIT;
    return 0;
}

/*
 * put a msg into a task msg queue
 *    [NOTE] using priority (Descending-priority-sorted) in msg if in need 
 *
 * @ptask   - pointer to task
 * @mb      - msg block
 *
 */
vos_int vos_task_put(vos_task *ptask, vos_msgblk *mb)
{
    if (NULL == mb) return -1;
    if (NULL == ptask) return -1;
    if (NULL == ptask->queue_m) return -1;
    return vos_msg_queue_push(ptask->queue_m, mb);
}

/*
 * threa entry
 *
 * [NOTE] please modify this in your inherited task type
 */
void *vos_task_svc(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_task *ptask = (vos_task *)arg;
    struct timespec wait = {.tv_sec = 0, .tv_nsec = ptask->msecs * 1000 * 1000};

    vos_msgblk *mb = NULL;

    vos_debug ("==>task %s thread %d enter svc\n", ptask->task_name, (int)pthread_self());
    while (1)
    {
        if (TASK_STATE_RUN != ptask->cond)
        {
            vos_debug("task %s not in run.\n", ptask->task_name);
            sleep(5);
            continue;
        }

        // in running
        if (0 != vos_msg_queue_pop(ptask->queue_m, &mb))
        {
            // empty
            if (NULL != ptask->idle)
            {
                if (0 != ptask->idle(ptask))
                {
                    vos_debug("task %s idle failed.\n", ptask->task_name);
                    break;
                }
            }
            // wait forcely
            nanosleep(&wait, NULL);
        }
        else
        {
            vos_debug ("task %s thread %d find a msg %p\n", ptask->task_name, (int)pthread_self(), mb);
            if (NULL == mb)
            {
                vos_debug("task %s msg queue failed to pop msg.\n", ptask->task_name);
                continue;
            }
            else
            {
                if (NULL == ptask->process)
                {
                    vos_debug ("task %s find a msg without process\n",
                            ptask->task_name);
                }
                else
                {
                    if (-1 == ptask->process(ptask, mb))
                    {
                        vos_debug ("task %s thread %d process msg %p failed\n",
                                ptask->task_name, (int)pthread_self(), mb);
                        vos_mt_free(mb);
                        break;
                    }
                }
                vos_mt_free(mb);
            }
         }
    }
    vos_debug ("==>task %s thread %d exit svc\n", ptask->task_name, (int)pthread_self());
    pthread_exit(NULL);
}

/*
 * instance task manager
 *
 * @bits   - bit num for hash, no more than 32
 * @pptm   - pointer to task manager pointer
 *
 * @return
 *      -1 - failed
 *       0 - success
 *
 */
vos_int vos_tm_instance(vos_u32 bits, vos_task_m **pptm)
{
    if (0 == bits || bits > 32) return -1;
    if (NULL == pptm) return -1;
    vos_task_m *ptm = (vos_task_m*)malloc(sizeof(vos_task_m));
    if (NULL == ptm) return -1;

    ptm->table_size = 1 << bits;
    ptm->ptable = (struct hlist_head *)malloc(sizeof(struct hlist_head)
            * ptm->table_size);
    if (NULL == ptm->ptable)
    {
        free(ptm);
        return -1;
    }
    vos_debug("--- ptm size= %d\n", ptm->table_size);
    for(vos_int i = 0; i < ptm->table_size; i++)
        INIT_HLIST_HEAD(&ptm->ptable[i]);
    (*pptm) = ptm;
    return 0;
}

/*
 * add a task into task manager
 *
 * @tm     - pointer to a task manager
 * @task   - pointer to a task
 *
 * @return
 *      -1 - failed
 *       0 - success
 *
 */
vos_int vos_tm_add_task(vos_task_m *ptm, vos_task *task)
{
    if (NULL == task) return 0;
    if (NULL == ptm || NULL == ptm->ptable) return -1;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;
    vos_task *e = NULL;

    size_t name_len = strlen(task->task_name) + 1;
    vos_u32 hash = jhash(task->task_name, name_len - 1, 0);
    vos_debug("tm add task %s with hash 0x%x, 0x%x\n", task->task_name, hash, hash & (ptm->table_size - 1));
    head = &ptm->ptable[hash & (ptm->table_size - 1)];
    hlist_for_each(node, head)//hlist_for_each_entry(e, node, head, hlist)
    {
        e = hlist_entry(node, vos_task, hlist);
        vos_debug("add cmp task %s\n", e->task_name);
        if (!strcmp(task->task_name, e->task_name))
        {
            // duplicate name
            vos_debug("duplicated task name %s, failed to add this one.\n", task->task_name);
            return -1;
        }
    }

    hlist_add_head(&task->hlist, head);
    return 0;
}

/*
 * remove a task from a task manager
 *
 * @tm     - pointer to a task manager
 * @task   - pointer to a task
 *
 * @return
 *      -1 - failed
 *       0 - success
 *
 */
vos_int vos_tm_del_task(vos_task *task)
{
    if (NULL == task) return 0;
    hlist_del(&task->hlist);
    return 0;
}

/*
 * find a task by name
 *
 * @tm     - pointer to a task manager
 * @name   - task name
 * @ppt    - pointer to task pointer
 *
 * @return
 *      NULL - task not existed
 *    others - pointer to the task
 *
 */
vos_task * vos_tm_get_task(vos_task_m *ptm, vos_str name)
{
    if (NULL == name) return NULL;
    if (NULL == ptm || NULL == ptm->ptable) return NULL;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;
    vos_task *e = NULL;
    vos_u32 hash = jhash(name, strlen(name), 0);

    head = &ptm->ptable[hash & (ptm->table_size - 1)];
    hlist_for_each(node, head)//hlist_for_each_entry(e, node, head, hlist)
    {
        e = hlist_entry(node, vos_task, hlist);
        vos_debug("get task cmp name %s\n", e->task_name);
        if (!strcmp(name, e->task_name))
        {
            vos_debug("find the task in name %s.\n", e->task_name);
            return e;
        }
    }
    return NULL;
}

/*
 * modify: by ghr on 2013-03-06
 *    des: start all tasks in a manager except for SYS tasks
 */

/*
 * start all tasks except for SYS tasks
 *    if specified, make tasks in chain automaticlly
 *
 * @ptm    - pointer to a task manager
 * @arg    - customized param when task_open called
 *
 * @return
 *      -1 - failed
 *       0 - success
 *
 */
vos_int vos_tm_open(vos_task_m *ptm, void *arg)
{
    if (NULL == ptm || NULL == ptm->ptable) return -1;

    struct hlist_head *head = NULL;
    struct hlist_node *node = NULL;
    vos_task *e = NULL, *next = NULL;
    // start tasks
    int i = 0;
    for (; i < ptm->table_size; i++)
    {
        head = &ptm->ptable[i];
        hlist_for_each(node, head)//hlist_for_each_entry(e, node, head, hlist)
        {
            e = hlist_entry(node, vos_task, hlist);
            next = NULL;
            if (NULL != e->n_name)
            {
                next = vos_get_apptask(e->n_name);
                vos_debug("=====>next=%p\n", next);
                if (NULL == next)
                {
                    vos_debug("Can't find next task in name %s at task %s.\n", e->n_name, e->task_name);
                    return -1;
                }
                else
                {
                    vos_debug("find next task in name %s at task %s.\n", e->n_name, e->task_name);
                    e->next = next;
                }
            }

            if (0 != vos_task_open(e, arg))
            {
                vos_debug("Failed to open task %s.\n", e->task_name);
                return -1;
            }
        }
    }
    return 0;
}

/*
 * event loop
 *
 * @ptm    - pointer to a task manager
 *
 * @return
 *      -1 - failed
 *       0 - success
 */
vos_int vos_tm_loop(vos_task_m *ptm)
{
    if (NULL == ptm) return -1;
    while (1)
    {
        sleep(10000);
    }
    return 0;
}
