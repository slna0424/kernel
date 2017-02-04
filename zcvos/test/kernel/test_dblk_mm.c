/*
 ============================================================================
 Name        : dblk_mm_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */


#include <assert.h>

#include "../../kernel/vos_types.h"
#include "../../kernel/vos_dblk.h"
#include "../../kernel/vos_mm_m.h"

#define COUNT 100
#define SIZE  256

int main(int argc, char *argv[])
{
    vos_meta_m *mtm = NULL;
    vos_m_instance(mtm, vos_mm_m, COUNT, SIZE, NULL);
    assert(NULL != mtm);

    vos_mm_m *pm = (vos_mm_m *)mtm;

    pm->mtm.meta_init = vos_dblk_init;
    char f_name[] = "./dblk_mm.map";
    pm->f_name = f_name;
    assert (VOS_OK == vos_mtm_open(pm));

    vos_mtm_print(pm);
    int count = 0;
    vos_dblk *pmeta = NULL;

    while (NULL != (pmeta = (vos_dblk *)vos_mt_malloc(pm, 30, NULL)))
    {
        printf("==>malloc dblk for index %06d\n", ++count);
    }
    vos_mtm_print(pm);

    vos_mtm_reset(pm);

    pmeta = (vos_dblk *)vos_mt_malloc(pm, 30, NULL);

    assert(pmeta->base == (vos_str)pmeta+VOS_DBLK_SIZE);
    assert(pmeta->base == pmeta->rd_ptr);
    assert(pmeta->wr_ptr == pmeta->rd_ptr);
    assert(0 == vos_dblk_get_len(pmeta));
    assert(vos_dblk_get_size(pmeta) == vos_dblk_get_cap(pmeta));

    char data[]="hello world!";
    int len = strlen(data);
    assert(0 == vos_dblk_ncopy(pmeta, data, len));
    printf("len=%d, dblk_len=%d\n", len, vos_dblk_get_len(pmeta));
    assert(len == vos_dblk_get_len(pmeta));
    vos_mt_free(pmeta);
    assert(COUNT == pm->mtm.left);
    vos_mtm_destory(pm);
    return 0;
}
