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
** v1.2.1	leo@2012/03/28: fix the bug of munmap(), the first argument should be virtual address.
** v1.2.4	leo@2012/04/28: add VIRTUAL_ALLOC_STATC to turn on/off add/del_useraddr_for_statc.
**          leo@2012/05/16: modify alcdwl_flush_buffer().
** v1.2.9   leo@2012/09/10: add bigmem support.
** v1.2.12  leo@2012/11/30: modify alcdwl_init_bigmem() interface.
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_buffallocapi.h>
#include <pmm_lib.h>
#include <alc_dwl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>


#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"ALCDWL_I:"
#define WARNHEAD	"ALCDWL_I:"
#define ERRHEAD		"ALCDWL_I:"
#define TIPHEAD		"ALCDWL_I:"

//
#define VIRTUAL_ALLOC_STATC

//
#define PMM_DEV_NAME	"/dev/pmm"

//
typedef struct{
	IM_INT32	pmm_fd;
}alcdwl_context_t;


alcdwl_t alcdwl_init(IM_TCHAR *owner)
{
	alcdwl_context_t *dwl;
	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), owner));

	//
	dwl = (alcdwl_context_t *)alcdwl_malloc(sizeof(alcdwl_context_t));
	if(dwl == IM_NULL){
		IM_ERRMSG((IM_STR("malloc(dwl) failed")));
		return IM_NULL;
	}
	memset((void *)dwl, 0, sizeof(alcdwl_context_t));
	dwl->pmm_fd = -1;	// mark it invalid first.

	dwl->pmm_fd = open(PMM_DEV_NAME, O_RDWR);
	if(dwl->pmm_fd < 0){
		IM_ERRMSG((IM_STR("open(%s) failed"), PMM_DEV_NAME));
		goto Fail;
	}

	if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_INIT, owner)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_INIT) failed")));
		goto Fail;
	}

	return (alcdwl_t)dwl;
Fail:
	alcdwl_deinit((alcdwl_t)dwl);
	return IM_NULL;
}

IM_RET alcdwl_deinit(alcdwl_t handle)
{
	alcdwl_context_t *dwl = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

	if(dwl->pmm_fd != -1){
		if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DEINIT, IM_NULL)){
			IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DEINIT) failed")));
		}
		close(dwl->pmm_fd);
	}
	alcdwl_free((void *)dwl);

	return IM_RET_OK;
}

void * alcdwl_malloc(IM_INT32 size)
{
	return malloc(size);
}

void alcdwl_free(void *p)
{
	free(p);
}

void * alcdwl_memset(void *p, IM_CHAR c, IM_INT32 size)
{
	return memset(p, c, size);
}

void * alcdwl_memcpy(void *dst, void *src, IM_INT32 size)
{
	return memcpy(dst, src, size);
}

static IM_RET alcdwl_mm_add_useraddr_for_statc(IN alcdwl_t handle, IN void *uVirAddr, IN IM_INT32 size)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_mm_add_useraddr_for_statc_t ds_acfs;
	IM_INFOMSG((IM_STR("%s(uVirAddr=0x%x, size=%d)"), IM_STR(_IM_FUNC_), (IM_INT32)uVirAddr, size));

	ds_acfs.uVirAddr = uVirAddr;
	ds_acfs.size = size;
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_ADD_USERADDR_FOR_STATC, (void *)&ds_acfs)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_ADD_USERADDR_FOR_STATC) failed")));
		return IM_RET_FAILED;
	}

	return IM_RET_OK;
}

