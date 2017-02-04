/**
 * @brief 环形缓冲区
 *
 * 环形缓冲区是用来实现读写无锁机制的一种数据结构，多用于生产者-消费者模型中。
 * 在只有一个读和一个写的情况下，可以不使用锁机制；但在多个读之间或者多个写之间
 * 仍然需要调用者自行处理竞争关系。
 *
 * Copyright(C) ShaanXi ZiChen Tech. Ltd. Co. 2011-2015.  ALL RIGHTS RESERVED!
 *
 * @file vos_circbuf.h
 * @author kevin.z.y
 * @date  20140210
 */

#ifndef VOS_CIRCBUF_H
#define VOS_CIRCBUF_H

#include "vos_types.h"

/**
 * @brief 环形缓冲区
 *
 * 从Linux内核源码移植而来，采用"减一"方式的环形缓冲区，同时
 * 为了提高效率，约定
 *    - 数据区大小为1<<N，实际存储大小为1<<N-1
 *    - 缓冲区为空时读与写的偏移量指向同一位置
 *    - 缓冲区为满时写偏移量在读偏移量的"前一个"位置
 *
 */
typedef struct 
{
	vos_ucptr             base;     /**<@brief 实际数据存储区域 */
    vos_int             rd_pos;     /**<@brief 读偏移量 */
    vos_int             wr_pos;     /**<@brief 写偏移量 */
    vos_int               size;     /**<@brief 缓冲区容量为1<<N */
} vos_circ_buff;

/**
 * @brief 初始化环形缓冲区
 *
 * @param[in,out] cf 环形缓冲区指针
 * @param[in] data 存储实际数据的空间
 * @param[in] bits 数据区大小为1<<bits，data的空间不能小于1<<bits
 */
#define INIT_CIRC_BUFF(cf, data, bits) \
do {\
(cf)->base = data; \
(cf)->rd_pos = 0; \
(cf)->wr_pos = 0; \
(cf)->size = 1 << bits; \
} while(0)

/**
 * @brief 重置环形缓冲区，将读写指针都指向起始位置
 *
 * @param[in,out] cf 环形缓冲区指针
 */
#define RESET_CIRC_BUFF(cf) \
do {\
(cf)->rd_pos = 0; \
(cf)->wr_pos = 0; \
} while(0)

/**
 * @brief 计算环形缓冲区中的两个偏移量之间的字节数
 *
 * @param[in] start 起始偏移量
 * @param[in] end 终止偏移量
 * @param[in] size 环形缓冲区大小
 */
#define CIRC_BUFF_CNT(start, end, size) \
(((end) - (start)) & ((size) - 1))

/**
 * @brief 判断缓冲区是否为空
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_IS_EMPTY(cf) \
((cf)->rd_pos == (cf)->wr_pos)

/**
 * @brief 判断缓冲区是否为满
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_IS_FULL(cf) \
(((cf)->size - 1) == CIRC_BUFF_CNT ((cf)->rd_pos, (cf)->wr_pos, (cf)->size))

/**
 * @brief 计算环形缓冲区中有效数据的字节数(在读写偏移量之间)
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_LENGTH(cf) \
CIRC_BUFF_CNT((cf)->rd_pos,(cf)->wr_pos,(cf)->size)

/**
 * @brief 计算环形缓冲区中剩余控件的字节数(在写偏移量之"后"，读偏移量之"前")
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_SPACE(cf) \
CIRC_BUFF_CNT((cf)->wr_pos, ((cf)->rd_pos + 1), (cf)->size)

/**
 * @brief 将读偏移量增加指定偏移量(读取数据)
 *
 * @param[in,out] cf 环形缓冲区指针
 * @param[in] offset 偏移量
 */
#define CIRC_BUFF_INCR_RD(cf, offset) \
((cf)->rd_pos = ((cf)->rd_pos + offset) &((cf)->size - 1))

/**
 * @brief 将写偏移量增加指定偏移量(写入数据)
 *
 * @param[in,out] cf 环形缓冲区指针
 * @param[in] offset 偏移量
 */
#define CIRC_BUFF_INCR_WR(cf, offset) \
((cf)->wr_pos = ((cf)->wr_pos + offset) &((cf)->size - 1))

/**
 * @brief 计算自读偏移量开始连续的有效数据字节数
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_CNT_TO_END(cf) \
        (((cf)->wr_pos >= (cf)->rd_pos) ? ((cf)->wr_pos - (cf)->rd_pos) : ((cf)->size - (cf)->rd_pos))

/**
 * @brief 计算自写偏移量开始连续的有效空间字节数
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_SPACE_TO_END(cf) \
        ((0 == (cf)->rd_pos) ? ((cf)->size - 1 - (cf)->wr_pos) \
                : (((cf)->wr_pos >= (cf)->rd_pos) ? ((cf)->size - (cf)->wr_pos) \
                        : ((cf)->rd_pos - 1 - (cf)->wr_pos)))

/**
 * @brief 获得环形缓冲区读偏移量
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_GET_RD(cf) \
(vos_ucptr)&((cf)->base[(cf)->rd_pos])

/**
 * @brief 获得环形缓冲区写偏移量
 *
 * @param[in] cf 环形缓冲区指针
 */
#define CIRC_BUFF_GET_WR(cf)  \
(vos_ucptr)&((cf)->base[(cf)->wr_pos])

/**
 * @brief 将指定数据写入缓冲区，直至数据写完或者缓冲区满。【非线程安全】
 *
 * @param[in,out] cf 环形缓冲区指针
 * @param[in,out] src 数据源指针，会按需移动，最终为剩余数据
 * @param[in,out] size 数据大小，会按需减少，最终为剩余数据大小
 */
#define CIRC_BUFF_WRITE_BYTES(cf, src, size) \
({while (!CIRC_BUFF_IS_FULL(cf) && size > 0) \
{\
    int elen = CIRC_BUFF_SPACE_TO_END(cf); \
    vos_debug("src=%p, elen=%d, size=%d, rd=%d, wr=%d\n", src, elen, size, (cf)->rd_pos, (cf)->wr_pos); \
    if (0 == elen) {break;}/*if (0 == elen) {CIRC_BUFF_INCR_WR(cf, 1); continue;}*/ \
    elen = elen > size ? size : elen; \
    memcpy(CIRC_BUFF_GET_WR(cf), src, elen); \
    size -= elen; \
    src += elen; \
    CIRC_BUFF_INCR_WR(cf, elen); \
}})

/**
 * @brief 将缓冲区指定大小有效数据读入指定数据区，直至缓冲区空【非线程安全】
 *
 * @param[in,out] cf 环形缓冲区指针
 * @param[in,out] dst 数据区指针，会按需移动，最终为数据尾
 * @param[in,out] size 数据大小，会按需减少，最终为剩余大小
 */
#define CIRC_BUFF_READ_BYTES(cf, dst, size) \
({while (!CIRC_BUFF_IS_EMPTY(cf) && size > 0) \
{\
    int elen = CIRC_BUFF_CNT_TO_END(cf); \
    vos_debug("dst=%p, elen=%d, size=%d, rd=%d, wr=%d.\n", dst, elen, size, (cf)->rd_pos, (cf)->wr_pos); \
    if (0 == elen) {break;} /*if (0 == elen) {CIRC_BUFF_INCR_RD(cf, 1); continue;}*/ \
    elen = elen > size ? size : elen; \
    memcpy(dst, CIRC_BUFF_GET_RD(cf), elen); \
    size -= elen; \
    dst += elen; \
    CIRC_BUFF_INCR_RD(cf, elen); \
}})

#endif /* VOS_CIRCBUF_H */

