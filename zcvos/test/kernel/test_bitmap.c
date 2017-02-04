/*
 ============================================================================
 Name        : test_bitmap.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "../../kernel/vos_bitmap.h"

int main(int argc, char *argv[])
{
    printf("start test bitmap\n");

    unsigned char bitmap[] = {0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    unsigned char bitmap2[] = {0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    vos_bitmap_m *pbm = vos_bitmap_create(64);
    vos_bitmap_load(pbm, (vos_u8 *)bitmap);
/*
    for (int i = 0; i < 64; i++)
    {
        printf("byte %x, %x, %x\n",
                i / BITS_PER_BYTE,
                (unsigned char)((bitmap[BITS_TO_BYTE(i + 1) - 1] >> (BITS_PER_BYTE - 1 - (i % BITS_PER_BYTE)))),
                (unsigned char)((bitmap[BITS_TO_BYTE(i + 1) - 1] >> (BITS_PER_BYTE - 1 - (i % BITS_PER_BYTE)))) & 1);
    }*/

    assert(1 == vos_bitmap_test(pbm, 0));
    assert(1 == vos_bitmap_test(pbm, 1));
    assert(1 == vos_bitmap_test(pbm, 2));
    assert(1 == vos_bitmap_test(pbm, 3));
    assert(1 == vos_bitmap_test(pbm, 4));
    assert(1 == vos_bitmap_test(pbm, 5));
    assert(0 == vos_bitmap_test(pbm, 6));

    printf("-------------------\n");
    vos_bitmap_zero(pbm);
    for (int i = 0; i < 64; i++)
    {
        printf("i= %d\n", i);
        assert(0 == vos_bitmap_test(pbm, i));
    }
    printf("\n-------------------\n");

    vos_bitmap_set(pbm, 1);
    for (int i = 0; i < 8; i++)
    {
        printf(" 0x%0x", bitmap[i]);
    }
    printf("\n-------------------\n");
    assert(0 == vos_bitmap_test(pbm, 0));
    assert(1 == vos_bitmap_test(pbm, 1));
    return 0;
}
