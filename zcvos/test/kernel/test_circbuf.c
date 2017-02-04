/*
 ============================================================================
 Name        : circbuff_test.c
 Author      : kevin
 Version     : 1.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 
 ============================================================================
 */

#include <stdio.h>
#include "assert.h"
#include "../../kernel/vos_types.h"
#include "../../kernel/vos_circbuf.h"

#define BUFSIZE     16

int main(int argc, char *argv[])
{
    unsigned char data[BUFSIZE] = "\0";
    vos_circ_buff cb;
    vos_circ_buff *pcb = &cb;

    INIT_CIRC_BUFF(pcb, data, 4);
    assert(CIRC_BUFF_IS_EMPTY(pcb));
    int i = 0;
    while(!CIRC_BUFF_IS_FULL(pcb))
    {
        CIRC_BUFF_WRITE_BYTE(pcb, (char)(0x30 + (i++)));
        printf("write byte %x, rd_pos[%d], wr_pos[%d], count[%d],size[%d], %d\n",
                data[pcb->wr_pos - 1], pcb->rd_pos, pcb->wr_pos, CIRC_BUFF_LENGTH(pcb), pcb->size,
                (pcb->rd_pos - pcb->wr_pos) & (pcb->size - 1));
    }
    assert(!CIRC_BUFF_IS_EMPTY(pcb));
    assert(CIRC_BUFF_IS_FULL(pcb));
    printf("1 count to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_CNT_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    printf("1 space to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_SPACE_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    assert(BUFSIZE-1 == (CIRC_BUFF_CNT_TO_END(pcb)));
    assert(0 == (CIRC_BUFF_SPACE_TO_END(pcb)));
    i = 0;
    while(!CIRC_BUFF_IS_EMPTY(pcb))
    {
        char v = 0x00;
        CIRC_BUFF_READ_BYTE(pcb, v);
        printf("rd byte %x, rd_pos[%d], wr_pos[%d]\n", v, pcb->rd_pos, pcb->wr_pos);
    }
    printf("\n");
    assert(CIRC_BUFF_IS_EMPTY(pcb));
    assert(!CIRC_BUFF_IS_FULL(pcb));
    printf("2 count to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_CNT_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    printf("2 space to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_SPACE_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    assert(0 == (CIRC_BUFF_SPACE_TO_END(pcb)));
    assert(0 == (CIRC_BUFF_CNT_TO_END(pcb)));

    unsigned char tmp[] = "0123456789";
    circ_buff_ncopy(pcb, tmp, strlen((char *)tmp));
    assert(!CIRC_BUFF_IS_EMPTY(pcb));
    assert(!CIRC_BUFF_IS_FULL(pcb));
    printf("3 count to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_CNT_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    printf("3 space to end %d, rd_pos[%d], wr_pos[%d]\n", CIRC_BUFF_SPACE_TO_END(pcb), pcb->rd_pos, pcb->wr_pos);
    return 0;
}
