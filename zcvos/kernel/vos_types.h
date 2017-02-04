/*
 * vos_types.h
 *
 * some types copied from Linux kernel source, while others introduced in vos version 2.
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 *
 * -----------------------------------------------------------------
 *   2013-12-10          kevin                    more vos_***
 * -----------------------------------------------------------------
 */
#ifndef VOS_TYPES_H_
#define VOS_TYPES_H_

#include <sys/types.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stddef.h>             /* offsetof() */
#include <errno.h>
#include <string.h>

#include "vos_error.h"

/*
 * system task type
 */
typedef enum
{
    SYS_TASK_LOG        =   0,
    SYS_TASK_TIMER      =   1,
    SYS_TASK_BUTT
} SYS_TASK_ENUM;

//#include "vos_strfunc.h"

/*
 * msgblk manager setting begin
 */
#ifndef DEBUG
#define MAX_MSG_NUM         256*1024
#define MAX_DBLK_NUM        512*1024
#define MAX_DBLK_SIZE       1024
#else
#define MAX_MSG_NUM         20
#define MAX_DBLK_NUM        20
#define MAX_DBLK_SIZE       1024
#endif
/*
 * msgblk manager setting end
 */

/*
 * Now the declaration can be written this way:
 *        array (pointer (char), 4) y;
 */
#define pointer(T)  typeof(T *)
#define array(T, N) typeof(T [N])

typedef unsigned char       vos_u8;
typedef unsigned short     vos_u16;
typedef unsigned int       vos_u32;
typedef unsigned long long vos_u64;
typedef char                vos_i8;
typedef short              vos_i16;
typedef int                vos_i32;
typedef long long          vos_i64;

typedef int                vos_int;
typedef unsigned int      vos_uint;
typedef char *             vos_str;
typedef void *             vos_ptr;
typedef unsigned char *  vos_ucptr;
typedef unsigned int *   vos_uiptr;

typedef size_t            vos_size;
typedef ssize_t          vos_ssize;

#define vos_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define vos_align_ptr(p, a) \
    (vos_u8 *) (((vos_uiptr) (p) + ((vos_uiptr) a - 1)) & ~((vos_uiptr) a - 1))

#ifdef DEBUG
#define vos_debug(fmt, ...) \
do {\
    fprintf(stderr, fmt, ##__VA_ARGS__);\
} while(0);
#else
#define vos_debug(fmt, ...)
#endif

#define vos_tolower(c)      (vos_u8) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define vos_toupper(c)      (vos_u8) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_MASK(x, mask) (((x) + (mask))&~(mask))
#define PTR_ALIGN(p, a) ((typeof(p))ALIGN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)

#define INVALID_INDEX               -1

typedef struct _vos_env vos_env;

extern vos_env *g_env;

/*
 * query would stop if row_handler return value in NON-ZERO
 */
typedef vos_int (*row_handler)(vos_ptr params, vos_int column_size, vos_str* column_value, vos_str* column_name);

#endif /* VOS_TYPES_H_ */
