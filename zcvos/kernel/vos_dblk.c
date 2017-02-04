/*
 * vos_dblk.c
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 * -----------------------------------------------------------------
 *   2013-12-10          kevin        rename data_block to vos_dblk
 * -----------------------------------------------------------------
 */

#include "vos_types.h"
#include "vos_metablk.h"
#include "vos_dblk.h"

void * vos_dblk_init(void *p, void *arg)
{
    if (NULL == p) return NULL;

    vos_dblk *pnode = (vos_dblk *)p;
    vos_debug("init dblk at %p\n", p);
    pnode->base = (vos_str) p + VOS_DBLK_SIZE;
    pnode->rd_ptr = pnode->base;
    (NULL == arg)? (pnode->wr_ptr = pnode->base) : (pnode->wr_ptr = arg);
    pnode->cont = NULL;
    return p;
}

void vos_dblk_fini(void *p)
{
    if (NULL == p) return;

    vos_dblk *pnode = (vos_dblk *)p;
    pnode->base = (vos_str) p + VOS_DBLK_SIZE;
    pnode->rd_ptr = pnode->base;
    pnode->wr_ptr = pnode->base;
    pnode->cont = NULL;
}

/*
 * get data length in used 
 *
 * @return
 *  lenth  - means wr_ptr - rd_ptr
 *
 */
vos_i32 vos_dblk_get_len(vos_dblk *block)
{
    if (NULL == block) return VOS_OK;
    vos_debug("get length %p\n", block);

    vos_i32 len = (vos_i32)(block->wr_ptr - block->rd_ptr);

    return (len+vos_dblk_get_len(block->cont));
}

/*
 * set data length on a single block  
 *
 * @block - ponter to a data block
 * @len   - desired length
 *
 * @return
 *     VOS_OK - success
 *    VOS_ERROR  -failed
 *
 */
vos_i32 vos_dblk_set_slen(vos_dblk *block, vos_i32 len)
{
    if (NULL == block) return VOS_OK;
    vos_debug("set length %p to %d\n", block, len);
    if (block->rd_ptr + len - block->base > block->meta.mtm->size)
        return VOS_ERROR;
    block->wr_ptr = block->rd_ptr + len;
    return VOS_OK;
}

/*
 * get space size
 *
 * @block - ponter to a data block
 *
 * @return
 *     space size for each block
 *
 */
vos_u32 vos_dblk_get_size(vos_dblk *block)
{
    if (NULL == block || NULL == block->meta.mtm) return 0;
    vos_debug("get size at %p = %d\n", block, block->meta.mtm->size);
    return block->meta.mtm->size;
}

/*
 * get spare space size including continued blocks if existed
 *
 * @block - ponter to a data block
 *
 * @return
 *     spare size
 */
vos_u32 vos_dblk_get_cap(vos_dblk *block)
{
    if (NULL == block || NULL == block->meta.mtm) return 0;
    vos_debug("get cap at %p = %d\n", block, block->meta.mtm->size);
    vos_int cap = block->meta.mtm->size - VOS_DBLK_SIZE - (block->wr_ptr - block->base);
    return (cap+vos_dblk_get_cap(block->cont));
}

/*
 * get spare space size on a single block
 *
 * @block - ponter to a data block
 *
 * @return
 *     spare size on a single block
 */
vos_u32 vos_dblk_get_scap(vos_dblk *block)
{
    if (NULL == block || NULL == block->meta.mtm) return 0;
    vos_debug("get scap at %p = %d\n", block, block->meta.mtm->size);
    return block->meta.mtm->size - (block->wr_ptr - block->base);

}

/*
 * get read address pointer 
 *
 * @block - ponter to a data block
 *
 * @return
 *     pointer to read buff
 */
vos_str vos_dblk_get_rd(vos_dblk *block)
{
    if (NULL == block) return VOS_OK;
    vos_debug("get read ptr %p\n", block->rd_ptr);
    return block->rd_ptr;
}

/*
 * move read address pointer
 *
 * [NOTE1] this founction would NOT cross border with continued blocks
 * [NOTE2] it implies these data from base to rd_ptr would be ignored ever since.
 *
 * @block - vos_dblk
 * @offset - moving step in bytes
 * 
 * @return
 *    VOS_OK  - success
 *    VOS_ERROR  - failed (when crossing border)
 */
vos_i32 vos_dblk_mv_rd(vos_dblk *block, vos_i32 offset)
{
    if (NULL == block || NULL == block->meta.mtm) return VOS_OK;
    vos_debug("move read ptr %p, offset = %d\n", block->rd_ptr, offset);
    if(block->rd_ptr + offset - block->base < block->meta.mtm->size)
    {
    	block->rd_ptr += offset;
    	return VOS_OK;
    }
    return VOS_ERROR;
}

