/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.1.0	leo@2012/03/17: first commit.
** v1.2.9   leo@2012/09/10: add bigmem support.
** v1.2.12  leo@2012/11/30: modify alcdwl_init_bigmem() interface.
**
*****************************************************************************/ 

#ifndef __ALC_DWL_H__
#define __ALC_DWL_H__


typedef void * alcdwl_t;

alcdwl_t alcdwl_init(IN IM_TCHAR *owner);
IM_RET alcdwl_deinit(IN alcdwl_t handle);
IM_RET alcdwl_init_bigmem(IN alcdwl_t handle, IN IM_INT32 blockSize, IN IM_INT32 blockNum, IN IM_BOOL isCache);
IM_RET alcdwl_deinit_bigmem(IN alcdwl_t handle);
IM_RET alcdwl_mm_alloc(IN alcdwl_t handle, IN IM_INT32 size, IN IM_INT32 flag, OUT alc_buffer_t *alcBuffer);
IM_RET alcdwl_mm_free(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer);
IM_RET alcdwl_mm_map_useraddr(IN alcdwl_t handle, IN void *uVirAddr, IN IM_INT32 size, OUT alc_buffer_t *alcBuffer);
IM_RET alcdwl_mm_unmap_useraddr(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer);
IM_RET alcdwl_flush_buffer(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer, IN IM_INT32 flag);

IM_RET alcdwl_statc_lock_access(IN alcdwl_t handle);
IM_RET alcdwl_statc_unlock_access(IN alcdwl_t handle);
IM_RET alcdwl_statc_get_owner_num(IN alcdwl_t handle, OUT IM_INT32 *num);
IM_RET alcdwl_statc_get_owner(IN alcdwl_t handle, IN IM_INT32 index, OUT IM_TCHAR *owner);
IM_RET alcdwl_statc_get_owner_buffer_num(IN alcdwl_t handle, IN IM_TCHAR *owner, OUT IM_INT32 *num);
IM_RET alcdwl_statc_get_owner_buffer(IN alcdwl_t handle, IN IM_TCHAR *owner, IN IM_INT32 index, OUT alc_buffer_t *alcBuffer);

void * alcdwl_malloc(IM_INT32 size);
void alcdwl_free(void *p);
void * alcdwl_memset(void *p, IM_CHAR c, IM_INT32 size);
void * alcdwl_memcpy(void *dst, void *src, IM_INT32 size);

#endif	// __ALC_DWL_H__

