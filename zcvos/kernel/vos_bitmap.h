/*
 * vos_bitmap.h
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 */

#ifndef VOS_BITMAP_H_
#define VOS_BITMAP_H_

#include "vos_types.h"

// bitmap manager
typedef struct
{
    vos_u8          *map;       // bitmap
    vos_i32        nbits;       // bits number
    vos_i32       nbytes;       // bytes number
} vos_bitmap_m;

// bitops
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#ifndef VOS_64
#define BITS_PER_LONG 32
#else
#define BITS_PER_LONG 64

#endif

#define BITS_PER_BYTE 8

#define BITS_TO_LONGS(nr)    DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define BITS_TO_BYTE(nr)     DIV_ROUND_UP(nr, BITS_PER_BYTE)

#define BITOP_WORD(nr)      ((nr) / BITS_PER_LONG)


/*
 * create a bitmap manager
 *
 * @nbits  - how many bits 
 *
 * @return
 *    NULL - when failed
 *    pointer to bitmap manager
 *
 */
vos_bitmap_m* vos_bitmap_create(vos_i32 nbits);

/*
 * destroy a bitmap manager
 *
 *  free memory allocated by bitmap_create
 *
 * @bm  -  pointer to bitmap manager
 *
 */
void vos_bitmap_destory(vos_bitmap_m *bm);

/*
 * load bitmap from specified map instead of blank map on creation
 *
 *     bitmap_unload should be called before bitmap_destory
 *
 * @bm  -  pointer to bitmap manager
 * @map  - specified bitmap
 *
 * @return
 *    VOS_OK - success
 *    VOS_ERROR - failed
 *
 */
vos_i32 vos_bitmap_load(vos_bitmap_m *bm, vos_u8 *map);

/*
 * unload bitmap 
 *
 *    free bitmap if in need before unload
 *
 * @bm  - pointer to bitmap manager
 *
 */
void vos_bitmap_unload(vos_bitmap_m *bm);

/*
 * clear bitmap
 *
 *  set "ocupied" on bits for payloading
 *
 * @bm  - pointer to bitmap manager
 *
 */
void vos_bitmap_zero(vos_bitmap_m *bm);

/*
 * set specified bit in a bitmap
 *
 * @bm   - pointer to bitmap manager
 * @offset - which bit to be set 
 *
 */
void vos_bitmap_set(vos_bitmap_m *bm, vos_i32 offset);

/*
 * clear specified bit in a bitmap
 *
 * @bm   - pointer to bitmap manager
 * @offset - which bit to be cleared
 *
 */
void vos_bitmap_clear(vos_bitmap_m *bm, vos_i32 offset);

/*
 * test specified bit in a bitmap
 *
 * @bm   - pointer to bitmap manager
 * @offset - which bit to be cleared
 *
 * return:
 *      0 - not set
 *      1 - set
 *     -1 - invalid offset
 */
vos_int vos_bitmap_test(vos_bitmap_m *bm, vos_i32 offset);

/*
 * toggle specified bit in a bitmap
 *
 * @bm   - pointer to bitmap manager
 * @offset - which bit to be toggled
 *
 */
void vos_bitmap_toggle(vos_bitmap_m *bm, vos_i32 offset);

/*
 * fetch specified bit in a bitmap
 *
 * @bm   - pointer to bitmap manager
 * @offset - which bit to be toggled
 * @return:
 *       0 - bit is cleared
 *       1 - bit is set
 *      VOS_ERROR -invalid operation
 *
 */
vos_i32 vos_bitmap_fetch(vos_bitmap_m *bm, vos_i32 offset);

/*
 * find and ocupy an unused bit in a bitmap
 *
 * @bm  - pointer to bitmap manager
 *
 * @return:
 *      VOS_ERROR - invalid operation or no resource
 *   others - offset in bitmap
 *
 */
vos_i32 vos_bitmap_take(vos_bitmap_m *bm);

#endif /* VOS_BITMAP_H_ */
