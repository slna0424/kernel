/*
 ============================================================================
 Name        : vos_kernel.h
 Author      : kevin.z.y 
 Version     : 1.0 
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2013. 
                    ALL RIGHTS RESERVED! 
 Description : header files used in vos2
 ============================================================================
 */
#ifndef VOS_KERNEL_H_
#define VOS_KERNEL_H_

//TODO: make file check kernel version > 2.6.25
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

// common definitions
#include "vos_types.h"
#include "vos_error.h"

// data structure
#include "vos_list.h"
//#include "vos_plist.h"
#include "vos_hash.h"
#include "vos_jhash.h"

// composite types
#include "vos_bitmap.h"
#include "vos_metablk.h"
#include "vos_dblk.h"
#include "vos_dblk_m.h"
#include "vos_msgblk.h"
#include "vos_tokenblk.h"
#include "vos_mm_m.h"
#include "vos_pool_m.h"
//#include "vos_rb_dblk.h"
#include "vos_msg_queue.h"
#include "vos_hashblk.h"

// basic component
#include "vos_env.h"
#include "vos_task.h"

#endif /* VOS_KERNEL_H_ */