static IM_RET alcdwl_mm_del_useraddr_for_statc(IN alcdwl_t handle, IN void *uVirAddr)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(uVirAddr=0x%x)"), IM_STR(_IM_FUNC_), (IM_INT32)uVirAddr));
	
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_DEL_USERADDR_FOR_STATC, (void *)&uVirAddr)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_DEL_USERADDR_FOR_STATC) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_init_bigmem(IN alcdwl_t handle, IN IM_INT32 blockSize, IN IM_INT32 blockNum, IN IM_BOOL isCache)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_mm_init_bigmem_t ds_ib;
	IM_INFOMSG((IM_STR("%s(blockSize=%d, blockNum=%d, isCache=%d)"), IM_STR(_IM_FUNC_), blockSize, blockNum, isCache));

	ds_ib.blockSize = blockSize;
	ds_ib.blockNum = blockNum;
	ds_ib.isCache = isCache;
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_INIT_BIGMEM, (void *)&ds_ib)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_INIT_BIGMEM) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_deinit_bigmem(IN alcdwl_t handle)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_DEINIT_BIGMEM, IM_NULL)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_DEINIT_BIGMEM) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_mm_alloc(IN alcdwl_t handle, IN IM_INT32 size, IN IM_INT32 flag, OUT alc_buffer_t *alcBuffer)
{
	IM_RET ret = IM_RET_OK;
	IM_BOOL alcVir;
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_mm_alloc_t ds_ma;
	IM_INFOMSG((IM_STR("%s(size=%d, flag=0x%x)"), IM_STR(_IM_FUNC_), size, flag));

	alcdwl_memset((void *)alcBuffer, 0, sizeof(alc_buffer_t));

	if((flag & ALC_FLAG_PHY_MUST) || (flag & ALC_FLAG_PHY_LINEAR_PREFER) || (flag & ALC_FLAG_PHY_PREFER)){
		alcVir = IM_FALSE;
		ds_ma.size = size;
		ds_ma.flag = flag;
		if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_ALLOC, (void *)&ds_ma)){
			IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_ALLOC) failed")));
			ret = IM_RET_FAILED;
			if(flag & ALC_FLAG_PHY_PREFER){	// it could use vir_alloc.
				alcVir = IM_TRUE;
				ret = IM_RET_FALSE;
			}
		}else{
			alcdwl_memcpy((void *)alcBuffer, (void *)&ds_ma.alcBuffer, sizeof(alc_buffer_t));
			IM_ASSERT(alcBuffer->attri & ALC_BUFFER_ATTRI_ALLOCATED);
			//IM_ASSERT(alcBuffer->buffer.flag & IM_BUFFER_FLAG_DEVADDR);
			IM_ASSERT(IM_BUFFER_HAS_PHY_FLAG(alcBuffer->buffer));

			alcBuffer->buffer.vir_addr = mmap(0, alcBuffer->buffer.size, PROT_READ | PROT_WRITE, 
				(flag & ALC_FLAG_CACHE) ? MAP_PRIVATE : MAP_SHARED, alc->pmm_fd, alcBuffer->buffer.phy_addr);
			IM_ASSERT(alcBuffer->buffer.vir_addr != MAP_FAILED);
		}
	}else{
		alcVir = IM_TRUE;
	}

	if(alcVir == IM_TRUE){
		alcBuffer->buffer.vir_addr = malloc(size);
		if(alcBuffer->buffer.vir_addr == IM_NULL){
			IM_ERRMSG((IM_STR("malloc viraddr failed")));
			ret = IM_RET_FAILED;
		}
		alcBuffer->buffer.size = size;
		alcBuffer->attri = ALC_BUFFER_ATTRI_ALLOCATED;
#ifdef VIRTUAL_ALLOC_STATC
		alcdwl_mm_add_useraddr_for_statc(handle, alcBuffer->buffer.vir_addr, size);
#endif
	}

	return ret;
}

IM_RET alcdwl_mm_free(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(vir_addr=0x%x)"), IM_STR(_IM_FUNC_), (IM_INT32)alcBuffer->buffer.vir_addr));
	IM_ASSERT(alcBuffer->attri & ALC_BUFFER_ATTRI_ALLOCATED);

	if(IM_BUFFER_HAS_PHY_FLAG(alcBuffer->buffer)){
		IM_ASSERT(alcBuffer->buffer.vir_addr != IM_NULL);
		munmap((void *)alcBuffer->buffer.vir_addr, alcBuffer->buffer.size);
		if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_FREE, (void *)alcBuffer)){
			IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_FREE) failed")));
			return IM_RET_FAILED;
		}
	}else{
		free(alcBuffer->buffer.vir_addr);
#ifdef VIRTUAL_ALLOC_STATC
		alcdwl_mm_del_useraddr_for_statc(handle, alcBuffer->buffer.vir_addr);
#endif
	}

	return IM_RET_OK;
}

IM_RET alcdwl_mm_map_useraddr(IN alcdwl_t handle, IN void *uVirAddr, IN IM_INT32 size, OUT alc_buffer_t *alcBuffer)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_mm_map_useraddr_t ds_mu;
	IM_INFOMSG((IM_STR("%s(uVirAddr=0x%x, size=%d)"), IM_STR(_IM_FUNC_), (IM_INT32)uVirAddr, size));

	ds_mu.uVirAddr = uVirAddr;
	ds_mu.size = size;
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_MAP_USERADDR, (void *)&ds_mu)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_MAP_USERADDR) failed")));
		return IM_RET_FAILED;
	}

	alcdwl_memcpy((void *)alcBuffer, (void *)&ds_mu.alcBuffer, sizeof(alc_buffer_t));	
	IM_ASSERT((alcBuffer->attri & ALC_BUFFER_ATTRI_MAPPED) && (alcBuffer->attri & ALC_BUFFER_ATTRI_DEVADDR));
	IM_ASSERT(alcBuffer->buffer.flag & IM_BUFFER_FLAG_MAPPED);
	IM_ASSERT(alcBuffer->buffer.flag & IM_BUFFER_FLAG_DEVADDR);
	IM_ASSERT(IM_BUFFER_HAS_PHY_FLAG(alcBuffer->buffer));

	return IM_RET_OK;
}

