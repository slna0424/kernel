/*
 * vos_env.c
 *
 *    software environment (message manager, config manager, etc.)
 *      for an entity.
 *
 *  Created on: 2012-5-29
 *      Author: kevin
 */

#include <stdlib.h>
#include <string.h>

#include "vos_env.h"
#include "vos_msgblk.h"
#include "vos_task.h"
#include "vos_dblk_m.h"

/*
 * global variables begin
 */
vos_env *g_env = NULL;

vos_msgblk_m *g_msg_m = NULL;

vos_int g_task_id = 1;
/*
 * global variables end
 */

/*
 * init entity environment
 *    [NOTE] modify MAX_MSG_NUM, MAX_DBLK_NUM, MAX_DBLK_SIZE if needed
 */
vos_int init_env(vos_str conf, vos_ptr arg)
{
    if (NULL == conf) return VOS_ERROR;

    // config file
    g_env = (vos_env *)malloc(sizeof(vos_env));
    if (NULL == g_env) return VOS_MEM_ERROR;

    snprintf(g_env->system.name, 255, "%s", conf);
    vos_debug("init env with %s\n", conf);

    if( 0 != open_parser(&(g_env->system)))    
    {        
        vos_debug("failed to load config file %s.\n", g_env->system.name);
        return VOS_FILE_IO;    
    }

    // task manager
    if (0 != vos_tm_instance(7, (vos_task_m **)&g_env->task_m))
    {
        vos_debug("failed to create task manager.\n");
        return VOS_FILE_IO; 
    }
    
    // t_sys
    for (vos_int i = 0; i < SYS_TASK_BUTT; i++)
        g_env->t_sys[i] = NULL;

    // arg
    g_env->arg = arg;

    return VOS_OK;
}

/*
 *  open entity environment
 *     [NOTE] please call DEFINE_SYS_TASK, DEFINE_TASK before open_env
 *     [NOTE2] if DBLK_M_TYPE is NOT vos_pool_m, you may do more preparation before open_env
 *             in case vos_mm_m, f_name should be assgined properly.
 */
vos_int open_env(vos_ptr arg)
{
    if (NULL == g_env) return VOS_ERROR;
    
    // system tasks
    /*for (vos_int i = 0; i < SYS_TASK_BUTT; i++)
    {
        if (NULL != g_env->t_sys[i])
        {
            if (0 != vos_task_open((vos_task*)(g_env->t_sys[i]), arg))
            {
                vos_debug("Failed to open system task %s.\n", ((vos_task*)(g_env->t_sys[i]))->task_name);
                return -1;
            }
        }
    }*/

    // app tasks
    if(0 != vos_tm_open(g_env->task_m, arg)) return VOS_ERROR;

    return vos_tm_loop(g_env->task_m);
}

