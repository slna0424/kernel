/*===========================================================================
 * name			: sock_task.h
 * Author 		: zyh
 * Version		: v1.0
 * Created on	: 2013-6-8
 * Copyright   	: Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2013-6-8.
 *                       ALL RIGHTS RESERVED! 
 *
 * Description :
 *===========================================================================
 */

#ifndef SOCK_TASK_H_
#define SOCK_TASK_H_

#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include<sys/time.h>
#include<unistd.h>
#include <strings.h>

#include "../kernel/vos_kernel.h"
#include "logtask.h"

//max event
#define MAX_EVENTS 512

//all loop count
#define MAX_LOOP	3

//all sleep time
#define MAX_SLEEP		 1

//listen number
#define LISTEN_NUM		5

//buff size
#define BUFF_SIZE     1024


// operation msg ,continue,change status,closed sock,

#define SOCK_PCCC_YNNN  0x1000	// operation msg  and  interrupt operation  and not change sock status and not closed sock
#define SOCK_PCCC_YNNY	0x1001	// operation msg  and  interrupt operation  and not change sock status and closed sock
#define SOCK_PCCC_YNYN  0x1010	// operation msg  and  interrupt operation  and  change sock status and not closed sock

#define SOCK_PCCC_YNYY  0x1011    // operation msg  and  interrupt operation  and  change sock status and closed sock

#define SOCK_PCCC_YYNN  0x1100   // operation msg  and  continue to operation  and  not change sock status and not closed sock
#define SOCK_PCCC_YYNY  0x1101   // operation msg  and  continue to operation  and  not change sock status and  closed sock
#define SOCK_PCCC_YYYN  0x1110   // operation msg  and  continue to operation  and  change sock status and not closed sock

#define SOCK_PCCC_YYYY  0x1111   // operation msg  and  continue to operation  and  change sock status and closed sock

#define SOCK_PCCC_NNNN  0x0000   // not operation msg  and  interrupt operation  and not change sock status and not closed sock
#define SOCK_PCCC_NNNY  0x0001   // not operation msg  and  interrupt operation  and not change sock status and  closed sock
#define SOCK_PCCC_NNYN  0x0010   // not operation msg  and  interrupt operation  and change sock status and not closed sock

#define SOCK_PCCC_NNYY  0x0011   // not operation msg  and  interrupt operation  and change sock status and closed sock

#define SOCK_PCCC_NYNN  0x0100	// not operation msg  and continue to operation and not change sock status and not closed sock
#define SOCK_PCCC_NYNY  0x0101   // not operation msg  and interrupt operation and not change sock status and closed sock
#define SOCK_PCCC_NYYN  0x0110	// not operation msg  and continue to operation and  change sock status and not closed sock
#define SOCK_PCCC_NYYY  0x0111	// not operation msg  and continue to operation and  change sock status and  closed sock


#ifdef SOCKCLIENT
#define	INFOMSG			"[sockclient]"
#else
#define	INFOMSG			"[sockserv]"
#endif

/*
 * description:
 * 		Applied to the link  before
 * 		If this task application and client, and is a short link,the message is returned to 0, no message returns -1.
 * 		if it it long link,always return 0.
 * 		If this task application and server, you must return to not  zero  or setting this callback function is NULL
 * parameter:
 * 		@task
 * 		@linkfd,The linked descriptor
 * 		@arg
 * return:
 * 		-1 not link,0 link,
 */
typedef	int (*pre_sock)(vos_task *task,void *arg);

/*
 * description:
 * 		applied to the link  after
 * parameter:
 * 		@task
 * 		@linkfd,The linked descriptor
 * 		@arg
 * return:
 * 		SOCK_PCCC_NNNY closed the sock,
 *      SOCK_PCCC_NNNN success
 *      default,SOCK_PCCC_NNNY
 */
typedef	int (*after_sock)(vos_task *task,int linkfd,void *arg);
/*
 * description:
 * 		Applied to the send  after
 * parameter:
 * 		@task
 * 		@linkfd,The linked descriptor
 * 		@mb send msg
 * 		@arg
 * return:
 *     SOCK_PCCC_NNNN, continue and not pop msg
 *     SOCK_PCCC_NNNY, closed the sock
 *     SOCK_PCCC_NNYN,  changed to recv msg
 *     SOCK_PCCC_NYNN, continue send msg and pop a msg
 *     SOCK_PCCC_NYNY, continue send msg until sending end and closed sock
 *     SOCK_PCCC_NYYN, continue send msg until sending end and changed to recv msg and not closed sockd
 *     SOCK_PCCC_NYYY, continue send msg until sending end and changed to recv msg and  closed sockd
 *
 */
typedef int (*after_send)(vos_task *task,vos_msgblk *mb,int linkfd,void *arg);

/*
 * description:
 * 		Applied to the send  before
 * parameter:
 * 		@task
 * 		@linkfd,The linked descriptor
 * 		@arg
 * return:
 *		SOCK_PCCC_NNNY,close sock
 *		SOCK_PCCC_NNNN,send msg
 *		SOCK_PCCC_NNYN,not send msg and changed to recv msg
 */
typedef int (*before_send)(vos_task *task,vos_msgblk *mb,int linkfd,void *arg);

/*
 * description:
 * 		applied to the recv after
 * parameter:
 * 		@task
 * 		@mb    recv msgblock
 * 		@linkfd
 * 		@arg
 * return:
 * 		first place,Whether to push msg to next task,0 false,1 true
 * 		second place,Whether to interrupt operation,0 false,1 true
 *      third place,Whether to change socked status,0 false,1 true
 *      fourth place,Whether to close sock,0 false,1 true
 */
typedef int(*after_recv)(vos_task *task,vos_msgblk *mb,int linkfd,void *arg);

/*
 * description:
 * 		applied to the recv after
 * parameter:
 * 		@task
 * 		@mb    recv msgblock
 * 		@arg
 * return:
 * 		SOCK_PCCC_NNNY,close sock
 *		SOCK_PCCC_NNNN,recv msg
 *		SOCK_PCCC_NNYN,not recv msg and changed to send msg
 */
