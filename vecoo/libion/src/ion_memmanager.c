/*
 *  ion.c
 *
 * Memory Allocator functions for ion
 *
 *   Copyright 2011 Google, Inc
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#define LOG_TAG "ion_memmanager"

#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>
#include "cdx_list.h"
#include "plat_log.h"

#include "./include/ion/ion.h"

#define ION_ALLOC_ALIGN  SZ_4k


typedef struct tag_ion_mem_node
{
    AW_ION_MEM_INFO_S mem_info;
    struct list_head mList;
}ION_MEM_NODE_S;

typedef struct tag_ion_mem_manager
{
    int dev_fd;
    struct list_head mMemList;  //ION_MEM_NODE_S
    pthread_mutex_t mLock;
    //pthread_mutex_t mRefLock;
    int ref_cnt;
}ION_MEM_MANAGER_S;

static ION_MEM_MANAGER_S *g_mem_manager = NULL;
static pthread_mutex_t g_mutex_alloc = PTHREAD_MUTEX_INITIALIZER;


int ion_memOpen(void)
{
    int ret = 0;

    pthread_mutex_lock(&g_mutex_alloc);

    if (g_mem_manager != NULL)
    {
        alogd("ion allocator has already been created");
        goto SUCCEED_OUT;
    }

    g_mem_manager = (ION_MEM_MANAGER_S *)malloc(sizeof(ION_MEM_MANAGER_S));
    if (NULL == g_mem_manager)
    {
        aloge("no mem! open fail");
        pthread_mutex_unlock(&g_mutex_alloc);
        return -1;
    }
    memset(g_mem_manager, 0, sizeof(ION_MEM_MANAGER_S));
    INIT_LIST_HEAD(&g_mem_manager->mMemList);
    pthread_mutex_init(&g_mem_manager->mLock, NULL);

    g_mem_manager->dev_fd = ion_open();
    if (g_mem_manager->dev_fd <= 0)
    {
        aloge("ion open fail");
        pthread_mutex_destroy(&g_mem_manager->mLock);
        free(g_mem_manager);
        g_mem_manager = NULL;

        pthread_mutex_unlock(&g_mutex_alloc);
        return -1;
    }

SUCCEED_OUT:
    //pthread_mutex_lock(&g_mem_manager->mRefLock);
    g_mem_manager->ref_cnt++;
    //pthread_mutex_unlock(&g_mem_manager->mRefLock);

    pthread_mutex_unlock(&g_mutex_alloc);
    return ret;
}

int ion_memClose(void)
{
    int ret = 0;

    pthread_mutex_lock(&g_mutex_alloc);

    if (g_mem_manager == NULL)
    {
        alogw("has not open, please open first!");
        pthread_mutex_unlock(&g_mutex_alloc);
        return ret;
    }

    if (--g_mem_manager->ref_cnt <= 0)
    {
        if (g_mem_manager->ref_cnt < 0)
        {
            alogw("maybe close more times than open");
        }

        if (g_mem_manager->dev_fd > 0)
        {
            if (ion_close(g_mem_manager->dev_fd) != 0)
            {
                aloge("ion close fail");
                //ret = -1;
            }
            g_mem_manager->dev_fd = 0;
        }

        ION_MEM_NODE_S *pEntry, *pTmp;

        pthread_mutex_lock(&g_mem_manager->mLock);
        if (!list_empty(&g_mem_manager->mMemList))
        {
            aloge("fatal error! why some ion mem still in list??? force to free!");
            list_for_each_entry_safe(pEntry, pTmp, &g_mem_manager->mMemList, mList)
            {
                list_del(&pEntry->mList);

                if (munmap((void *)(pEntry->mem_info.vir), pEntry->mem_info.size) < 0)
                {
                    aloge("munmap 0x%p, size: %d failed", (void*)pEntry->mem_info.vir, pEntry->mem_info.size);
                }

                /*close dma buffer fd*/
                close(pEntry->mem_info.fd_data.fd);
                ion_free(g_mem_manager->dev_fd, pEntry->mem_info.fd_data.handle);

                free(pEntry);
            }
        }
        pthread_mutex_unlock(&g_mem_manager->mLock);

        pthread_mutex_destroy(&g_mem_manager->mLock);

        free(g_mem_manager);
        g_mem_manager = NULL;
    }
    else
    {
        alogd("still left %d in use ", g_mem_manager->ref_cnt);
    }

    pthread_mutex_unlock(&g_mutex_alloc);

    return ret;
}

