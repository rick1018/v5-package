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
** v1.2.1	leo@2012/03/28: modify the check result of the im_list_put_back() function.
** v1.2.4	leo@2012/04/28: 
**			1. modify alc_map_useraddr().
**			2. modify owner check in alc_open().
**			3. add alc_flush_buffer() functions.
**          leo@2012/05/16: modify alc_flush_buffer().
** v1.2.9   leo@2012/09/10: add bigmem support.
** v1.2.12  leo@2012/11/30: modify alc_init_bigmem() interface.
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_buffallocapi.h>
#include <pmm_lib.h>
#include <alc_dwl.h>
#include <buffalloc.h>


#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"ALC_I:"
#define WARNHEAD	"ALC_W:"
#define ERRHEAD		"ALC_E:"
#define TIPHEAD		"ALC_T:"

//
typedef struct{
	IM_TCHAR		owner[ALC_OWNER_LEN_MAX + 1];
	im_mempool_handle_t	mpl;
	im_list_handle_t	abfList;

	alcdwl_t		dwl;
}alc_instance_t;


BUFFALLOC_API IM_UINT32 alc_version(IM_TCHAR *ver_string)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if(ver_string != IM_NULL){
		im_oswl_strcpy(ver_string, (IM_TCHAR *)IM_STR(VER_STRING));
	}
	return IM_MAKE_VERSION(VER_MAJOR, VER_MINOR, VER_PATCH);
}

BUFFALLOC_API IM_RET alc_open(OUT ALCCTX *inst, IN IM_TCHAR *owner)
{
	alc_instance_t *alc;	
	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), owner));

	// check parameter.
	if((inst == IM_NULL) || ((owner == IM_NULL) || (im_oswl_strlen(owner) >= ALC_OWNER_LEN_MAX))){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	alc = (alc_instance_t *)alcdwl_malloc(sizeof(alc_instance_t));
	if(alc == IM_NULL){
		IM_ERRMSG((IM_STR("malloc(alc) failed")));
		return IM_RET_FAILED;
	}
	alcdwl_memset((void *)alc, 0, sizeof(alc_instance_t));
	im_oswl_strcpy(alc->owner, owner);

	alc->dwl = alcdwl_init(alc->owner);	// must use alc->owner.
	if(alc->dwl == IM_NULL){
		IM_ERRMSG((IM_STR("alcdwl_init() failed")));
		goto Fail;
	}

	alc->mpl = im_mpool_init((func_mempool_malloc_t)alcdwl_malloc, (func_mempool_free_t)alcdwl_free);
	if(alc->mpl == IM_NULL){
		IM_ERRMSG((IM_STR("im_mpool_init() failed")));
		goto Fail;
	}

	alc->abfList = im_list_init(sizeof(alc_buffer_t), alc->mpl);
	if(alc->abfList == IM_NULL){
		IM_ERRMSG((IM_STR("im_list_init() failed")));
		goto Fail;
	}

	//
	*inst = (ALCCTX)alc;
	return IM_RET_OK;
Fail:
	alc_close((ALCCTX)alc);
	return IM_RET_FAILED;
}

