/*
 ============================================================================
 Name        : vos_bitmap.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <stdlib.h>
#include "vos_bitmap.h"

vos_bitmap_m* vos_bitmap_create(vos_i32 nbits)
{
    vos_bitmap_m *bm = (vos_bitmap_m *)malloc(sizeof(vos_bitmap_m));
    if (NULL != bm)
    {
        bm->nbits = nbits;
        bm->nbytes = BITS_TO_BYTE(bm->nbits);
        bm->map = NULL;
    }
    return bm;
}

void vos_bitmap_destory(vos_bitmap_m *bm)
{
    if (NULL != bm) free(bm);
}

vos_i32 vos_bitmap_load(vos_bitmap_m *bm, vos_u8 *map)
{
    if (NULL == bm || NULL == map) return VOS_ERROR;
    bm->map = map;
    return VOS_OK;
}

void vos_bitmap_unload(vos_bitmap_m *bm)
{
    if (NULL == bm) return;
    bm->map = NULL;
}

void vos_bitmap_zero(vos_bitmap_m *bm)
{
    memset((void *)(bm->map), 0, bm->nbytes);
}

void vos_bitmap_set(vos_bitmap_m *bm, vos_i32 offset)
{
    if (NULL != bm && offset < bm->nbits)
        bm->map[BITS_TO_BYTE(offset + 1) - 1] |= (1 << (BITS_PER_BYTE - 1 - (offset % BITS_PER_BYTE)));
}

void vos_bitmap_clear(vos_bitmap_m *bm, vos_i32 offset)
{
    if (NULL != bm && offset < bm->nbits)
        bm->map[BITS_TO_BYTE(offset + 1) - 1] &= (~(1 << (offset % BITS_PER_BYTE)));
}

vos_i32 vos_bitmap_test(vos_bitmap_m *bm, vos_i32 offset)
{
    if (NULL != bm && offset < bm->nbits)
    {
        return (vos_i32)((bm->map[BITS_TO_BYTE(offset + 1) - 1] >> (BITS_PER_BYTE - 1 - (offset % BITS_PER_BYTE))) & 1);
    }
    else
        return -1;
}

void vos_bitmap_toggle(vos_bitmap_m *bm, vos_i32 offset)
{
    if (NULL != bm && offset < bm->nbits)
        bm->map[BITS_TO_BYTE(offset + 1) - 1] = ~bm->map[BITS_TO_BYTE(offset + 1) - 1];
}

vos_i32 vos_bitmap_fetch(vos_bitmap_m *bm, vos_i32 offset)
{
    if (NULL != bm && offset < bm->nbits)
        return (vos_i32)(bm->map[BITS_TO_BYTE(offset + 1) - 1] >> (offset % BITS_PER_BYTE)) & 1;
    else
        return VOS_ERROR;
}

vos_i32 vos_bitmap_take(vos_bitmap_m *bm)
{
    if (NULL == bm) return INVALID_INDEX;
    for (vos_i32 i = 0; i < bm->nbytes; i++)
    {
        if (0xFF != bm->map[i])
        {
            vos_i32 pos = 0;
            vos_i32 mask = (bm->map[i] ^ 0xFF);
            while (0 == (mask & (1 << pos)) && pos < 7)
                    pos++;
            bm->map[i] |= (1 << pos);
            return i * BITS_PER_BYTE + pos;
        }
    }
    return INVALID_INDEX;
}
