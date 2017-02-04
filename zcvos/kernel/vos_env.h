/*
 * vos_env.h
 *
 *    software environment (message manager, config manager, etc.)
 *      for an entity.
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 */

#ifndef VOS_ENVIRNMENT_H_
#define VOS_ENVIRNMENT_H_

#include "parser.h"
#include "syntax.h"
#include "vos_types.h"
#include "vos_metablk.h"

struct _vos_env
{
    system_t                system;
    vos_ptr                 task_m;
    vos_ptr  *t_sys[SYS_TASK_BUTT];
    vos_ptr                    arg;
};

vos_int init_env(vos_str conf, vos_ptr arg);

vos_int open_env(vos_ptr arg);

#endif /* VOS_ENVIRNMENT_H_ */

