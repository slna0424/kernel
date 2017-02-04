/*
 ============================================================================
 Name        : mm_m_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */
#include <sys/stat.h>

#include "assert.h"
#include "../../kernel/vos_metablk.h"
#include "../../kernel/vos_mm_m.h"

#define COUNT 100
#define SIZE  256

int main(int argc, char *argv[])
{
    vos_meta_m *mtm;
    vos_m_instance(mtm, vos_mm_m, COUNT, SIZE, NULL);
    assert(NULL != mtm);

    vos_mm_m *pm = (vos_mm_m *)mtm;

    char f_name[] = "./test_mm_m.map";
    pm->f_name = f_name;
    assert (VOS_OK == vos_mtm_open(pm));

    vos_mtm_print(pm);
    int count = 0;
    vos_metablk *pmeta = NULL;

    while (NULL != (pmeta = (vos_metablk *)vos_mt_malloc(pm, 30, NULL)))
    {
        printf("==>malloc meta for index %06d\n", ++count);
    }
    vos_mtm_print(pm);
    printf("left = %d\n", pm->mtm.left);
    assert(0 == pm->mtm.left);
    vos_mtm_reset(pm);
    vos_mtm_print(pm);
    assert(COUNT == pm->mtm.left);
    assert(NULL != (pmeta = (vos_metablk *)vos_mt_malloc(pm, 30, NULL)));
    assert(COUNT-1 == pm->mtm.left);
    vos_mtm_print(pm);
    vos_mt_free(pmeta);
    assert(COUNT == pm->mtm.left);
    vos_mtm_print(pm);

    vos_mtm_destory(pm);
    return 0;
}