unsigned char* ion_allocMem(unsigned int size)
{
    int ret;
    ion_user_handle_t handle;
    int map_fd;
    unsigned char *vir_ptr;
    unsigned int phy_addr;

    if (0 == size)
    {
        aloge("size error!");
        return NULL;
    }

    ION_MEM_NODE_S *pMemNode;
    pMemNode = (ION_MEM_NODE_S *)malloc(sizeof(ION_MEM_NODE_S));
    if (NULL == pMemNode)
    {
        aloge("no mem! alloc fail");
        return NULL;
    }
    memset(pMemNode, 0, sizeof(ION_MEM_NODE_S));

    ret = ion_alloc( g_mem_manager->dev_fd, size, ION_ALLOC_ALIGN,
                     AW_ION_DMA_HEAP_MASK | AW_ION_CARVEOUT_HEAP_MASK,
                     AW_ION_CACHED_FLAG | AW_ION_CACHED_NEEDS_SYNC_FLAG,
                     &handle);
    if (ret != 0)
    {
        free(pMemNode);
        aloge("ion alloc fail!");
        return NULL;
    }

    ret = ion_map( g_mem_manager->dev_fd, handle, size,
                   PROT_READ|PROT_WRITE,
                   MAP_SHARED,
                   0,
                   &vir_ptr,
                   &map_fd );

    if ((ret !=0) || (vir_ptr==NULL))
    {
        aloge("ion map fail");
        ion_free(g_mem_manager->dev_fd, handle);
        free(pMemNode);
        return NULL;
    }

    if (0 == ion_get_phyaddr( g_mem_manager->dev_fd,
                              handle, size, &phy_addr ))
    {
        aloge("get phyaddr fail!");
        if (munmap((void *)vir_ptr, size) < 0)
        {
            aloge("munmap 0x%p, size: %d failed", (void*)vir_ptr, size);
        }

        /*close dma buffer fd*/
        close(map_fd);
        ion_free(g_mem_manager->dev_fd, handle);
        return NULL;
    }

    pMemNode->mem_info.vir = (unsigned int)vir_ptr;
    pMemNode->mem_info.phy = phy_addr;
    pMemNode->mem_info.size = size;
    pMemNode->mem_info.fd_data.fd = map_fd;
    pMemNode->mem_info.fd_data.handle = handle;

    pthread_mutex_lock(&g_mem_manager->mLock);
    list_add_tail(&pMemNode->mList, &g_mem_manager->mMemList);
    pthread_mutex_unlock(&g_mem_manager->mLock);

    return vir_ptr;
}

static int searchExistMemList(void *vir_ptr, ION_MEM_NODE_S **pNode)
{
    int ret = -1;
    ION_MEM_NODE_S *pEntry, *pTmp;

    pthread_mutex_lock(&g_mem_manager->mLock);
    if (list_empty(&g_mem_manager->mMemList))
    {
        pthread_mutex_unlock(&g_mem_manager->mLock);
        *pNode = NULL;
        return ret;
    }

    list_for_each_entry_safe(pEntry, pTmp, &g_mem_manager->mMemList, mList)
    {
        if (pEntry->mem_info.vir == (unsigned int)vir_ptr)
        {
            ret = 0;
            *pNode = pEntry;
            //list_del(&pEntry->mList);
            break;
        }
    }
    pthread_mutex_unlock(&g_mem_manager->mLock);

    return ret;
}

int ion_freeMem(void *vir_ptr)
{
    int ret = -1;
    ION_MEM_NODE_S *pEntry;

    ret = searchExistMemList(vir_ptr, &pEntry);
    if (!ret)
    {
        pthread_mutex_lock(&g_mem_manager->mLock);
        list_del(&pEntry->mList);
        pthread_mutex_unlock(&g_mem_manager->mLock);

        if (munmap((void *)(pEntry->mem_info.vir), pEntry->mem_info.size) < 0)
        {
            aloge("munmap 0x%p, size: %d failed", (void*)pEntry->mem_info.vir, pEntry->mem_info.size);
        }

        /*close dma buffer fd*/
        close(pEntry->mem_info.fd_data.fd);
        ion_free(g_mem_manager->dev_fd, pEntry->mem_info.fd_data.handle);
        free(pEntry);
    }
    else
    {
        aloge("vir ptr not find in memlist, free fail!");
    }

    return ret;
}

unsigned int ion_getMemPhyAddr(void *vir_ptr)
{
    int ret = 0;
    ION_MEM_NODE_S *pEntry;

    if (vir_ptr == NULL)
    {
        aloge("null ptr!");
        return 0;
    }

    ret = searchExistMemList(vir_ptr, &pEntry);
    if (!ret)
    {
        return pEntry->mem_info.phy;
    }
    else
    {
        aloge("vir_ptr not find int list!get phyaddr fail!");
    }

    return ret;
}

int ion_flushCache(void *vir_ptr, unsigned int size)
{
    int ret = -1;
    ION_MEM_NODE_S *pEntry;

    if (vir_ptr == NULL || !size)
    {
        aloge("null ptr or size=0");
        return -1;
    }

    ret = searchExistMemList(vir_ptr, &pEntry);
    if (!ret)
    {
        ret = ion_flush_cache(g_mem_manager->dev_fd, (void *)pEntry->mem_info.vir, pEntry->mem_info.size);
        if (ret != 0)
        {
            aloge("flush cache fail");
        }
    }
    else
    {
        aloge("vir_ptr not find int list! flush cache fail!");
    }

    return ret;
}

