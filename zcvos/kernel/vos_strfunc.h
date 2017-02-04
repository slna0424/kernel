/*
 * vos_strfunc.h
 *      all funcstion are copied from nginx 1.4
 *
 *  Created on: 2013-12-10
 *      Author: kevin
 * 
 */

#ifndef VOS_STR_FUNC_H_
#define VOS_STR_FUNC_H_

#include <regex.h>
#include <sys/time.h>
#include <time.h>
#include "vos_kernel.h"

vos_str str_split (vos_str * string,  const vos_str token);

void str_trim(vos_str str);

vos_int regex_valid(const vos_str pattern, const vos_str str);

vos_ucptr vos_hex2str(vos_ucptr dst, vos_ucptr src, vos_size len);

void vos_encode_base64(vos_str *dst, vos_str *src);

vos_int vos_decode_base64(vos_str *dst, vos_str *src);

vos_int vos_decode_base64url(vos_str *dst, vos_str *src);

vos_int vos_decode_base64_internal(vos_str *dst, vos_str *src, const vos_ucptr basis);

vos_uint vos_utf8_decode(vos_ucptr *p, vos_size n);

vos_size vos_utf8_length(vos_ucptr p, vos_size n);

vos_ucptr vos_utf8_cpystrn(vos_ucptr dst, vos_ucptr src, vos_size n, vos_size len);

/*
 * zyh add
 */
vos_int right_zero(vos_str str1,vos_int len,vos_str str2) ;

vos_int left_zero(vos_str str1,vos_int len,vos_str str2);

void getlocal_time_date(vos_int flag,vos_str arg,vos_int len);

#endif /* VOS_vos_dblk_H_ */
