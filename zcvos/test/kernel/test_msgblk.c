/*
 ============================================================================
 Name        : msgblk_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <assert.h>

#include "../../kernel/vos_types.h"
#include "../../kernel/vos_msgblk.h"

#define COUNT 100
#define SIZE  256

vos_msgblk_m *g_msg_m = NULL;

int main(int argc, char *argv[])
{
    // msg mng
    vos_meta_m *mtm = NULL;
    vos_m_instance(mtm, vos_msgblk_m, COUNT, VOS_MSGBLK_LEN, NULL);
    assert(NULL != mtm);
    assert (VOS_OK == vos_mtm_open(mtm));
    g_msg_m = (vos_msgblk_m *)mtm;
    printf("--------msg mng open------------------\n");
    // node mng

    vos_m_instance(mtm, vos_pool_m, COUNT, SIZE, NULL);
    assert(NULL != mtm);
    mtm->meta_init = vos_dblk_init;
    assert (VOS_OK == vos_mtm_open(mtm));
    vos_pool_m *pm = (vos_pool_m *)mtm;
    vos_mtm_print(pm);
    printf("--------node mng open------------------\n");

    vos_msgblk *mb = NULL;
    int count = 0;
    while (NULL != (mb = (vos_msgblk *)vos_mt_malloc(g_msg_m, VOS_MSGBLK_LEN, NULL)))
    {
        printf("==>malloc dblk for index %06d\n", ++count);
    }
    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);

    printf("--------reset msg mng------------------\n");
    vos_mtm_reset(g_msg_m);

    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);

    vos_dblk *node = (vos_dblk *)vos_mt_malloc(pm, 50, NULL);
    mb = (vos_msgblk *)vos_mt_malloc(g_msg_m, VOS_MSGBLK_LEN, NULL);
    assert (NULL != mb);
    mb->buff = (vos_metablk *)node;
    vos_mt_ref((vos_metablk *)node);      // used by mb
    printf("base=%p, buff=%p\n", ((vos_dblk *)mb->buff)->base, mb->buff);
    assert(BASE_PTR(mb) == (vos_str)mb->buff+VOS_DBLK_SIZE);
    assert(BASE_PTR(mb) == RD_PTR(mb));
    assert(WR_PTR(mb) == RD_PTR(mb));
    assert(0 == vos_dblk_get_len(((vos_dblk *)mb->buff)));


    printf("--------write msg ------------------\n");
    char data[]="hello world!";
    int len = strlen(data);
    assert(0 == vos_dblk_ncopy((vos_dblk *)mb->buff, data, len));
    printf("len=%d, dblk_len=%d\n", len, vos_dblk_get_len((vos_dblk *)mb->buff));
    assert(len == vos_dblk_get_len((vos_dblk *)mb->buff));

    printf("--------clone msg ------------------\n");
    vos_msgblk *mb2 = vos_msg_clone(mb, NULL);
    assert(NULL != mb2);
    assert(mb2->buff == mb->buff);
    assert(3 == mb->buff->count);
    assert(COUNT-2 == g_msg_m->mtm.left);
    assert(COUNT-1 == pm->mtm.left);
    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);

    printf("--------free msg, %d, %d ------------------\n", g_msg_m->mtm.left, pm->mtm.left);
    vos_mt_free(node);      // regarding to line 57
    printf("--------ref, %d, %d %d------------------\n", node->meta.count, mb->meta.count, mb2->meta.count);
    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);
    vos_mt_free(mb);
    printf("--------ref, %d, %d %d------------------\n", node->meta.count, mb->meta.count, mb2->meta.count);
    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);
    vos_mt_free(mb2);
    printf("--------ref, %d, %d %d------------------\n", node->meta.count, mb->meta.count, mb2->meta.count);
    vos_mtm_print(pm);
    vos_mtm_print(g_msg_m);
    assert(COUNT == g_msg_m->mtm.left);
    assert(COUNT == pm->mtm.left);

    printf("------- destroy mtm ------------------\n");
    vos_mtm_destory(pm);
    vos_mtm_destory(g_msg_m);
    return 0;
}