/*
 * get write address pointer
 *
 * @block - ponter to a data block
 *
 * @return
 *     pointer to write buff
 */
vos_str vos_dblk_get_wr(vos_dblk *block)
{
    if (NULL == block) return NULL;
    vos_debug("get write ptr %p\n", block->wr_ptr);
    return block->wr_ptr;
}

/*
 * move write address pointer
 *
 * [NOTE1] this founction would NOT cross border with continued blocks
 *
 * @block - vos_dblk
 * @offset - moving step in bytes
 * 
 * @return
 *    VOS_OK  - success
 *    VOS_ERROR - failed (when crossing border)
 *
 */
vos_i32 vos_dblk_mv_wr(vos_dblk *block, vos_i32 offset)
{
    if (NULL == block) return VOS_OK;
    vos_debug("move write ptr %p, offset = %d\n", block->wr_ptr, offset);
    if (block->wr_ptr + offset - block->base > block->meta.mtm->size)
        return VOS_ERROR;
    block->wr_ptr += offset;
    return VOS_OK;
}

/*
 * copy data into block at wr_ptr, wr_ptr would be moved sequencely
 *   
 * @block - vos_dblk
 * @src   - data source 
 * @n     - desired bytes
 *
 * @return
 *     VOS_OK  - success
 *     VOS_ERROR - failed (when crossing border )
 *
 */
vos_i32 vos_dblk_ncopy(vos_dblk *block, vos_str src, vos_i32 n)
{
    if (NULL == block || NULL == block->meta.mtm) return VOS_OK;
    vos_debug("copy %p with %s, n=%d\n", block, src, n);
    vos_i32 cap = vos_dblk_get_cap(block);
    if (cap < n)
    {
        vos_debug("can't copy larger msg %d into remaining space %d\n", n, cap);
        return VOS_ERROR;
    }

    vos_i32 size = n;
    vos_i32 scap = vos_dblk_get_scap(block);
    if (scap > 0)
    {
        // still have some space in first block
        size = (n > scap) ? scap : n;
        memcpy(block->wr_ptr, src, size);
        block->wr_ptr += size;
        n -= size;
        src += size;
        vos_debug("copy fist block %p, left=%s, n=%d\n", block, src, n);
    }

    if (n > 0)
    {
        // continued blocks
        vos_debug("ncopy continue block %p, left=%s, n=%d\n", block, src, n);
        vos_dblk *pnode = NULL;
        while (NULL != (pnode = block->cont))
        {
            scap = vos_dblk_get_scap(pnode);
            if (scap <= 0) continue;
            size = (n > scap) ? scap : n;
            memcpy(pnode->wr_ptr, src, size);
            pnode->wr_ptr += size;
            n -= size;
            src += size;
            vos_debug("copy continue block %p, left=%s, n=%d\n", pnode, src, n);
        }
    }
    return VOS_OK;
}

/*
 * copy data from block
 *
 * @block - vos_dblk
 * @dst   - dest buffer
 * @n     - desired bytes
 *
 * @return
   length  - bytes in actual buffer
 *      VOS_ERROR - failed (when crossing border )
 *
 */
vos_i32 vos_dblk_snprint(vos_dblk *block, vos_str dst, vos_i32 n)
{
    if (NULL == block) return VOS_OK;
    vos_debug("print %p into %p\n", block, dst);
    vos_i32 len = vos_dblk_get_len(block);
    n = (n < len ? n : len);
    vos_i32 left = n;

    vos_i32 size = block->wr_ptr - block->rd_ptr;
    if (n <= size)
    {
        // only one block
        memcpy(dst, block->rd_ptr, n);
        return n;
    }
    else
    {
        // first block
        vos_debug("snprint continue block %p\n", block);
        memcpy(dst, block->rd_ptr, size);
        dst += size;
        left -= size;
    }

    // continued blocks
    vos_dblk *pnode = NULL;
    while (NULL != (pnode = block->cont) && left > 0)
    {
        size = pnode->wr_ptr - pnode->rd_ptr;
        (left < size) ? memcpy(dst, pnode->rd_ptr, left) 
                    : memcpy(dst, pnode->rd_ptr, size);
        dst += size;
        left -= size;
    }
    return n;
}

/*
 * reset a data block, ignore all data stored
 *
 *
 * @db      -pointer to a data block
 *
 *
 */
void vos_dblk_reset(vos_dblk *db)
{
    if (NULL == db || NULL == db->meta.mtm) return;
    db->rd_ptr = db->base;
    db->wr_ptr = db->base;
    memset(db->rd_ptr, 0, db->meta.mtm->size - VOS_DBLK_SIZE);

    vos_debug("reset this blk %p.\n", db);
}