typedef int(*before_recv)(vos_task *task,int linkfd,void *arg);

/*
 * description:
 * 		applied to the check timeout
 * parameter:
 * 		@task
 * 		@linkfd,The linked descriptor
 * 		@arg
 * return:
 *	    SOCK_PCCC_NNNY ,closed the sock
 *	    SOCK_PCCC_NNNN , success
 *	    SOCK_PCCC_NNYY, reconnect
 */
typedef	int (*check_sock)(vos_task *task,int linkfd,void *arg);

//
typedef struct _conn_s_ 
{
//#ifdef SOCKCLIENT
	// the ip of listen or link
	char			host[24];
	//the port of listen or link
	int					port;
	//timieout,sec
	int				 timeout;
//#else
	char			servhost[24];
	int					servport;
//	int				 servtimeout;
//#endif
}CONN_STRU;

typedef enum
{
    SOCK_INIT       = 0,
    SOCK_RUN     	= 1,
    SOCK_DIS      	= 2,
    SOCK_BUTT
}SOCK_STATUS;

//
typedef struct _socket_event
{
	// listen fd
    int 		 			 fd;
    int 	 		 	 events;
    //user data
    void 	   		   	   *arg;
    //event state ,init 0,run 1,disable 2
    int 	 		 	 status;
    // recv data buffer
    vos_msgblk   		  	  *buff;

    // last active time
    long 			 last_active;

    //callback func
    void (*call_back)(int fd, int events, void *arg);
} sock_event;


typedef struct _socket_task
{
	vos_task                       t;
	// globale epoll description
	int                      epollfd;

	CONN_STRU                 config;
	sock_event              *myevent;
	struct epoll_event       *events;

	//num of myevent
	int                        count;
	void                        *arg;

	//applied to the sock linked before
	pre_sock                 presock;
	//applied to the sock link after
	after_sock             aftersock;
	//applied to check socked
	check_sock             checksock;
	//applied to the send msg before
	before_send           beforesend;
	//applied to the send msg after
	after_send             aftersend;
	//applied to the recv msg before
	before_recv           beforerecv;
	//applied to the recv msg after
	after_recv             afterrecv;

} sock_task;


static inline void *sock_task_svc(void *arg);

static void sendData(int fd, int events, void *arg);
static void recvData(int fd, int events, void *arg);
static inline int _close_sock_(sock_task *s_sock,sock_event *ev);
static int initListenSocket(sock_task *s_sock) ;

/*
 * description:
 * 			encrypted sock
 * parameter:
 * 		@fd  sock description
 * return:
 * 		return -1 if failed, return encrypted sock if successed
 */
static inline unsigned char sock_encrypt(int fd)
{
	return  ~fd ^ 0x0090;
}

/*
 * description:
 * 		Analysis of sock encryption
 * parameter:
 * 		@encry encryptied sock
 * return:
 * 		return -1 if failed, return sock description if successed
 */
static inline int sock_decrypt(unsigned char encry)
{
	unsigned char res = ~(encry ^ 0x0090);
	return (int)res ;
}

/*
 * description:
 * 			get the sockconfig
 * paramter:
 * 		@system
 * 		@task
 * return£º
 * 		-1,failed 0,success
 */
static inline int get_sock_conf(system_t *system,CONN_STRU *pconfig)
{
	if(NULL == system || NULL == pconfig) return -1;
	char  *ip 	= NULL;
	long int *port = NULL;
	long int *timeout = NULL;

//#ifdef SOCKCLIENT
	//ip
	if( 0 != get_value_string(system, "sockip", 0, &ip))
	{
		vos_debug("%s get the sockip failed.\n",INFOMSG);
		return -1;
	}
	snprintf(pconfig->host,24,"%s",ip);

	//port
	if( 0 != get_value_numeric(system, "sockport", 0, &port))
	{
		vos_debug("%s get the sockport failed \n",INFOMSG);
		return -1;
	}
	pconfig->port = (int)*port ;

	//timeout
	if( 0 != get_value_numeric(system, "sockout", 0, &timeout))
	{
		vos_debug("%s get the sockout failed \n",INFOMSG);
		return -1;
	}
	pconfig->timeout = (int)*timeout ;
	vos_debug("%s sockaddress:[%s : %d] \n",INFOMSG,pconfig->host,pconfig->port);
//#else
	//ip
	if( 0 != get_value_string(system, "sockservip", 0, &ip))
	{
		vos_debug("%s get the sockip failed.\n",INFOMSG);
		return -1;
	}
	snprintf(pconfig->servhost,24,"%s",ip);

	//port
	if( 0 != get_value_numeric(system, "sockservport", 0, &port))
	{
		vos_debug("%s get the sockport failed \n",INFOMSG);
		return -1;
	}
	pconfig->servport = (int)*port ;

	//timeout
//	if( 0 != get_value_numeric(system, "sockservout", 0, &timeout))
//	{
//		vos_debug("%s get the sockout failed \n",INFOMSG);
//		return -1;
//	}
//	pconfig->servtimeout = (int)*timeout ;
//	vos_debug("%s sockaddress:[%s : %d] \n",INFOMSG,pconfig->servhost,pconfig->servport);
//#endif
	//free
	free(ip);
	free(port);
	free(timeout);

	return 0;
}

/*
 * desc:
 * 		Get the current time
 * parameter:
 * return:
 * 	success,current time,failed,-1
 */
static inline long int getTime()
{
	struct timeval 	tv;
	if(!gettimeofday(&tv,NULL))
	{
		return tv.tv_sec;
	}
	return -1;
}

/*
 * description:
 * 		set sock opttion
 * parameter:
 * return:
 */
static inline void _set_sock_opt_()
{
	return ;
}


