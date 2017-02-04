/*
 * vos_strfunc.c
 *
 *  Created on: 2013-12-10
 *      Author: kevin
 * 
 */

#include "vos_strfunc.h"


vos_str str_split (vos_str * string,  const vos_str token)
{
    vos_ucptr str, rst;
    vos_str ctrl = token;

    char map[32];
    int count;

    if (NULL == string) return NULL;
    if (NULL == (*string)) return NULL;

    /* Clear control map */
    for (count = 0; count < 32; count++)
        map[count] = 0;

    rst = str = (vos_ucptr)(*string);
    /* Set bits in delimiter table */
    do
    {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);

    /* Find the end of the token. If it is not the end of the string,
    * put a null there. */
    for ( ; *str; str++ )
    {
        //ACE_DEBUG((SMAP_DEBUG ACE_TEXT("go on:%c, 0x%x \n"), *str, (int)*str)); //zc_debug("go on:%c\n", *str);
        if ( map[*str >> 3] & (1 << (*str & 7)))
        {
            *str++ = '\0';
            break;
        }
    }

    /* Determine if a token has been found. */
    if ( (vos_ucptr)*string == str )
        return NULL;
    else
    {
        (*string) = (vos_str)str;
        return (vos_str)rst;
    }
}


void str_trim(vos_str str)
{
    if (NULL == str) return;
    vos_int len ,len2 ;
    len = len2 = strlen(str);
    char* p = str;
    while (len > 0)
    {
        if (' ' == str[len] ||
        		'\r' == str[len] ||
        		'\n' == str[len] ||
        		0x00 == str[len])
        {
        	str[len] = 0;
        	len--;
        }
        else{
        	break;
        }
    }
    for (vos_int i = 0; i < len2; i++)
    {
        if (' ' == str[i]
           || '\r' == str[i]
           || '\n' == str[i])
            p++;
        else{
        	break;
        }
    }
    strncpy(str, p, len);
}


/*
 * zyh add
 */
/*
 * As the alignment, and zero
 */
vos_int right_zero(vos_str str1,vos_int len,vos_str str2)
{
	if(NULL == str1 || NULL == str2) return -1;
	return snprintf(str1,len,"%-s%0*d",str2,len-strlen(str2),0);
}

vos_int left_zero(vos_str str1,vos_int len,vos_str str2)
{
	if(NULL == str1 || NULL == str2) return -1;
	int size = len-strlen(str2);
	return (1 >= size) ? snprintf(str1,len,"%s",str2) : snprintf(str1,len,"%0*d%s",size -1,0, str2);
}
/*
 * get local time and date
 * flag :0-get HHMMSS (client)
 *      1-get MMDD   (client)
 *      2-get YYYYMMDD   (corexml)
 *      3-get YYYYMMDDHHMMSS  (corexml)
 * ...
 */
void getlocal_time_date(int flag,char *arg,int len)
{
    struct tm* m;
    time_t g_t;
    g_t = time(NULL);
    m = localtime(&g_t);
    switch(flag)
    {
    case 0:
        snprintf(arg,len,"%02d%02d%02d",m->tm_hour, m->tm_min, m->tm_sec);
        break;
    case 1:
        snprintf(arg,len,"%02d%02d",m->tm_mon + 1, m->tm_mday);
        break;
    case 2:
        snprintf(arg,len,"%04d%02d%02d",m->tm_year + 1900,m->tm_mon + 1, m->tm_mday);
        break;
    case 3:
        snprintf(arg,len,"%04d%02d%02d%02d%02d%02d",m->tm_year + 1900,m->tm_mon + 1, m->tm_mday,m->tm_hour, m->tm_min, m->tm_sec);
        break;
    default:
        break;
    }

    return;
}
