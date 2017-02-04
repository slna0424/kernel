/*
 ============================================================================
 Name        : tokenblk_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <assert.h>

#include "../../kernel/vos_types.h"
#include "../../kernel/vos_tokenblk.h"

#define COUNT 100
#define SIZE  256

vos_tokenblk_m *g_msg_m = NULL;

int main(int argc, char *argv[])
{
    // msg mng
    vos_m_instance(g_msg_m, vos_msgblk_m, COUNT, VOS_TOKENBLK_LEN, NULL);
    assert(NULL != g_msg_m);
    assert (VOS_OK == vos_mtm_open(g_msg_m));
    printf("--------token mng open------------------\n");

    vos_msgblk *mb = NULL;
    int count = 0;
    while (NULL != (mb = (vos_msgblk *)vos_mt_malloc(g_msg_m, VOS_TOKENBLK_LEN, NULL)))
    {
        printf("==>malloc dblk for index %06d\n", ++count);
    }
    vos_mtm_print(g_msg_m);

    printf("--------reset token mng------------------\n");
    vos_mtm_reset(g_msg_m);

    vos_mtm_print(g_msg_m);

    printf("------- destroy mtm ------------------\n");
    vos_mtm_destory(g_msg_m);
    return 0;
}
