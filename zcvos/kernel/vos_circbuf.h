/**
 * @brief ���λ�����
 *
 * ���λ�����������ʵ�ֶ�д�������Ƶ�һ�����ݽṹ��������������-������ģ���С�
 * ��ֻ��һ������һ��д������£����Բ�ʹ�������ƣ����ڶ����֮����߶��д֮��
 * ��Ȼ��Ҫ���������д�������ϵ��
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
 * @brief ���λ�����
 *
 * ��Linux�ں�Դ����ֲ����������"��һ"��ʽ�Ļ��λ�������ͬʱ
 * Ϊ�����Ч�ʣ�Լ��
 *    - ��������СΪ1<<N��ʵ�ʴ洢��СΪ1<<N-1
 *    - ������Ϊ��ʱ����д��ƫ����ָ��ͬһλ��
 *    - ������Ϊ��ʱдƫ�����ڶ�ƫ������"ǰһ��"λ��
 *
 */
typedef struct 
{
	vos_ucptr             base;     /**<@brief ʵ�����ݴ洢���� */
    vos_int             rd_pos;     /**<@brief ��ƫ���� */
    vos_int             wr_pos;     /**<@brief дƫ���� */
    vos_int               size;     /**<@brief ����������Ϊ1<<N */
} vos_circ_buff;

/**
 * @brief ��ʼ�����λ�����
 *
 * @param[in,out] cf ���λ�����ָ��
 * @param[in] data �洢ʵ�����ݵĿռ�
 * @param[in] bits ��������СΪ1<<bits��data�Ŀռ䲻��С��1<<bits
 */
#define INIT_CIRC_BUFF(cf, data, bits) \
do {\
(cf)->base = data; \
(cf)->rd_pos = 0; \
(cf)->wr_pos = 0; \
(cf)->size = 1 << bits; \
} while(0)

/**
 * @brief ���û��λ�����������дָ�붼ָ����ʼλ��
 *
 * @param[in,out] cf ���λ�����ָ��
 */
#define RESET_CIRC_BUFF(cf) \
do {\
(cf)->rd_pos = 0; \
(cf)->wr_pos = 0; \
} while(0)

/**
 * @brief ���㻷�λ������е�����ƫ����֮����ֽ���
 *
 * @param[in] start ��ʼƫ����
 * @param[in] end ��ֹƫ����
 * @param[in] size ���λ�������С
 */
#define CIRC_BUFF_CNT(start, end, size) \
(((end) - (start)) & ((size) - 1))

/**
 * @brief �жϻ������Ƿ�Ϊ��
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_IS_EMPTY(cf) \
((cf)->rd_pos == (cf)->wr_pos)

/**
 * @brief �жϻ������Ƿ�Ϊ��
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_IS_FULL(cf) \
(((cf)->size - 1) == CIRC_BUFF_CNT ((cf)->rd_pos, (cf)->wr_pos, (cf)->size))

/**
 * @brief ���㻷�λ���������Ч���ݵ��ֽ���(�ڶ�дƫ����֮��)
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_LENGTH(cf) \
CIRC_BUFF_CNT((cf)->rd_pos,(cf)->wr_pos,(cf)->size)

/**
 * @brief ���㻷�λ�������ʣ��ؼ����ֽ���(��дƫ����֮"��"����ƫ����֮"ǰ")
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_SPACE(cf) \
CIRC_BUFF_CNT((cf)->wr_pos, ((cf)->rd_pos + 1), (cf)->size)

/**
 * @brief ����ƫ��������ָ��ƫ����(��ȡ����)
 *
 * @param[in,out] cf ���λ�����ָ��
 * @param[in] offset ƫ����
 */
#define CIRC_BUFF_INCR_RD(cf, offset) \
((cf)->rd_pos = ((cf)->rd_pos + offset) &((cf)->size - 1))

/**
 * @brief ��дƫ��������ָ��ƫ����(д������)
 *
 * @param[in,out] cf ���λ�����ָ��
 * @param[in] offset ƫ����
 */
#define CIRC_BUFF_INCR_WR(cf, offset) \
((cf)->wr_pos = ((cf)->wr_pos + offset) &((cf)->size - 1))

/**
 * @brief �����Զ�ƫ������ʼ��������Ч�����ֽ���
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_CNT_TO_END(cf) \
        (((cf)->wr_pos >= (cf)->rd_pos) ? ((cf)->wr_pos - (cf)->rd_pos) : ((cf)->size - (cf)->rd_pos))

/**
 * @brief ������дƫ������ʼ��������Ч�ռ��ֽ���
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_SPACE_TO_END(cf) \
        ((0 == (cf)->rd_pos) ? ((cf)->size - 1 - (cf)->wr_pos) \
                : (((cf)->wr_pos >= (cf)->rd_pos) ? ((cf)->size - (cf)->wr_pos) \
                        : ((cf)->rd_pos - 1 - (cf)->wr_pos)))

/**
 * @brief ��û��λ�������ƫ����
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_GET_RD(cf) \
(vos_ucptr)&((cf)->base[(cf)->rd_pos])

/**
 * @brief ��û��λ�����дƫ����
 *
 * @param[in] cf ���λ�����ָ��
 */
#define CIRC_BUFF_GET_WR(cf)  \
(vos_ucptr)&((cf)->base[(cf)->wr_pos])

/**
 * @brief ��ָ������д�뻺������ֱ������д����߻��������������̰߳�ȫ��
 *
 * @param[in,out] cf ���λ�����ָ��
 * @param[in,out] src ����Դָ�룬�ᰴ���ƶ�������Ϊʣ������
 * @param[in,out] size ���ݴ�С���ᰴ����٣�����Ϊʣ�����ݴ�С
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
 * @brief ��������ָ����С��Ч���ݶ���ָ����������ֱ���������ա����̰߳�ȫ��
 *
 * @param[in,out] cf ���λ�����ָ��
 * @param[in,out] dst ������ָ�룬�ᰴ���ƶ�������Ϊ����β
 * @param[in,out] size ���ݴ�С���ᰴ����٣�����Ϊʣ���С
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

