/*
 * vos_dblk.h
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 * -----------------------------------------------------------------
 *   2013-12-10          kevin        rename data_block to vos_dblk
 * -----------------------------------------------------------------
 */

#ifndef VOS_DBLK_H_
#define VOS_DBLK_H_

#include "vos_types.h"
#include "vos_metablk.h"

/*
 * vos_dblk provides an abstract data block struct with uniformed functions 
 *
 * [NOTE] all methods are NOT thread-safe, caller should handle related issue accordingly.
 *
 */
 
typedef struct _vos_dblk      vos_dblk;

struct _vos_dblk
{   
    vos_metablk       meta;     // meta block
    vos_dblk         *cont;     // continued block list
    vos_str           base;     // base address
    vos_str         rd_ptr;     // read address
    vos_str         wr_ptr;     // write address
};

#define VOS_DBLK_SIZE       sizeof(vos_dblk)

typedef vos_i32 (*mkcont_func)(vos_dblk *);   // function to make continued block list

void * vos_dblk_init(void *p, void *arg);

void vos_dblk_fini(void *p);

/*
 * get data length in used 
 *
 * @return
 *  lenth  - means wr_ptr - rd_ptr
 *
 */
vos_i32 vos_dblk_get_len(vos_dblk *block);

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
vos_i32 vos_dblk_set_slen(vos_dblk *block, vos_i32 len);

/*
 * get space size
 *
 * @block - ponter to a data block
 *
 * @return
 *     space size for each block
 *
 */
vos_u32 vos_dblk_get_size(vos_dblk *block);

/*
 * get spare space size including continued blocks if existed
 *
 * @block - ponter to a data block
 *
 * @return
 *     spare size
 */
vos_u32 vos_dblk_get_cap(vos_dblk *block);

/*
 * get spare space size on a single block
 *
 * @block - ponter to a data block
 *
 * @return
 *     spare size on a single block
 */
vos_u32 vos_dblk_get_scap(vos_dblk *block);

/*
 * get read address pointer 
 *
 * @block - ponter to a data block
 *
 * @return
 *     pointer to read buff
 */
vos_str vos_dblk_get_rd(vos_dblk *block);

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
vos_i32 vos_dblk_mv_rd(vos_dblk *block, vos_i32 offset);

/*
 * get write address pointer
 *
 * @block - ponter to a data block
 *
 * @return
 *     pointer to write buff
 */
vos_str vos_dblk_get_wr(vos_dblk *block);

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
vos_i32 vos_dblk_mv_wr(vos_dblk *block, vos_i32 offset);

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
vos_i32 vos_dblk_ncopy(vos_dblk *block, vos_str src, vos_i32 n);

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
vos_i32 vos_dblk_snprint(vos_dblk *block, vos_str dst, vos_i32 n);

/*
 * reset a data block, ignore all data stored
 *
 *
 * @db      -pointer to a data block
 *
 *
 */
void vos_dblk_reset(vos_dblk *db);

#endif /* VOS_DBLK_H_ */
