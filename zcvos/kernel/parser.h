/*
 ============================================================================
 Name        : parse.h
 Author      : zichen
 Version     : 2.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : 读写配置文件，一个key若有多个配置值，用','分隔
 ============================================================================
 */
 
#ifndef PARSE_H_
#define PARSE_H_

#include <stdlib.h>
#include <stdio.h>  
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "syntax.h"
#include "stdarg.h"
#include "vos_list.h"

#define 	BUFS 			1024 	/* 2M */   /*ghr modify on 20130308 update the 256 to 1024*/
#define 	TOKEN_LEN		sizeof(token_t)
#define 	MAX_VALUE_LEN		BUFS
typedef void 	*yyscan_t;

// 标记一个token_t的属性(key,value,...) 
typedef enum 
{
	ENUM_TOK_KEY, 		// 键
	ENUM_TOK_VALUE, 	// 值
	ENUM_TOK_REGION, 	// 域
	ENUM_TOK_NOTATION, 	// 注释
	ENUM_TOK_NULLINE, 	// 空行
	ENUM_BUTT
} TYPE_ENUM;

// 标记一个语句的属性(kvalue,region,notation) 
typedef enum 
{
	ENUM_KV_KVALUE, 	// 键
	ENUM_KV_REGION, 	// 域
	ENUM_KV_NOTATION, 	// 注释
	ENUM_KV_NULLINE,	// 空行
	ENUM_KV_BUTT
} KV_TYPE_ENUM;

struct _tok_pool_m;
struct _kvalue_pool_m;
typedef struct _tok_pool_m tok_pool_m;
typedef struct _kvalue_pool_m kvalue_pool_m;

// 全局变量
typedef struct _system_t
{
	char		name[255];
	FILE		*fp;
	yyscan_t 	scanner;		//typedef void* yyscan_t
	char 		*extra_info;
	void		*pParser;		//解析器
	tok_pool_m 	*token_list;		//符号池
	kvalue_pool_m 	*kvalue_list;		//语句池
} system_t;

// 符号结构，符号是词法分析术语，匹配一个正则表达式的字串就可看成一个符号，如一个key、value、...
typedef struct _token_t
{
	TYPE_ENUM		type;
	int			lineno;
	char			*Identifier;
	system_t		*system;
	struct list_head 	list;
} token_t;

// 符号表管理
struct _tok_pool_m
{
	int			count;
	struct list_head 	head;
	pthread_mutex_t 	lock;
} ;

// 语句结构，一个'key = value'为一个语句 
typedef struct _kvalue_t 
{
	int			type;
	char			*key;
	char			*value;
	char			*orther;
	struct list_head 	list;
} kvalue_t;

// 语句表
struct _kvalue_pool_m
{
	int			count;
	struct list_head 	head; 
	pthread_mutex_t 	lock;
} ;

// 字串值
typedef struct _vstring_t
{
	char			*value;
	struct list_head 	list; 
} vstring_t;

// 数字值
typedef struct _vnumber_t
{
	int			value;
	struct list_head 	list; 
} vnumber_t;

// 值链表 
typedef struct _value_list
{
	int			count;
	struct list_head 	head; 
} value_list;



/*
 * 打开配置文件解析器，fname为配置文件名 
 */
int open_parser(system_t *system);

/*
 * 关闭配置文件解析器,fname为修改后的配置文件名，一般与原文件同名 
 */
void close_parser(system_t *system);

/* 
 * 输入key，和位置，获得相应value值
 * get_value_str(),获取字符串值
 * get_value_numeric(),获取数值 
 * 如果等号后面有多个用逗号隔开的值，用count参数指定要获取的值的位置，count从0开始
 * 如果key为空，返回-1，否则返回0
 */
int get_value_string(system_t *system, char *key, int count, char **value);
int get_value_numeric(system_t *system, char *key, int count, long int **value);


#endif /* PARSE_H_ */