/*
 * desc:
 * 		set nonblock mode
 * parameter:
 * 			files descriptor
 * return:
 *		success greater than zero ,failed,less then zero
 */
static inline int setNonblock(int fd)
{
	 return fcntl(fd, F_SETFL, O_NONBLOCK);
}


/*
 * description:
 * 			set event
 * parameter:
 * 			@ev
 * 			@fd
 * 			@reback func
 * 			@arg
 * return:
 * 			0 success,-1 failed
 */
static int eventSet(sock_event *ev, int fd, void (*call_back)(int, int, void*), void *arg)
{
	if(NULL == ev ) return -1;
    ev->fd 			= fd;
    ev->call_back 	= call_back;
    ev->events 		= 0;
    ev->arg 		= arg;
    ev->status 		= SOCK_INIT;
    ev->last_active = getTime();
    ev->buff 		= NULL;

    return 0;
}

/*
 * description:
 * 			add/mod an event to epoll
 * parameter:
 * 		@epollFd
 * 			the return of epoll_created
 * 		@events
 * 		@ev
 * return:
 * 		-1,failed 0,success
 */
static int eventAdd(int epollFd, int events, sock_event *ev)
{
	if(0 > epollFd || 0 > events || NULL == ev) return -1;
    struct epoll_event epv = {0, {0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->events = events;
    if(ev->status == 1){
        op = EPOLL_CTL_MOD;
    }
    else{
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    if(0 > epoll_ctl(epollFd, op, ev->fd, &epv))
    {
    	  vos_debug("%s Event Add failed[fd=%d], evnets[%d]\n",INFOMSG,ev->fd, events);
    	  return -1;
    }
    vos_debug("%s Event Add OK[fd=%d], op=%d, evnets[%d]\n",INFOMSG,ev->fd, op, events);
    return 0;
}

/*
 * description:
 * 			delete an event from epoll
 * parameter:
 * 			@epollFd
 * 			@ev
 * return:
 * 		-1,failed 0,success
 */
static int eventDel(int epollFd, sock_event *ev)
{
	if(0 > epollFd || NULL == ev) return -1;
    struct epoll_event epv = {0, {0}};
    if(ev->status != 1) return -1;
    epv.data.ptr = ev;
    ev->status = 0;
    if(0 > epoll_ctl(epollFd, EPOLL_CTL_DEL, ev->fd, &epv))
    {
    	vos_debug("%s Event del failed[fd=%d], evnets[%d]\n",INFOMSG,ev->fd, ev->events);
    	return -1;
    }
    vos_debug("%s Event del success[fd=%d], evnets[%d]\n",INFOMSG,ev->fd, ev->events);
    return 0;
}
/*
 * description:
 * 			close the sock
 * parameter:
 * 			@s_sock
 * 			@pos
 * return:
 * 		-1 failed,0 success
 */
static inline int _close_sock_(sock_task *s_sock,sock_event *ev)
{
	if(NULL == s_sock || NULL == ev) return -1;

	//del ev from epoll pool
	eventDel(s_sock->epollfd, ev);

	if(0 < s_sock->count) s_sock->count--;
	//close sock description
	close(ev->fd);

	ev->fd 			= 0;
	ev->events 		= 0;
	ev->status 		= 0;
	ev->last_active = getTime();
	ev->buff 		= NULL;

	return 0;
}

/*
 * description:
 * parameter:
 * 			@s_sock
 * 			@ev
 * 			@type 1 or 2
 * return:
 */
static inline int _change_socksta_(sock_task *s_sock,sock_event *ev ,int type,int fd)
{
	if(NULL == s_sock || NULL == ev || 0 > type || 0 > fd ) return -1;

	// change to send event
	if(0 != eventDel(s_sock->epollfd, ev))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s eventDel failed,type = %d \n",INFOMSG,type);
		return -1;
	}
	if(1 == type)
	{
		if(0 != eventSet(ev, fd, sendData, ev))
		{
			vos_log(&(s_sock->t),LM_ERROR,"%s eventSet failed,type = %d \n",INFOMSG,type);
			return -1;
		}
		if(0 != eventAdd(s_sock->epollfd, EPOLLOUT, ev))
		{
			vos_log(&(s_sock->t),LM_ERROR,"%s eventAdd failed,type = %d \n",INFOMSG,type);
			return -1;
		}
	}
	else
	{
		if(0 != eventSet(ev, fd, recvData, ev))
		{
			vos_log(&(s_sock->t),LM_ERROR,"%s eventSet failed,type = %d \n",INFOMSG,type);
			return -1;
		}
		if(0 != eventAdd(s_sock->epollfd, EPOLLIN, ev))
		{
			vos_log(&(s_sock->t),LM_ERROR,"%s eventAdd failed,type = %d \n",INFOMSG,type);
			return -1;
		}
	}

	return 0;

}

/*
 * description:
 * 			send msg ,
 * parameter:
 * 		@fd	socked description
 * 		@mb	msg block
 * return:
 *		success ,return have send bytes,othre failed
 */
static inline int _send_msg_(int fd, vos_msgblk *mb )
{
	if(0 > fd || NULL == mb) return -1;
	int len = 0,s_offset = 0,s_len = 0;
	//get msg'length
	s_len = (int)dblk_get_len(mb->buff);
	if(0 >= s_len) return 1;
	vos_debug("-----begin send message----\n");
	//send msg
	while(1)
	{
		len = send(fd, mb->buff->rd_ptr + s_offset, s_len - s_offset, 0);

		if(0 < len)
		{
			s_offset += len;
			/*
			 * sends success,
			 * If the return value is equal to the length of the data block,return have send bytes,
			 * or continue to send
			 */
			if(s_len == s_offset)
			{
				vos_debug("send message success,s_offset:[%d] \n",s_offset);
				return s_offset;
			}else{
				continue ;
			}
		}
		else
		{
			//failed,return
			return len;
		}
	}

}

/*
 * description:
 * 		receive data
 * parameter:
 * 		@fd sock description
 * 		@mb the buff to save recv msg
 * 		@len	-1 or recv msg'len
 * return:
 * 		-1 failed, return recv msg'len if success
 */

static inline int _recv_msg_(int fd, vos_msgblk *mb,int len)
{
	if(0 > fd || NULL == mb) return -1;
	//get mb'len
	int size = -1,rlen = -1;
	if(0 >= (size = (int)dblk_get_size(mb->buff))) return -1;

	/*
	 * Equal to the -1, to accept data block size,
	 * length is less than the data block size, accept the specified length data,
	 *  otherwise, the received data block size data
	 */
	if(-1 == len)
	{
		rlen = recv(fd, mb->buff->wr_ptr,size, 0);
	}else{
		size = (size > len ? len : size);
		rlen = recv(fd, mb->buff->wr_ptr,size, 0);
	}

	//move wr_ptr
	if(0 < rlen)mb->buff->wr_ptr += rlen ;
	vos_debug("%s recv msg len:[%d] \n",INFOMSG,rlen);
	return rlen ;
}

/*
 *-1:break,0:continue
 */
static inline int _after_callback_(sock_task *s_sock,vos_msgblk **msg ,int res,int *change,int *closed)
{
	if(NULL == s_sock) return -1;

	vos_log(&(s_sock->t),LM_INFO,"%s into after_callback, res:[%d],change:[%d],closed:[%d] \n",INFOMSG,res,*change,*closed);
	vos_log(&(s_sock->t),LM_INFO,"%s SOCK_PCCC_NNNN:[%d],SOCK_PCCC_NNNY:[%d],SOCK_PCCC_NNYN:[%d],SOCK_PCCC_NNYY:[%d],SOCK_PCCC_NYNN:[%d],SOCK_PCCC_NYYN:[%d],SOCK_PCCC_NYNY:[%d],SOCK_PCCC_NYYY:[%d] \n",
					INFOMSG,SOCK_PCCC_NNNN,SOCK_PCCC_NNNY,SOCK_PCCC_NNYN,SOCK_PCCC_NNYY,SOCK_PCCC_NYNN,SOCK_PCCC_NYYN,SOCK_PCCC_NYNY,SOCK_PCCC_NYYY);

	//push the msg to next task
	if(SOCK_PCCC_YNNN <= res)
	{
		vos_log(&(s_sock->t),LM_INFO,"...after_callback,return gt SOCK_PCCC_YNNN \n");
		if(NULL != s_sock->t.next)
		{
			if (0 != task_put(s_sock->t.next, *msg))
			{
				vos_log(&(s_sock->t),LM_ERROR,"%s put msg to next task failed \n",INFOMSG);
			}
			else
			{
				vos_debug("%s put msg to next task success \n",INFOMSG);
			}
		}
	}
	else
	{
		//pop msg to send
		if(SOCK_PCCC_NYNN <= res && res < SOCK_PCCC_YNNN)
		{
			  vos_log(&(s_sock->t),LM_INFO,"...after_callback,return gt SOCK_PCCC_NYNN and lt SOCK_PCCC_YNNN \n");
			  vos_debug("msg_num = %d \n",s_sock->t.queue_m->msg_num);
			  if(0 < s_sock->t.queue_m->msg_num)
			  {
				  if(0 != msg_queue_pop(s_sock->t.queue_m, msg))
				  {
					  vos_log(&(s_sock->t),LM_ERROR,"%s pop msg failed \n",INFOMSG);
					  return -1 ;
				  }else{
					  vos_log(&(s_sock->t),LM_ERROR,"%s pop msg success \n",INFOMSG);
				  }
			  }else{
				  vos_log(&(s_sock->t),LM_INFO,"%s queue_m has no message \n",INFOMSG);
			  }
		}
	}

	 int result = -1;
	 //handle res
	switch(res)
	{
		// operation msg ,continue,change status,closed sock,
		case  SOCK_PCCC_NNNN:
			result = 0;
			break;
		case  SOCK_PCCC_NNNY:
			*closed = 0;
			break ;
		case  SOCK_PCCC_NNYN:
			*change = 0;
			break;
		case  SOCK_PCCC_NNYY:
			*closed = 0;
			break;
		case  SOCK_PCCC_NYNN:				//continue
			result = 0;
			break;
		case  SOCK_PCCC_NYNY:
//			result = 0;
			*closed = 0;
			break;
		case  SOCK_PCCC_NYYN:
			result = 0;
			*change = 0;
			break;
		case  SOCK_PCCC_NYYY:
//			result  = 0;
			*closed = 0;
			break;


		case  SOCK_PCCC_YNNN:
			break;
		case  SOCK_PCCC_YNNY:
			*closed = 0;
			break;
		case  SOCK_PCCC_YNYN:
			*change = 0;
			break ;
		case  SOCK_PCCC_YNYY:
			*closed = 0;
			break;
		case  SOCK_PCCC_YYNN:
			result = 0;
			break;
		case  SOCK_PCCC_YYNY:
			result = 0;
			*closed = 0;
			break ;
		case  SOCK_PCCC_YYYN:
			result = 0;
			*change = 0;
			break;
		case  SOCK_PCCC_YYYY:
			*closed = 0;
			break ;
		default:
			break;

	}

	vos_log(&(s_sock->t),LM_INFO,"%s after callback,result:[%d],change:[%d],closed:[%d] \n",INFOMSG,result,*change,*closed);
	return result ;
}

static void recvData(int fd, int events, void *arg)
{
	if(0 > fd || 0 > events || NULL == arg) return ;
	sock_task *s_sock = ( sock_task*)arg;
	struct epoll_event *epoll_events = s_sock->events ;
	sock_event *ev = (sock_event *)epoll_events[events].data.ptr;

	vos_log(&(s_sock->t),LM_INFO,"<<<<<<<<<<<<<<<<<%s begin recv msg >>>>>>>>>>>>>>>>>>>>\n",INFOMSG);

    // loop receive data
	int  i_judge_change = -1, i_judge_close = -1,res = -1;
	while(1)
	{
		i_judge_change = -1; i_judge_close = -1;
		 int sign = -1;
		// callback before_recv
		if(NULL != s_sock->beforerecv)
		{
			res = s_sock->beforerecv(&(s_sock->t),ev->fd,s_sock->arg);

			_after_callback_(s_sock,NULL ,res,&i_judge_change,&i_judge_close);

			if(0 == i_judge_change || 0 == i_judge_close) break;

		}


		//	create msg block
		vos_msgblk *msg = NULL;

		msg = msg_create_force(s_sock->t.data_m, BUFF_SIZE, 3, 200);
		if (NULL == msg)
		{
			vos_log(&(s_sock->t),LM_ERROR,"%s create msg failed \n",INFOMSG);
			return ;
		}

		// receive data
		  if(SOCK_RUN != ev->status)
		  {
			  msg_free(msg);
			  break;
		   }
		  int len = _recv_msg_(ev->fd, msg,-1);
		  vos_log(&(s_sock->t),LM_INFO,"%s  _recv_msg_ return %d \n",INFOMSG,len);

		  //handle result

		  if(0 > len)
		  {
			  if(errno == EAGAIN)		//has no msg to recv
				{
					vos_log(&(s_sock->t),LM_WARN,"%s has no msg to recv \n",INFOMSG);
					i_judge_change = 0;
				}
				else
				{
					vos_log(&(s_sock->t),LM_ERROR,"%s recv msg failed \n",INFOMSG);
					i_judge_close = 0;
				}
		  }
		  else if(0 == len)
		  {
			  vos_log(&(s_sock->t),LM_INFO,"%s [fd=%d] pos[%d],closed gracefully \n",INFOMSG,fd,ev->events);
			  i_judge_close = 0;
		  }
		  else
		  {
			  vos_log(&(s_sock->t),LM_INFO,"%s recv msg success \n",INFOMSG);
			  //callback
			   if(NULL == s_sock->afterrecv)
			   {
				   //push the msg to next task
					 if(NULL != s_sock->t.next)
					 {
						if (0 != task_put(s_sock->t.next, msg))
						{
							vos_log(&(s_sock->t),LM_ERROR,"%s put msg to next task failed \n",INFOMSG);
						}
						else
						{
							vos_debug("%s put msg to next task success \n",INFOMSG);
						}
					 }

					 i_judge_change = 0;
			   }
			   else
			   {
				   vos_log(&(s_sock->t),LM_INFO,"%s afterrecv is not NULL,begin handle \n",INFOMSG);

				   //callback ,afterrecv
				    res = s_sock->afterrecv(&(s_sock->t),msg,ev->fd,s_sock->arg);
				   //after callback,continue or break;
				   sign = _after_callback_(s_sock,&msg ,res,&i_judge_change,&i_judge_close);
			   }
		  }

		 vos_log(&(s_sock->t),LM_INFO,"%s after recv msg,sign:[%d],change:[%d],closed:[%d] \n",INFOMSG,sign,i_judge_change,i_judge_close);
		//free
		msg_free(msg);

		if(0 != sign) break;
	} //end while(1)

	vos_log(&(s_sock->t),LM_INFO,"<<<<<<<<<<<<<<<<<%s end recv msg >>>>>>>>>>>>>>>>>>>>\n",INFOMSG);

	// change to send event
	if(0 == i_judge_change)
	{
		vos_log(&(s_sock->t),LM_INFO,"the queue_num:[%d] when change sock to send \n",s_sock->t.queue_m->msg_num);
		_change_socksta_(s_sock,ev ,1,fd);
	}

	//close socked
	if(0 == i_judge_close)	_close_sock_(s_sock,ev);


	return ;
}

/*
 * description:
 * 			send message function
 * parameter:
 * return:
 */
static void sendData(int fd, int events, void *arg)
{
	sock_task *s_sock = ( sock_task*)arg;

	vos_log(&(s_sock->t),LM_INFO,"<<<<<<<<<<<<<<<<<<<<<<<<<<<%s begin  send message >>>>>>>>>>>>>>>>>>>>>>>\n",INFOMSG);

	struct epoll_event *epoll_events = s_sock->events ;
	sock_event *ev = (struct _socket_event*)epoll_events[events].data.ptr;
	 int  i_judge_change, i_judge_close,res = -1;

	 while(1)
	 {
		 i_judge_change = -1; i_judge_close = -1,res = -1;
		 //callback,beforesend
		 if(NULL != s_sock->beforesend)
		 {
			int res = s_sock->beforesend(&(s_sock->t),ev->buff,ev->fd,s_sock->arg);

			_after_callback_(s_sock,NULL,res,&i_judge_change,&i_judge_close);
			// change or close sockfd
			if(0 == i_judge_change || 0 == i_judge_close)
			{
				msg_free(ev->buff);
				ev->buff = NULL;
				break ;
			}

		 }
		 else
		 {
			 // beforesend is NULL
		 }

		 //send message
		 res = _send_msg_(ev->fd,ev->buff);

		 if(0 > res)
		 {
			 //said write buffer queue is full
			if (res == -1 && errno == EAGAIN)
			{
				 vos_log(&(s_sock->t),LM_WARN,"said write buffer queue is full \n");
				 sleep(MAX_SLEEP);
				 continue ;
			}
			//send error,closed the socked
			i_judge_close = 0;
			//free msg
			 msg_free(ev->buff);
			 ev->buff = NULL;
			 vos_log(&(s_sock->t),LM_ERROR,"said write error \n");
			break;
		 }
		 else if(0 == res)
		 {
			 //broken links
			 i_judge_close = 0;
			vos_log(&(s_sock->t),LM_ERROR,"%s broken likes when send msg,fd:[%d] \n",INFOMSG,ev->fd);
			 //free msg
			 msg_free(ev->buff);
			 ev->buff = NULL;
			break;
		 }
		 else
		 {
			 vos_log(&(s_sock->t),LM_INFO,"%s send msg success \n",INFOMSG);
			 //send msg success,callback
			 if(NULL == s_sock->aftersend)
			 {
				 //free msg
				 msg_free(ev->buff);
				 ev->buff = NULL;
				 i_judge_change = 0;
				 break;
			 }
			 else
			 {
				 res = -1;
				 res = s_sock->aftersend(&(s_sock->t),ev->buff,ev->fd,s_sock->arg);
				 vos_log(&(s_sock->t),LM_INFO,"%s aftersend return %d \n",INFOMSG,res);

				 //free msg
				 msg_free(ev->buff);
				 ev->buff = NULL;
				 vos_msgblk *mb = NULL;

				 res = _after_callback_(s_sock,&mb ,res,&i_judge_change,&i_judge_close) ;
				 vos_log(&(s_sock->t),LM_INFO,"%s send msg success,after callback,change:[%d],closed:[%d],res:[%d] \n",INFOMSG,i_judge_change,i_judge_close,res);
				 if(0 != res)
				 {
					 vos_log(&(s_sock->t),LM_ERROR," %s _after_callback return broken \n",INFOMSG);
					 break;
				 }
				 //after callback,continue
				if(NULL != mb)
				{
					vos_log(&(s_sock->t),LM_INFO,"%s mb is not NULL \n",INFOMSG);
					ev->buff = mb ;
					continue ;
				}
				else
				{
					vos_log(&(s_sock->t),LM_WARN,"%s mb is NULL \n",INFOMSG);
					//said queue has no msg
					i_judge_change = 0;
					break ;
				}
			 } //end send msg success
		 }  // end send message


	 }
	 vos_log(&(s_sock->t),LM_INFO,"%s after send msg,change:[%d],closed:[%d] \n",INFOMSG,i_judge_change,i_judge_close);
	// change to send event
	if(0 == i_judge_change)
	{
		vos_log(&(s_sock->t),LM_INFO,"the queue_num:[%d] when change sock to recv \n",s_sock->t.queue_m->msg_num);
		_change_socksta_(s_sock,ev ,2,fd);
	}

	//close socked
	if(0 == i_judge_close)	_close_sock_(s_sock,ev);

	vos_log(&(s_sock->t),LM_INFO,"<<<<<<<<<<<<<<<<<<<<<<<<<<<%s end  send message >>>>>>>>>>>>>>>>>>>>>>>\n",INFOMSG);
	return ;
}


/*
 * description:
 * 		accept connect
 * parameter:
 * 		@fd
 * 		@events
 * 		@arg
 * return:
 */

#ifndef SOCKCLIENT
static void acceptConn(int fd, int events, void *arg)
{
	if(0 > fd || 0 > events || NULL == arg) return ;

	sock_task *s_sock = (sock_task *)arg;
	sock_event *s_events = s_sock->myevent ;

	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	int nfd, i;

	do{
		printf("---------nfd = [%d],i = [%d] \n",nfd,i);
		// accept
		if((nfd = accept(fd, (struct sockaddr*)&sin, &len)) == -1)
		{
			//nonblocking, no data
			if(errno != EAGAIN && errno != EINTR)
			{
				sleep(1);
				continue ;
			}
			vos_log(&(s_sock->t),LM_ERROR,"%s %s: accept, %d",INFOMSG, __func__, errno);
			break;
		}

		for(i = 0; i < MAX_EVENTS; i++)
		{
			if(s_events[i].status == 0)
			{
				break;
			}
		}
		if(i == MAX_EVENTS)
		{
			vos_log(&(s_sock->t),LM_WARN,"%s %s:max connection limit[%d].",INFOMSG, __func__, MAX_EVENTS);
			continue ;
		}
		//		 set nonblocking
		if(0 > setNonblock(fd)) return ;
		//		 add a read event for receive data
		eventSet(&s_events[i], nfd, recvData, &s_events[i]);
		eventAdd(s_sock->epollfd, EPOLLIN, &s_events[i]);
		s_sock->count++ ;
		vos_debug("new conn[%s:%d][time:%ld], pos[%d]\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port), s_events[i].last_active,i);
		vos_log(&(s_sock->t),LM_INFO,"%s new conn[%s:%d][time:%ld], pos[%d]\n",INFOMSG,inet_ntoa(sin.sin_addr),ntohs(sin.sin_port), s_events[i].last_active,i);
	}while(nfd > 0);
}

#endif



/*
 * description:
 *		init the server'sock
 * parameter:
 * 		@epollfd,epoll descriptor
 * 		@config	,struct _conn_s_
 * return:
 * 		success,0 failed -1
 */
//static int bind_bz = 0;
static int initListenSocket(sock_task *s_sock)
{
	if(NULL == s_sock) return -1;
	sock_event *s_events = s_sock->myevent ;

    int listenFd = -1;
    if(-1 == (listenFd = socket(AF_INET, SOCK_STREAM, 0)))
    {
    	vos_log(&(s_sock->t),LM_ERROR,"%s socket failed \n",INFOMSG);
    	return -1;
    }

    // bind & listen
    struct sockaddr_in sin,sout;
    bzero(&sin,sizeof(sin));
    sin.sin_family = AF_INET;

    bzero(&sout,sizeof(sout));
    sout.sin_family = AF_INET ;

//#ifdef SOCKCLIENT
    sin.sin_addr.s_addr = inet_addr(s_sock->config.host);
    sin.sin_port = htons(s_sock->config.port);
//#else
    sout.sin_addr.s_addr = inet_addr(s_sock->config.servhost);
    sout.sin_port = htons(s_sock->config.servport);
//#endif

    bzero(&(sin.sin_zero),8);
    bzero(&(sout.sin_zero),8);


	if(-1 == bind(listenFd, (struct sockaddr*)&sout, sizeof(struct sockaddr)))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s bind failed \n",INFOMSG);
		return -1;
	}
	vos_log(&(s_sock->t),LM_INFO,"%s bind success \n",INFOMSG);


    // sock client connect
#ifdef SOCKCLIENT
	if (0 > connect(listenFd, (struct sockaddr *) &sin, sizeof(sin)))
	{
			vos_log(&(s_sock->t),LM_ERROR,"%s connect error \n",INFOMSG);
			return -1;
	}
	vos_log(&(s_sock->t),LM_INFO,"%s connect success \n",INFOMSG);

	//pre_sock callback
	if(NULL != s_sock->aftersock)
	{
		int res = s_sock->aftersock(&(s_sock->t),listenFd,s_sock->arg);
		switch(res)
		{
			case SOCK_PCCC_NNNY:		//close sock
				vos_log(&(s_sock->t),LM_WARN,"%s aftersock return broken \n",INFOMSG);
				close(listenFd);
				return -1;
		    case SOCK_PCCC_NNNN:		//right
		    	vos_log(&(s_sock->t),LM_WARN,"%s aftersock  links success \n",INFOMSG);
		    	s_events[0].status = 1;
		    	break;
		    default:				//default
		    	vos_log(&(s_sock->t),LM_WARN,"%s aftersock return unknown,The default close links\n",INFOMSG);
		    	close(listenFd);
		    	break;
		}
	}
	else
	{
		vos_log(&(s_sock->t),LM_WARN,"%s aftersock is NULL,The default links\n",INFOMSG);
		s_events[0].status = 1;
	}

#else

    //todo:set limit

    if(-1 == listen(listenFd, LISTEN_NUM))
    {
    	vos_log(&(s_sock->t),LM_ERROR,"%s listen failed \n",INFOMSG);
    	return -1;
    }
    vos_log(&(s_sock->t),LM_INFO,"%s listen success ,listen fd = %d\n",INFOMSG,listenFd);

#endif

	// set non-blocking
	if(0 > setNonblock(listenFd))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s set non-blocking failed \n",INFOMSG);
		return -1;
	}
	vos_debug("%s set non-blocking success,fd = %d\n",INFOMSG,listenFd);


	//add listen sock, s_events[MAX_EVENTS] is used by listen fd
#ifdef SOCKCLIENT
	if(0 != eventSet(&s_events[0], listenFd, sendData, &s_events[0]))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s set event failed,callback is sendData,listenFd = %d \n",INFOMSG,listenFd);
		//todo: close sock
		return -1;
	}
	if(0 !=  eventAdd(s_sock->epollfd, EPOLLOUT, &s_events[0]))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s add event to epollout failed \n",INFOMSG);
		//todo:close sock
		return -1;
	}

