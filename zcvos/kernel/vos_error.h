/*
 * vos_error.h
 *
 * error numbers and messages
 *
 *  Created on: 2014-1-25
 *      Author: kevin
 *
 */
#ifndef VOS_ERROR_H_
#define VOS_ERROR_H_

    
#define VOS_OK          0
#define VOS_ERROR      -1

#define VOS_MSGQUEUE_OVERFLOW   0x81

#define VOS_DB_OPEN_ERROR       0xE1
#define VOS_DB_SQL_ERROR        0xE2

#define VOS_MEM_ERROR           0xF1
#define VOS_FILE_IO             0xF2

#define VOS_COMM_TIMEOUT        0x91

#endif /* VOS_ERROR_H_ */
