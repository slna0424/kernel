/*
 * vos_task.h
 *
 *  Created on: 2012-6-2
 *      Author: kevin
 */

#ifndef VOS_TASK_H_
#define VOS_TASK_H_

#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "vos_types.h"
#include "vos_jhash.h"
#include "vos_dblk_m.h"
#include "vos_msg_queue.h"
#include "vos_env.h"

/*
 * vos_task is a bundle of thread which is 
 *     dedicated for handling messages or events
 *
 */
typedef struct _vos_task_m vos_task_m;
typedef struct _vos_task  vos_task;

typedef void *(*thread_func)(void *arg);
typedef vos_int (*init_task)(vos_task *, void *arg);
typedef void (*fini_task)(vos_task *);
typedef vos_int (*process_func)(vos_task *, vos_msgblk *arg);
typedef vos_int (*idle_func)(vos_task *);

struct _vos_task
{
    vos_int             task_id;        // task id defined by creator
    vos_str           task_name;        // task name

    vos_meta_m          *dblk_m;        // dblk mng
    vos_meta_m           *msg_m;        // msg mng

    vos_msg_queue_m    *queue_m;        // message queue

    vos_u32           n_threads;        // working thread number
    pthread_t             *pids;        // working thread pool
    vos_u32               msecs;        // waiting time for empty queue

    init_task              init;        // func for init
    fini_task              fini;        // func for clear up
    thread_func            loop;        // working thread entry
    idle_func              idle;        // func for idle
    process_func        process;        // func for msg proccessing
    vos_int                cond;        // status
    vos_str              n_name;        // name for next task
    vos_task              *next;        // pointer to next task

    void                   *arg;        // cutomized pointer
	struct hlist_node     hlist;        // hash list
};

#define TASK_STATE_INIT        -1
#define TASK_STATE_RUN          0
#define TASK_STATE_HOLD         1

void *vos_task_svc(void *arg);

extern vos_int g_task_id;

#define vos_dblk_malloc(t, size, arg) vos_mt_malloc(((vos_task *)(t))->dblk_m, size, arg)

vos_msgblk *vos_msg_malloc(vos_task *t, vos_size size, vos_ptr arg);

vos_msgblk * vos_msg_malloc_wait(vos_task *t, vos_size size, vos_u32 ms, void *arg);

vos_msgblk * vos_msg_dup(vos_task *t, vos_metablk *node, void *arg);

vos_msgblk * vos_msg_dup_wait(vos_task *t, vos_metablk *node, vos_u32 ms, void *arg);

#define vos_msg_free(mb) vos_mt_free(mb)

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
vos_int vos_task_instance(vos_task **ppt, const vos_str name, vos_u32 high_water, vos_int len, vos_int count, void *arg);

#define instance_vos_task(pptask, name, highwater, n_threads, arg) \
    vos_task_instance(pptask, name, highwater, sizeof(vos_task), n_threads, arg)

/*
 * destroy a task
 *
 *
 * @ptask   - pointer to a task
 *
 */
void vos_task_destroy(vos_task *ptask);

/*
 * open a task
 *  [NOTE] in default task cond would be set as TASK_STATE_HOLD
 *
 * @ptask   - pointer to a task
 * @arg     - customized para for func task_init
 *
 */
vos_int vos_task_open(vos_task *ptask, void *arg);

/*
 * close a task
 *
 *
 * @ptask   - pointer to a task
 *
 */
vos_int vos_task_close(vos_task *ptask);

/*
 * put a msg into a task msg queue
 *    [NOTE] using priority (Descending-priority-sorted) in msg if in need 
 *
 * @ptask   - pointer to task
 * @mb      - msg block
 *
 */
vos_int vos_task_put(vos_task *ptask, vos_msgblk *mb);

/*
 * threa entry
 *
 * [NOTE] please modify this in your inherited task type
 */
void *vos_task_svc(void *arg);

/*
 * task manager
 *
 */

#define TASK_HASH_BITS      6
#define TASK_HTABLE_SIZE    (1 << TASK_HASH_BITS)
#define MAX_HTABLE_NUM      (TASK_HTABLE_SIZE - 1)

struct _vos_task_m
{
    vos_u32         table_size;     // task table size
    struct hlist_head  *ptable;
};

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
vos_int vos_tm_instance(vos_u32 bits, vos_task_m **pptm);

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
vos_int vos_tm_add_task(vos_task_m *ptm, vos_task *task);

/*
 * macro for task declearation
 *    
 *
 * @tm      - pointer to a task manater
 * @name    - task name
 * @highwater - highwater for task msg queue
 * @type    - task type
 * @nname   - name for the next task
 *
 */
#define DEFINE_APP_TASK(name, highwater, type, n_threads, arg, next_name) \
do {\
    if (NULL == g_env->task_m) break; \
    vos_task *task = NULL; \
    if (0 == instance_##type(&task, name, highwater, n_threads, arg)) \
    {\
        task->n_name = next_name; \
        vos_debug("try to add a task\n"); \
        if (0 != vos_tm_add_task(g_env->task_m, task)) \
        {\
            vos_debug("failed to add task %s\n", task->task_name); \
            vos_task_destroy(task);\
        }\
    }\
} while(0)

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
vos_int vos_tm_del_task(vos_task *task);

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
vos_task * vos_tm_get_task(vos_task_m *ptm, vos_str name);

#define DEFINE_SYS_TASK(name, highwater, type, n_threads, arg, st_type) \
do { \
    if (st_type >= SYS_TASK_BUTT) break; \
    vos_task *task = NULL; \
    if (0 == instance_##type(&task, name, highwater, n_threads, arg)) \
    {\
        vos_debug("try to add sys task %x\n", st_type); \
        g_env->t_sys[st_type] = (vos_ptr)task; \
        vos_task_open(task, arg); \
    }\
} while(0)

#define vos_get_systask(st_type) ((st_type) < SYS_TASK_BUTT ? (vos_task *)(g_env->t_sys[st_type]) : NULL)

// find task in short way
#define vos_get_apptask(name) vos_tm_get_task((vos_task_m *)(g_env->task_m), name)

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
vos_int vos_tm_open(vos_task_m *ptm, void *arg);

/*
 * event loop
 *
 * @ptm    - pointer to a task manager
 *
 * @return
 *      -1 - failed
 *       0 - success
 */
vos_int vos_tm_loop(vos_task_m *ptm);

#endif /* VOS_TASK_H_ */
