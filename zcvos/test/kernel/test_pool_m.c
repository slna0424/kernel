/*
 ============================================================================
 Name        : pool_m_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description :
 ============================================================================
 */

#include "assert.h"
#include "../../kernel/vos_metablk.h"
#include "../../kernel/vos_pool_m.h"

#define COUNT 100
#define SIZE  256

int main(int argc, char *argv[])
{
    vos_meta_m *mtm = NULL;
    vos_m_instance(mtm, vos_pool_m, COUNT, SIZE, NULL);
    assert(NULL != mtm);

    assert (VOS_OK == vos_mtm_open(mtm));
    vos_pool_m *pm = (vos_pool_m *)mtm;
    vos_mtm_print(pm);
    int count = 0;
    vos_metablk *pmeta = NULL;

    assert (NULL != (pmeta = (vos_metablk *)vos_mt_malloc(pm, 30, NULL)));
    printf("meta %p ref %d\n", pmeta, pmeta->count);
    vos_mt_free(pmeta);
    printf("-----------------------------\n");

    while (NULL != (pmeta = (vos_metablk *)vos_mt_malloc(pm, 30, NULL)))
    {
        printf("==>malloc meta for index %06d\n", ++count);
    }
    vos_mtm_print(pm);

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
