/*
 ============================================================================
 Name        : parse.h
 Author      : zichen
 Version     : 2.0
 Copyright   : Copyright (C) ShaanXi ZiChen Tech. Ltd. Co. 2010-2011.
                        ALL RIGHTS RESERVED!
 Description : ��д�����ļ���һ��key���ж������ֵ����','�ָ�
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

// ���һ��token_t������(key,value,...) 
typedef enum 
{
	ENUM_TOK_KEY, 		// ��
	ENUM_TOK_VALUE, 	// ֵ
	ENUM_TOK_REGION, 	// ��
	ENUM_TOK_NOTATION, 	// ע��
	ENUM_TOK_NULLINE, 	// ����
	ENUM_BUTT
} TYPE_ENUM;

// ���һ����������(kvalue,region,notation) 
typedef enum 
{
	ENUM_KV_KVALUE, 	// ��
	ENUM_KV_REGION, 	// ��
	ENUM_KV_NOTATION, 	// ע��
	ENUM_KV_NULLINE,	// ����
	ENUM_KV_BUTT
} KV_TYPE_ENUM;

struct _tok_pool_m;
struct _kvalue_pool_m;
typedef struct _tok_pool_m tok_pool_m;
typedef struct _kvalue_pool_m kvalue_pool_m;

// ȫ�ֱ���
typedef struct _system_t
{
	char		name[255];
	FILE		*fp;
	yyscan_t 	scanner;		//typedef void* yyscan_t
	char 		*extra_info;
	void		*pParser;		//������
	tok_pool_m 	*token_list;		//���ų�
	kvalue_pool_m 	*kvalue_list;		//����
} system_t;

// ���Žṹ�������Ǵʷ��������ƥ��һ��������ʽ���ִ��Ϳɿ���һ�����ţ���һ��key��value��...
typedef struct _token_t
{
	TYPE_ENUM		type;
	int			lineno;
	char			*Identifier;
	system_t		*system;
	struct list_head 	list;
} token_t;

// ���ű����
struct _tok_pool_m
{
	int			count;
	struct list_head 	head;
	pthread_mutex_t 	lock;
} ;

// ���ṹ��һ��'key = value'Ϊһ����� 
typedef struct _kvalue_t 
{
	int			type;
	char			*key;
	char			*value;
	char			*orther;
	struct list_head 	list;
} kvalue_t;

// ����
struct _kvalue_pool_m
{
	int			count;
	struct list_head 	head; 
	pthread_mutex_t 	lock;
} ;

// �ִ�ֵ
typedef struct _vstring_t
{
	char			*value;
	struct list_head 	list; 
} vstring_t;

// ����ֵ
typedef struct _vnumber_t
{
	int			value;
	struct list_head 	list; 
} vnumber_t;

// ֵ���� 
typedef struct _value_list
{
	int			count;
	struct list_head 	head; 
} value_list;



/*
 * �������ļ���������fnameΪ�����ļ��� 
 */
int open_parser(system_t *system);

/*
 * �ر������ļ�������,fnameΪ�޸ĺ�������ļ�����һ����ԭ�ļ�ͬ�� 
 */
void close_parser(system_t *system);

/* 
 * ����key����λ�ã������Ӧvalueֵ
 * get_value_str(),��ȡ�ַ���ֵ
 * get_value_numeric(),��ȡ��ֵ 
 * ����Ⱥź����ж���ö��Ÿ�����ֵ����count����ָ��Ҫ��ȡ��ֵ��λ�ã�count��0��ʼ
 * ���keyΪ�գ�����-1�����򷵻�0
 */
int get_value_string(system_t *system, char *key, int count, char **value);
int get_value_numeric(system_t *system, char *key, int count, long int **value);


#endif /* PARSE_H_ */