#else
	if(0 != eventSet(&s_events[MAX_EVENTS], listenFd, acceptConn,s_sock))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s set event failed,callback is acceptConn,listenFd = %d \n",INFOMSG,listenFd);
		//todo:close sock
		return -1;
	}
		 // add listen socket
	if(0 != eventAdd(s_sock->epollfd, EPOLLIN, &s_events[MAX_EVENTS]))
	{
		vos_log(&(s_sock->t),LM_ERROR,"%s add event to epollin failed \n",INFOMSG);
		//todo:close sock
		return -1;
	}
#endif

	s_sock->count++ ;
	vos_log(&(s_sock->t),LM_INFO,"%s initListenSocket success \n",INFOMSG);
    return 0;
}

/*
 * description:
 *        timeout check here
 * parameter:
 * 			@s_sock
 * 			@timeout
 * retrun:
 * 		0 success,-1 failed
 */

static int check_sock_conn(sock_task *s_sock,int timeout)
{
	if(NULL == s_sock || 0 > timeout) return -1;
	long now = getTime();

	sock_event *p_events = s_sock->myevent ;
	//callback is null,return success
	if(NULL == s_sock->checksock) return 0;
	vos_debug("-------count = %d \n",s_sock->count);

	// doesn't check listen fd if sock servers
	for(int i = 0; i < s_sock->count; i++)
	{
		if(0 == p_events[i].fd) continue ;
		vos_debug("---i = %d,fd[%d]\n",i,p_events[i].fd);

		//if not run status
		if(p_events[i].status != 1) continue;

		//not timeout
		long duration = now - p_events[i].last_active;
		if(0 >= duration || duration < timeout) continue ;

		vos_log(&(s_sock->t),LM_INFO,"%s fd:[%d],i:[%d],timeout \n",INFOMSG,p_events[i].fd,i);
		 //timeout
		int res = s_sock->checksock(&(s_sock->t),p_events[i].fd,s_sock->arg);
		vos_log(&(s_sock->t),LM_INFO,"%s checksock return %d,fd:[%d] \n",INFOMSG,res,p_events[i].fd);
		switch(res)
		{
			case SOCK_PCCC_NNNY:				//close the sock
				_close_sock_(s_sock,&p_events[i]);
				break;
			case SOCK_PCCC_NNNN:				//success
				p_events[i].status = SOCK_RUN;
				p_events[i].last_active = now;
				break;
			case SOCK_PCCC_NNYY:				//reconnect
				if(0 == _close_sock_(s_sock,&p_events[i]))
				{
					initListenSocket(s_sock);
				}
				break;
			default:						//default
				_close_sock_(s_sock,&p_events[i]);
				break;
		}

	}

	return 0;
}


