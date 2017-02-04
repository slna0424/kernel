/*
 * vos_pool_m.c
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 */

#include "vos_pool_m.h"

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
void *pool_malloc(void *p, vos_size size, void *arg)
{
    if (NULL == p) return NULL;
    vos_pool_m *pm = (vos_pool_m *)p;
    vos_debug("try to get blk.\n");
    // get from free
    vos_metablk *pnode = NULL;
    if(list_empty(&pm->free_l))
    {
        vos_debug("free meta list is emtpy.\n");
        pnode = NULL;
    }
    else
    {
        pnode = list_first_entry(&pm->free_l, vos_metablk, list);
        vos_list_move_tail(&pnode->list, &pm->used_l);
        vos_debug("get pnode=%p.\n", pnode);
    }

    if (NULL == pnode)
        return NULL;
    else
    {
        return pnode;
    }
}

/*
 * recycle the specific meta block in pool
 *
 * [NOTE] this function is NOT thread-safe or process-safe
 *
 * @data   - meta block
 *
 */
void pool_free(void *data)
{
    if (NULL == data) return;
    vos_metablk *node = (vos_metablk *)data;
    vos_pool_m *pm = (vos_pool_m *)(node->mtm);
    if (NULL == pm) return;

    // move meta from used list to free list
    vos_list_move_tail(&node->list, &pm->free_l);
}

/*
 *  init pool manager  - called by function vos_mtm_open
 *    before calling this function,  params (size, count, i_func, etc) should be assinged correctly!
 *
 */
vos_i32 pool_init(void *p)
{
    if (NULL == p) return VOS_ERROR;
    vos_pool_m *pm = (vos_pool_m *)p;

    INIT_LIST_HEAD(&pm->free_l);
    INIT_LIST_HEAD(&pm->used_l);

    vos_metablk *pnode = NULL;
    vos_i32 i = 0;
    for (; i < pm->mtm.count; i++)
    {
        pnode = (vos_metablk *)malloc(pm->mtm.size);
        if (NULL == pnode)
        {
            vos_debug ("prepare memory for metablk failed at %d\n", i);
            return VOS_ERROR;
        }
        vos_list_add(&pnode->list, &pm->free_l);
        vos_debug ("create metablk and add free list with %d, msg = %p\n", i, pnode);
    }
    return VOS_OK;
}

/*
 * destroy a pool manager
 *    this function would try to call r_func to recycle meta blocks.
 *
 *
 * @dm     -pointer to a meta manager
 *
 */
void pool_fini(void *dm)
{
    vos_pool_m *pm = (vos_pool_m *)dm;
    if (NULL == pm) return;

    // free all blocks
    struct list_head *plist, *tmp;
    vos_metablk *pnode = NULL;
    if(!list_empty(&pm->used_l))
    {
        list_for_each_safe(plist, tmp, &pm->used_l)
        {
            pnode = list_entry(plist, vos_metablk, list);
            vos_list_del(&pnode->list);
            free(pnode);
        }
    }

    if(!list_empty(&pm->free_l))
    {
        list_for_each_safe(plist, tmp, &pm->free_l)
        {
            pnode = list_entry(plist, vos_metablk, list);
            vos_list_del(&pnode->list);
            free(pnode);
        }
    }
}

/*
 * do healthy maintenance on a pool manager
 *    recycle all used blocks lived too long
 *
 *
 * @dm     - pointer to a pool manager
 * @age    - living seconds
 *
 */
void pool_hm(void *dm, vos_int age)
{
    vos_pool_m *pm = (vos_pool_m *)dm;
    if (NULL == pm) return;
    time_t now;
    time(&now);

    // check free blocks
    struct list_head *plist, *tmp;
    vos_metablk *pnode = NULL;
    if(!list_empty(&pm->used_l))
    {
        list_for_each_safe(plist, tmp, &pm->used_l)
        {
            pnode = list_entry(plist, vos_metablk, list);
            if (age < difftime(now, pnode->tick))
            {
                vos_mt_free(pnode);
            }
            else
            {
                // all remains are younger than this one
                break;
            }
        }
    }
}

/*
 * reset a pool manager
 *    recycle all used meta blocks and make them free to be used
 *
 *
 * @dm     - pointer to a meta manager
 *
 */
void pool_reset(void *dm)
{
    vos_pool_m *pm = (vos_pool_m *)dm;
    if (NULL == pm) return;

    // free blocks
    struct list_head *plist, *tmp;
    vos_metablk *pnode = NULL;
    if(!list_empty(&pm->used_l))
    {
        list_for_each_safe(plist, tmp, &pm->used_l)
        {
            pnode = list_entry(plist, vos_metablk, list);
            pnode->count = 1;  // free forcely
            vos_mt_free(pnode);
        }
    }
}

/*
 *  print info about a meta manager
 */
void pool_print(void *dm)
{
    vos_pool_m *pm = (vos_pool_m *)dm;
    if (NULL == pm) return;

    struct list_head *plist, *tmp;
    vos_i32 count = 0;
    if(!list_empty(&pm->used_l))
    {
        list_for_each_safe(plist, tmp, &pm->used_l)
        {
            count++;
        }
    }
    vos_debug("used list: %d\n", count);

    count = 0;

    if(!list_empty(&pm->free_l))
    {
        list_for_each_safe(plist, tmp, &pm->free_l)
        {
            count++;
        }
    }
    vos_debug("free list: %d, left = %d, count = %d.\n", count, pm->mtm.left, pm->mtm.count);
}