BUFFALLOC_API IM_RET alc_close(IN ALCCTX inst)
{
	alc_buffer_t *abf;
	alc_instance_t *alc = (alc_instance_t *)inst;

	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), alc->owner));

	//
	if(inst == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	if(alc->abfList != IM_NULL){
		abf = (alc_buffer_t *)im_list_begin(alc->abfList);
		while(abf != IM_NULL){
			if(abf->attri & ALC_BUFFER_ATTRI_ALLOCATED){
				alcdwl_mm_free(alc->dwl, abf);
			}else if(abf->attri & ALC_BUFFER_ATTRI_MAPPED){
				alcdwl_mm_unmap_useraddr(alc->dwl, abf);
			}else{
				IM_ASSERT(IM_FALSE);
			}
			abf = (alc_buffer_t *)im_list_erase(alc->abfList, abf);
		}
		im_list_deinit(alc->abfList);
	}
	if(alc->mpl != IM_NULL){
		im_mpool_deinit(alc->mpl);
	}
	if(alc->dwl != IM_NULL){
		alcdwl_deinit(alc->dwl);
	}
	alcdwl_free((void *)inst);

	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_init_bigmem(IN ALCCTX inst, IN IM_INT32 blockSize, IN IM_INT32 blockNum, IN IM_BOOL isCache)
{
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s, blockSize=%d, blockNum=%d, isCache=%d)"), IM_STR(_IM_FUNC_), alc->owner, blockSize, blockNum, isCache));

	if((inst == IM_NULL) || (blockSize <= 0) || (blockNum <= 0)){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

    if(alcdwl_init_bigmem(alc->dwl, blockSize, blockNum, isCache) != IM_RET_OK){
        IM_ERRMSG((IM_STR("alcdwl_init_bigmem(blockSize=%d, blockNum=%d, isCache=%d) failed"), blockSize, blockNum, isCache));
        return IM_RET_FAILED;
    }

    return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_deinit_bigmem(IN ALCCTX inst)
{
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), alc->owner));

	if(inst == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

    if(alcdwl_deinit_bigmem(alc->dwl) != IM_RET_OK){
        IM_ERRMSG((IM_STR("alcdwl_deinit_bigmem() failed")));
        return IM_RET_FAILED;
    }

    return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_alloc(IN ALCCTX inst, IN IM_INT32 size, OUT IM_Buffer *buffer, IN IM_INT32 flag)
{
	IM_RET ret;
	alc_buffer_t abf;
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s, size=0x%x, flag=0x%x)"), IM_STR(_IM_FUNC_), alc->owner, size, flag));

	if((inst == IM_NULL) || (buffer == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}
	if(!(flag & ALC_FLAG_BIGMEM) && (size <= 0)){
		IM_ERRMSG((IM_STR("invalid parameter, size=%d, flag=0x%x"), size, flag));
		return IM_RET_INVALID_PARAMETER;
	}

	if(flag & ALC_FLAG_BIGMEM){ // bigmem must combined with phy_must.
		flag |= ALC_FLAG_PHY_MUST;
	}

	ret = alcdwl_mm_alloc(alc->dwl, size, flag, &abf);
	if(IM_FAILED(ret)){	// ret maybe IM_RET_FALSE
		IM_ERRMSG((IM_STR("alcdwl_mm_alloc() failed")));
		return IM_RET_FAILED;
	}
	IM_ASSERT(abf.attri & ALC_BUFFER_ATTRI_ALLOCATED);

	if(im_list_put_back(alc->abfList, &abf) != IM_RET_OK){
		IM_ERRMSG((IM_STR("im_list_put_back() failed")));
		alcdwl_mm_free(alc->dwl, &abf);
		return IM_RET_FAILED;
	}

	IM_BUFFER_COPYTO_pBUFFER(buffer, abf.buffer);
	IM_INFOMSG((IM_STR("abf: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x, devAddr=0x%x, attri=0x%x"), 
		(IM_INT32)abf.buffer.vir_addr, abf.buffer.phy_addr, abf.buffer.size, abf.buffer.flag, abf.devAddr, abf.attri));
	
	return ret;
}

BUFFALLOC_API IM_RET alc_free(IN ALCCTX inst, IN IM_Buffer *buffer)
{
	alc_buffer_t *abf;
	alc_instance_t *alc = (alc_instance_t *)inst;

	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), alc->owner));

	//
	if((alc == IM_NULL) || (buffer == IM_NULL) || (buffer->flag & IM_BUFFER_FLAG_MAPPED)){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	abf = (alc_buffer_t *)im_list_begin(alc->abfList);
	while(abf != IM_NULL){
		if((abf->buffer.vir_addr == buffer->vir_addr) && (abf->attri & ALC_BUFFER_ATTRI_ALLOCATED)){
			break;
		}
		abf = (alc_buffer_t *)im_list_next(alc->abfList);
	}

	if(abf == IM_NULL){
		IM_ERRMSG((IM_STR("not found the allocated buffer, vir_addr=0x%x"), (IM_INT32)buffer->vir_addr));
		return IM_RET_FAILED;
	}

	//
	if(alcdwl_mm_free(alc->dwl, abf) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_mm_free() failed")));
		return IM_RET_FAILED;
	}
	im_list_erase(alc->abfList, abf);

	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_map_useraddr(IN ALCCTX inst, IN void *usrVirAddr, IN IM_INT32 size, OUT IM_Buffer *buffer, OUT IM_UINT32 *devAddr)
{
	alc_buffer_t abf;
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s, usrVirAddr=0x%x, size=0x%x)"), IM_STR(_IM_FUNC_), alc->owner, (IM_INT32)usrVirAddr, size));

	if((inst == IM_NULL) || (usrVirAddr == IM_NULL) || (size <= 0) || (buffer == IM_NULL) || (devAddr == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	if(alcdwl_mm_map_useraddr(alc->dwl, usrVirAddr, size, &abf) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_mm_map_useraddr() failed")));
		return IM_RET_FAILED;
	}
	IM_ASSERT(abf.attri & ALC_BUFFER_ATTRI_MAPPED);
	IM_ASSERT(abf.buffer.flag & IM_BUFFER_FLAG_MAPPED);
	IM_ASSERT(abf.buffer.flag & IM_BUFFER_FLAG_DEVADDR);

	if(im_list_put_back(alc->abfList, &abf) != IM_RET_OK){
		IM_ERRMSG((IM_STR("im_list_put_back() failed")));
		alcdwl_mm_free(alc->dwl, &abf);
		return IM_RET_FAILED;
	}

	IM_BUFFER_COPYTO_pBUFFER(buffer, abf.buffer);
	*devAddr = abf.devAddr;
	IM_INFOMSG((IM_STR("abf: vir_addr=0x%x, phy_addr=0x%x, size=%d, flag=0x%x, devAddr=0x%x, attri=0x%x"), 
		(IM_INT32)abf.buffer.vir_addr, abf.buffer.phy_addr, abf.buffer.size, abf.buffer.flag, abf.devAddr, abf.attri));

	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_unmap_useraddr(IN ALCCTX inst, IN IM_Buffer *buffer)
{
	alc_buffer_t *abf;
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s, vir_addr=0x%x, phy_addr=0x%x, size=0x%x, flag=0x%x)"), 
		IM_STR(_IM_FUNC_), alc->owner, (IM_INT32)buffer->vir_addr, buffer->phy_addr, buffer->size, buffer->flag));

	//
	if((inst == IM_NULL) || (buffer == IM_NULL) || (!(buffer->flag & IM_BUFFER_FLAG_MAPPED))){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	abf = (alc_buffer_t *)im_list_begin(alc->abfList);
	while(abf != IM_NULL){
		if((abf->buffer.vir_addr == buffer->vir_addr) && (abf->attri & ALC_BUFFER_ATTRI_MAPPED)){
			break;
		}
		abf = (alc_buffer_t *)im_list_next(alc->abfList);
	}

	if(abf == IM_NULL){
		IM_ERRMSG((IM_STR("not found the mapped buffer, vir_addr=0x%x"), (IM_INT32)buffer->vir_addr));
		return IM_RET_FAILED;
	}

	//
	IM_INFOMSG((IM_STR("%s, devAddr=0x%x"), alc->owner, abf->devAddr));
	if(alcdwl_mm_unmap_useraddr(alc->dwl, abf) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_mm_unmap_useraddr() failed")));
		return IM_RET_FAILED;
	}
	im_list_erase(alc->abfList, abf);

	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_flush_buffer(IN ALCCTX inst, IN IM_Buffer *buffer, IN IM_INT32 flag)
{
	alc_buffer_t *abf;
	alc_instance_t *alc = (alc_instance_t *)inst;

	IM_INFOMSG((IM_STR("%s(%s, flag=%d)"), IM_STR(_IM_FUNC_), alc->owner, flag));

	//
	if((alc == IM_NULL) || (buffer == IM_NULL) || ((flag != ALC_FLUSH_DEVICE_TO_CPU) && (flag != ALC_FLUSH_CPU_TO_DEVICE))){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	abf = (alc_buffer_t *)im_list_begin(alc->abfList);
	while(abf != IM_NULL){
		if(abf->buffer.vir_addr == buffer->vir_addr){
			break;
		}
		abf = (alc_buffer_t *)im_list_next(alc->abfList);
	}

	if(abf == IM_NULL){
		IM_ERRMSG((IM_STR("not found the buffer, vir_addr=0x%x"), (IM_INT32)buffer->vir_addr));
		return IM_RET_FAILED;
	}
	if(!(abf->attri & ALC_BUFFER_ATTRI_MAPPED) && !(abf->attri & ALC_BUFFER_ATTRI_ALLOCATED)){
		IM_ERRMSG((IM_STR("invalid buffer, attri=0x%x"), abf->attri));
		return IM_RET_FAILED;
	}

	//
	if(alcdwl_flush_buffer(alc->dwl, abf, flag) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_flush_buffer() failed")));
		return IM_RET_FAILED;
	}

	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_get_devaddr(IN ALCCTX inst, IN IM_Buffer *buffer, OUT IM_UINT32 *devAddr)
{
	alc_buffer_t *abf;
	alc_instance_t *alc = (alc_instance_t *)inst;

	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), alc->owner));

	//
	if((alc == IM_NULL) || (buffer == IM_NULL) || (devAddr == IM_NULL) || (!(buffer->flag & IM_BUFFER_FLAG_DEVADDR))){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	abf = (alc_buffer_t *)im_list_begin(alc->abfList);
	while(abf != IM_NULL){
		if((abf->buffer.vir_addr == buffer->vir_addr) && (abf->attri & ALC_BUFFER_ATTRI_DEVADDR)){
			break;
		}
		abf = (alc_buffer_t *)im_list_next(alc->abfList);
	}

	if(abf == IM_NULL){
		IM_ERRMSG((IM_STR("not found the buffer, vir_addr=0x%x"), (IM_INT32)buffer->vir_addr));
		return IM_RET_FAILED;
	}else if(!(abf->buffer.flag & IM_BUFFER_FLAG_DEVADDR)){
		IM_ERRMSG((IM_STR("this buffer no devAddr, vir_addr=0x%x"), (IM_INT32)buffer->vir_addr));
		return IM_RET_FAILED;
	}

	*devAddr = abf->devAddr;
	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_statc_list_owner(IN ALCCTX inst, INOUT im_list_handle_t list)
{
	IM_TCHAR owner[ALC_OWNER_LEN_MAX];
	IM_INT32 i, num;
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s)"), IM_STR(_IM_FUNC_), alc->owner));

	//
	if((alc == IM_NULL) || (list == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	if(alcdwl_statc_lock_access(alc->dwl) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_statc_lock_access() failed")));
		return IM_RET_FAILED;
	}

	im_list_clear(list);
	IM_JIF(alcdwl_statc_get_owner_num(alc->dwl, &num));
	for(i=0; i<num; i++){
		IM_JIF(alcdwl_statc_get_owner(alc->dwl, i, owner));
		IM_JIF(im_list_put_back(list, (void *)owner));
	}

	alcdwl_statc_unlock_access(alc->dwl);
	return IM_RET_OK;
Fail:
	alcdwl_statc_unlock_access(alc->dwl);
	return IM_RET_FAILED;
}

BUFFALLOC_API IM_RET alc_statc_list_owner_buffer(IN ALCCTX inst, IN IM_TCHAR *owner, INOUT im_list_handle_t list)
{
	IM_INT32 i, num;
	alc_buffer_t abf;
	alc_instance_t *alc = (alc_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s(%s, list-owner=%s)"), IM_STR(_IM_FUNC_), alc->owner, owner));

	//
	if((alc == IM_NULL) || (owner == IM_NULL) || (list == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	//
	if(alcdwl_statc_lock_access(alc->dwl) != IM_RET_OK){
		IM_ERRMSG((IM_STR("alcdwl_statc_lock_access() failed")));
		return IM_RET_FAILED;
	}

	im_list_clear(list);
	IM_JIF(alcdwl_statc_get_owner_buffer_num(alc->dwl, owner, &num));
	for(i=0; i<num; i++){
		IM_JIF(alcdwl_statc_get_owner_buffer(alc->dwl, owner, i, &abf));
		IM_JIF(im_list_put_back(list, (void *)&abf));
	}

	alcdwl_statc_unlock_access(alc->dwl);
	return IM_RET_OK;
Fail:
	alcdwl_statc_unlock_access(alc->dwl);
	return IM_RET_FAILED;
}