/****************************************************************************************************************/
//												sock task
/****************************************************************************************************************/
/*
 *  sock task init
 */
static vos_i32 sock_task_init(vos_task *ptask, void *arg)
{
    //read sock'config files
    sock_task *socktask = (sock_task *)ptask;
    
    if(0 != get_sock_conf(&(g_env->system),&(socktask->config)))
    {
    	vos_log(ptask,LM_ERROR,"%s get sock config failed \n",INFOMSG);
    	return VOS_ERROR;
    }
    vos_log(ptask,LM_INFO,"%s get sock config success \n",INFOMSG);
    return VOS_OK;
}

/*
 * entry for socket task
 *
 */
static inline void *sock_task_svc(void *arg)
{
    if (NULL == arg) pthread_exit(NULL);
    vos_task *ptask = (vos_task *)arg;

    //read sock'config files
    sock_task *socktask = (sock_task *)ptask ;
    
    // g_Events[MAX_EVENTS] is used by listen fd
	sock_event s_events[MAX_EVENTS+1];
	// event loop
	struct epoll_event events[MAX_EVENTS];

	//init the sock_task
	socktask->myevent = (sock_event *)&s_events;
	socktask->events  = (struct epoll_event *)&events;


	 // create epoll,loop MAX_LOOP
	int g_epollFd = -1;
	for(int i = 0; i < MAX_LOOP; i++)
	{
		g_epollFd =	epoll_create(MAX_EVENTS);
		//success
		if(0 < g_epollFd) break;
		if(i == MAX_LOOP -1) exit(0);
		vos_log(ptask,LM_ERROR,"%s epoll_create(%d) failed,i = %d",INFOMSG,MAX_EVENTS,i);
		sleep(MAX_SLEEP);
	}
	vos_log(ptask,LM_INFO,"%s epoll_create(%d) success \n",INFOMSG,MAX_EVENTS);

	//save the epoll description
	socktask->epollfd = g_epollFd;

	// create & bind listen socket, and add to epoll, set non-blocking
	if(NULL != socktask->presock)
	{
		if(0 == socktask->presock(&(socktask->t),socktask->arg))
		{				//link
			vos_log(ptask,LM_INFO,"%s before svc loop,callback function presock return to  link \n",INFOMSG);
			 if(0 != initListenSocket(socktask))
			 {
				 vos_log(ptask,LM_INFO,"%s initlistensock failed \n",INFOMSG);
			 }
		}
		else
		{
			vos_log(ptask,LM_INFO,"%s before svc loop,callback function presock return to not link \n",INFOMSG);
		}
	}
	else
	{		//default link
		vos_log(ptask,LM_INFO,"%s callback function presock is NULL \n",INFOMSG);
		if(0 != initListenSocket(socktask))
		{
			vos_log(ptask,LM_INFO,"%s initlistensock failed \n",INFOMSG);
		}
	}
    
	vos_log (ptask,LM_INFO,"==>%s task %s thread %d enter svc\n",INFOMSG,ptask->task_name, (int)pthread_self());
    vos_msgblk *mb = NULL;
    while (1)
    {
        if (TASK_STATE_RUN != ptask->cond)
        {
            vos_debug("task %s not in run.\n", ptask->task_name);
            sleep(5);
            continue;
        }


    	//If there is no link, determine whether the link,
		if(0 >= socktask->count)
		{
			vos_log(ptask,LM_INFO," has no client fd,count:[%d] \n",socktask->count);
			if(NULL != socktask->presock)
			{
				if(0 == socktask->presock(&(socktask->t),socktask->arg))
				{
					if(-1 == initListenSocket(socktask))
					{
						sleep(MAX_SLEEP);
						continue;
					}
				}
				else
				{
					sleep(MAX_SLEEP);
					continue ;
				}
			}
			else
			{
				if(-1 == initListenSocket(socktask))
				{
					sleep(MAX_SLEEP);
					continue;
				}
			}
		}//end if(0 >= socktask->count)


    	// timeout check
//#ifdef SOCKCLIENT
		 check_sock_conn(socktask,socktask->config.timeout);
//#else
//		 check_sock_conn(socktask,socktask->config.servtimeout);
//#endif
		/*
		 * wait for events to happen
		 * parameter 4,-1,0,timeout
		 */
		int fds = epoll_wait(g_epollFd, events, MAX_EVENTS, 0);

		if(fds < 0)
		{		//wait error
			vos_log(ptask,LM_WARN,"%s epoll_wait error, exit\n",INFOMSG);

		}
		else if(0 == fds)
		{		//wait timeout
//			vos_debug("%s epoll_wait timeout \n",INFOMSG);
		}
		else
		{
#ifndef SOCKCLIENT
		vos_log(ptask,LM_INFO,"%s fds = %d \n",INFOMSG,fds);
#endif
			//handle happen events
			for(int i = 0; i < fds; i++)
			{   // in running
				sock_event *ev = (struct _socket_event*)events[i].data.ptr;
				vos_log(ptask,LM_INFO,"%s status = %d \n",INFOMSG, ev->status);
				if(1 == ev->status)
				{
					// read event
					if((ev->events&EPOLLIN))
					{
						vos_log(ptask,LM_INFO,"==================begin to read==============\n");
						ev->call_back(ev->fd, i, socktask);
					}

					// write event
					if((ev->events&EPOLLOUT))
					{
						vos_log(ptask,LM_INFO,"==================begin to write==============\n");
						//send message if mb is not NULL
						if(NULL != mb)
						{
							ev->buff = mb;
							ev->call_back(ev->fd, i, socktask);
							//free
//							msg_free(mb);
//							mb = NULL;
						}

					}
				}

			} //end for(int i = 0; i < fds; i++)

		} //end if(fds < 0)

		//sleep
		usleep(100);
    } //end while(1)

	// free resource
	close(g_epollFd);
	close(s_events[MAX_EVENTS].fd);
	vos_log (ptask,LM_INFO,"==>%s task %s thread %d exit svc\n",INFOMSG, ptask->task_name, (int)pthread_self());
    pthread_exit(NULL);
}

int instance_sock_task(vos_task **ppt, const vos_str name, vos_u32 high_water, vos_i32 count, void *arg)
{
    if (0 != task_instance(ppt, name, high_water, sizeof(sock_task), count, arg)) return -1;
    (*ppt)->init = sock_task_init;
	(*ppt)->loop = sock_task_svc;
	return 0;
}

#endif /* SOCK_TASK_H_ */