IM_RET alcdwl_mm_unmap_useraddr(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(vir_addr=0x%x)"), IM_STR(_IM_FUNC_), (IM_INT32)alcBuffer->buffer.vir_addr));

	IM_ASSERT((alcBuffer->attri & ALC_BUFFER_ATTRI_MAPPED) && (alcBuffer->attri & ALC_BUFFER_ATTRI_DEVADDR));
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_UNMAP_USERADDR, (void *)alcBuffer)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_UNMAP_USERADDR) failed")));
		return IM_RET_FAILED;
	}

	return IM_RET_OK;
}

IM_RET alcdwl_flush_buffer(IN alcdwl_t handle, IN alc_buffer_t *alcBuffer, IN IM_INT32 flag)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_mm_flush_buffer_t ds_fb;
	IM_INFOMSG((IM_STR("%s(flag=%d)"), IM_STR(_IM_FUNC_), flag));

	//IM_ASSERT(alcBuffer->attri & ALC_BUFFER_ATTRI_MAPPED);
	
	ds_fb.flag = flag;
	memcpy((void *)&ds_fb.alcBuffer, (void *)alcBuffer, sizeof(alc_buffer_t));
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_MM_FLUSH_BUFFER, &ds_fb)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_MM_FLUSH_BUFFER) failed")));
		return IM_RET_FAILED;
	}

	return IM_RET_OK;
}

IM_RET alcdwl_statc_lock_access(IN alcdwl_t handle)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_LOCK_ACCESS, IM_NULL)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_LOCK_ACCESS) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_statc_unlock_access(IN alcdwl_t handle)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_UNLOCK_ACCESS, IM_NULL)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_UNLOCK_ACCESS) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_statc_get_owner_num(IN alcdwl_t handle, OUT IM_INT32 *num)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_GET_OWNER_NUM, (void *)num)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_GET_OWNER_NUM) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET alcdwl_statc_get_owner(IN alcdwl_t handle, IN IM_INT32 index, OUT IM_TCHAR *owner)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_statc_get_owner_t ds_sgo;
	IM_INFOMSG((IM_STR("%s(index=%d)"), IM_STR(_IM_FUNC_), index));
	
	ds_sgo.index = index;
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_GET_OWNER, (void *)&ds_sgo)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_GET_OWNER) failed")));
		return IM_RET_FAILED;
	}
	strcpy(owner, ds_sgo.owner);

	return IM_RET_OK;
}

IM_RET alcdwl_statc_get_owner_buffer_num(IN alcdwl_t handle, IN IM_TCHAR *owner, OUT IM_INT32 *num)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_statc_get_owner_buffer_num_t ds_gobn;
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), owner));
	
	strcpy(ds_gobn.owner, owner);
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_GET_OWNER_BUFFER_NUM, (void *)&ds_gobn)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_GET_OWNER_BUFFER_NUM) failed")));
		return IM_RET_FAILED;
	}
	*num = ds_gobn.num;

	return IM_RET_OK;
}

IM_RET alcdwl_statc_get_owner_buffer(IN alcdwl_t handle, IN IM_TCHAR *owner, IN IM_INT32 index, OUT alc_buffer_t *alcBuffer)
{
	alcdwl_context_t *alc = (alcdwl_context_t *)handle;
	pmmlib_ioctl_ds_statc_get_owner_buffer_t ds_gob;
	IM_INFOMSG((IM_STR("%s(owner=%s, index=%d)"), IM_STR(_IM_FUNC_), owner, index));
	
	strcpy(ds_gob.owner, owner);
	ds_gob.index = index;
	if(ioctl(alc->pmm_fd, PMMLIB_IOCTL_STATC_GET_OWNER_BUFFER, (void *)&ds_gob)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_STATC_GET_OWNER_BUFFER) failed")));
		return IM_RET_FAILED;
	}
	alcdwl_memcpy((void *)alcBuffer, (void *)&ds_gob.alcBuffer, sizeof(alc_buffer_t));

	return IM_RET_OK;
}





