/*
 * reactor_task.h
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 */

#ifndef REACTOR_TASK_H_
#define REACTOR_TASK_H_

#include "../kernel/vos_types.h"

#include <sys/signalfd.h>   // 2.6.22
#include <signal.h>

#include <sys/timerfd.h>    // 2.6.25
#include <sys/time.h>  

#include <sys/eventfd.h>    // 2.6.22

#include <sys/epoll.h>

#include <fcntl.h>          /* nonblocking */
#include <sys/resource.h>   /*setrlimit */

typedef enum
{
    VOS_FD_SOCKET,
    VOS_FD_SIGNAL,
    VOS_FD_TIMER,
    VOS_FD_EVENT,
    VOS_FD_BUTT
} VOS_FD_TYPE_ENUM;

#define VOS_EVENT_IN        1<<0
#define VOS_EVENT_OUT       1<<1
#define VOS_EVENT_PRI       1<<2
#define VOS_EVENT_ERROR     1<<3
#define VOS_EVENT_HUP       1<<4

typedef struct
{
    vos_i32                   fd;
    vos_i32                 type;
    vos_u32               events;
    void                    *arg;
} VOS_FD_STRU;

typedef vos_int (*vos_handler) (VOS_FD_STRU *fd, vos_int event);

typedef struct
{
    i32                     size;
    i32                    count;
    struct epoll_event   *events;
    i32                      efd;
} VOS_EPOLL_STRU;



#endif /* REACTOR_TASK_H_ */

