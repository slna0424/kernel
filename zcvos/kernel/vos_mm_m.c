/*
 * vos_mm_m.c
 *
 *  Created on: 2014-1-28
 *      Author: kevin
 *
 */

//#define __USE_XOPEN2K
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#include "vos_types.h"
#include "vos_bitmap.h"
#include "vos_metablk.h"
#include "vos_mm_m.h"

/*
 *
 * memory map file structure
 *
 * ----------------------------------------------
 *              | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 *   bitmap     | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 *   (count)    |           .....               |
 *              | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 * ----------------------------------------------
 *              |        metablk             |
 *              |         (data)                |
 *              ---------------------------------
 *              |        metablk             |
 *     buff     |         (data)                |
 *   (count)    ---------------------------------
 *              |           ......              |
 *              ---------------------------------
 *              |        metablk             |
 *              |         (data)                |
 * ----------------------------------------------
 *
 */

vos_metablk *mm_index(vos_mm_m *dm, vos_u32 index)
{
    if(dm == NULL) return NULL;
    if(index > dm->mtm.count || index < 0) return NULL;
    vos_metablk *pnode = (vos_metablk *)(dm->buff + dm->mtm.size * index);
    return pnode;
}

vos_i32 mm_indexof(vos_mm_m *dm, vos_metablk *pnode)
{
    if(dm == NULL || NULL == pnode) return INVALID_INDEX;
    vos_i32 index = ((vos_str)pnode - dm->buff)/ dm->mtm.size;
    return (index > dm->mtm.count - 1) ? (INVALID_INDEX) : (index);
}

void *mm_malloc(void *p, size_t size, void *arg)
{
    if (NULL == p) return NULL;
    vos_mm_m *mm = (vos_mm_m *)p;

    vos_i32 index = vos_bitmap_take(mm->bm);
    return (INVALID_INDEX == index) ? NULL : mm_index(mm, index);
}

void mm_free(void *p)
{
    if (NULL == p) return;
    vos_metablk *pnode = (vos_metablk *)p;
    vos_mm_m *mm = (vos_mm_m *)pnode->mtm;

    vos_i32 ind = ((vos_str)pnode - mm->buff)/(pnode->mtm->size);
    vos_debug ("free a mmblk buff = %p, blk=%p, ind = %d\n", 
                mm->buff, pnode, ind);
    vos_bitmap_clear(mm->bm, ind);
}

vos_i32 mm_init(void *ptr)
{
    if (NULL == ptr) return 0;
    vos_mm_m *mm = (vos_mm_m *)ptr;

    if (NULL == mm->f_name) return -1;

    struct stat fstat;
    vos_u32 stdsize = mm->mtm.count * mm->mtm.size + BITS_TO_BYTE(mm->mtm.count);
    vos_u8 *p = NULL;
    int fd = -1;

    vos_debug ("create mmblk with %s %d %u\n", mm->f_name, mm->mtm.count, mm->mtm.size);
    while (1)
    {
        // create mmap file if file NOT existed
        if (0 != stat(mm->f_name, &fstat) )
        {
            if (ENOENT == errno)
            {
                fd = open(mm->f_name, O_RDWR|O_CREAT, 0666);
                if (fd < 0)
                {
                    vos_debug ("create file failed with %s\n", mm->f_name);
                    perror("create file");
                    return VOS_ERROR;
                }
                else
                {
                    close(fd);
                    if (0 != truncate((const char *)mm->f_name, (off_t)stdsize))
                    {
                        vos_debug ("truncate file %s failed to size %d \n", mm->f_name, stdsize);
                        perror("truncate file");
                        return VOS_ERROR;
                    }
                    vos_debug ("create file %s ok\n", mm->f_name);
                    break;
                }
            }
            else
            {
                vos_debug ("get file info failed with %s\n", mm->f_name);
                perror("stat file");
                return VOS_ERROR;
            }
        }
        else
        {
            if (S_ISREG(fstat.st_mode))
            {
                if (fstat.st_size < stdsize)
                {
                    vos_debug ("file %s is smaller than expected, delete it\n", mm->f_name);
                    unlink(mm->f_name);
                    continue;
                }
                else
                {
                    vos_debug ("file %s is good\n", mm->f_name);
                    break;
                }
            }
            else
            {
                vos_debug ("%s is not a proper file for map\n", mm->f_name);
                return VOS_ERROR;
            }
        }

    }

    if ((fd = open(mm->f_name, O_RDWR|O_CREAT)) < 0) {
        perror("open file for map");
        vos_debug ("open file %s failed\n", mm->f_name);
        return VOS_ERROR;
    }
    p = (vos_u8 *)mmap(NULL, stdsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == p) {
        perror("mmap");
        vos_debug ("map file %s failed\n", mm->f_name);
        return VOS_ERROR;
    }
    close(fd);
    
    vos_bitmap_m *bm = vos_bitmap_create(mm->mtm.count);
    if (NULL == bm)
    {
        vos_debug ("create bitmap manager failed\n");
        munmap(p, stdsize);
        return VOS_ERROR;
    }
    vos_bitmap_load(bm, p);


    mm->addr = p;
    mm->bm = bm;
    mm->buff = (vos_str)(p + bm->nbytes);
    vos_i32 i = 0;
    vos_metablk *pnode = NULL;
    for (; i < mm->mtm.count; i++)
    {
        pnode = (vos_metablk *)(mm->buff + i * mm->mtm.size);
        if (1 == vos_bitmap_fetch(mm->bm, i))
        {
            vos_debug ("add used list with %d, node = %p\n", i, pnode);
            mm->mtm.left--;
        }
        else
        {
            vos_debug ("add free list with %d, node = %p\n", i, pnode);
        }
    }

    return VOS_OK;
}

void mm_fini(void *p)
{
    vos_mm_m *mm = (vos_mm_m *)p;
    if (NULL == mm) return;
    if (NULL != mm->bm) vos_bitmap_unload(mm->bm);
    vos_debug("========the len is %d=======\n", mm->mtm.count * mm->mtm.size + BITS_TO_BYTE(mm->mtm.count));
    munmap(mm->addr, mm->mtm.count * mm->mtm.size + BITS_TO_BYTE(mm->mtm.count));
}

void mm_hm(void *p, vos_int age)
{
    if (NULL == p) return;
    vos_mm_m *mm = (vos_mm_m *)p;

    time_t now;
    time(&now);
    for (vos_int i = 0; i < mm->bm->nbits; i++)
    {
        if (1 == vos_bitmap_fetch(mm->bm, i))
        {
            vos_metablk *mt = mm_index(mm, i);
            if (age < now - mt->tick)
            {
                vos_mt_free(mt);
            }
        }
    }

}

vos_i32 mm_sync(void *dm, vos_i32 sync)
{
    vos_mm_m *mm = (vos_mm_m *)dm;
    if (NULL == mm) return 0;
    int flag = (0 == sync) ? (MS_ASYNC|MS_INVALIDATE) : (MS_SYNC|MS_INVALIDATE);
    return msync(mm->addr, mm->mtm.count * mm->mtm.size + BITS_TO_BYTE(mm->mtm.count), flag);
}

void mm_reset(void *dm)
{
    vos_mm_m *mm = (vos_mm_m *)dm;
    if (NULL == mm) return;

    vos_bitmap_zero(mm->bm);
    mm->mtm.left = mm->mtm.count;
}
