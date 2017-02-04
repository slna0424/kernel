/*
 * vos_msgblk.h
 *
 *     message block for tasks in an entity
 *
 *  Created on: 2014-1-27
 *      Author: kevin
 */

#include "vos_msgblk.h"
/*
 * free its dblk
 */
void msg_fini(void *p)
{
    if (NULL == p) return;

    vos_msgblk *mb = (vos_msgblk *)p;
    if (NULL != mb->buff && 0 == mb->meta.count) vos_mt_free(mb->buff);
}

/*
 * get length of data block in a msg block
 *
 * @mb - pointer to a msg block
 *
 */
/*vos_i32 vos_msg_get_len(vos_msgblk *mb)
{
    if (NULL == mb) return 0;
    if (NULL == mb->buff) return 0;
    return vos_dblk_get_len(mb->buff);
}*/

/*vos_i32 vos_msg_ncopy(vos_msgblk *mb, const vos_str src, vos_i32 n)
{
    if (NULL == mb) return 0;
    if (NULL == mb->buff) return 0;
    return vos_dblk_ncopy(mb->buff, src, n);
}

vos_i32 vos_msg_snprint(vos_msgblk *mb, vos_str dst, vos_i32 n)
{
    if (NULL == mb) return 0;
    if (NULL == mb->buff) return 0;
    return vos_dblk_snprint(mb->buff, dst, n);
}

void vos_msg_reset(vos_msgblk *mb)
{
    if (NULL == mb) return;
    if (NULL == mb->buff) return;
    return vos_dblk_reset(mb->buff);
}
*/
/*
 * create a vos_msgblk cloing data from another vos_msgblk.
 *  reference count of vos_dblk would be increased accordingly.
 */
vos_msgblk * vos_msg_clone(vos_msgblk *msg, void *arg)
{
    if (NULL == msg) return NULL;
    vos_meta_m *mtm = msg->meta.mtm;
    vos_msgblk *mb = vos_mt_malloc(mtm, VOS_MSGBLK_LEN, arg);
    if (NULL != mb)
    {
        mb->buff = msg->buff;
        vos_mt_ref((vos_metablk *)(mb->buff));
    }
    return mb;
}
