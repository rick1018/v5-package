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
** v1.2.4	leo@2012/04/28: modify alc_map_useraddr() parameter.
**          leo@2012/05/04: modify alc_alloc() size from IM_UINT32 to IM_INT32 to 
**                 avoid ld this function failed bug.
** v1.2.9   leo@2012/09/10: add bigmem interface.
** v1.2.12  leo@2012/11/30: modify alc_init_bigmem() interface.
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_buffallocapi.h>
#if (TARGET_SYSTEM == FS_ANDROID)
#include "mm_dma.h"
#elif (TARGET_SYSTEM == FS_WINCE)
#include "mm_dma_lib.h"
#endif
#include <buffalloc.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"ALC_I:"
#define WARNHEAD	"ALC_W:"
#define ERRHEAD		"ALC_E:"
#define TIPHEAD		"ALC_T:"


//#############################################################################
BUFFALLOC_API IM_UINT32 alc_version(IM_TCHAR *ver_string)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if(ver_string != IM_NULL){
		IM_Strcpy(ver_string, (IM_TCHAR *)IM_STR(VER_STRING));
	}
	return IM_MAKE_VERSION(VER_MAJOR, VER_MINOR, VER_PATCH);
}

BUFFALLOC_API IM_RET alc_open(OUT ALCCTX *alc, IN IM_TCHAR *owner)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if((alc == IM_NULL) || ((owner != IM_NULL) && (IM_Strlen(owner) >= ALC_OWNER_LEN_MAX))){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	*alc = IM_NULL;

	IM_BuffAlloc *inst = new IM_BuffAlloc(owner);
	if(inst == IM_NULL){
		IM_ERRMSG((IM_STR("new IM_BuffAlloc() failed")));
		return IM_RET_NOMEMORY;
	}	

	if(IM_RET_OK != inst->open()){
		IM_ERRMSG((IM_STR("buffalloc->open() failed")));
		delete inst;
		return IM_RET_FAILED;
	}

	*alc = (ALCCTX)inst;
	return IM_RET_OK;
}

BUFFALLOC_API IM_RET alc_close(IN ALCCTX alc)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if(alc == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	IM_BuffAlloc *inst = (IM_BuffAlloc *)alc;
	IM_RET ret = inst->close();
	delete inst;

	return ret;
}

BUFFALLOC_API IM_RET alc_alloc(IN ALCCTX alc, IN IM_INT32 size, OUT IM_Buffer *buffer, IN IM_INT32 flag)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if((alc == IM_NULL) || (size <= 0) || (buffer == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	return ((IM_BuffAlloc *)alc)->allocBuffer(size, buffer, flag);
}

BUFFALLOC_API IM_RET alc_free(IN ALCCTX alc, IN IM_Buffer *buffer)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if((alc == IM_NULL) || (buffer == IM_NULL)){
		IM_ERRMSG((IM_STR("invalid parameter, NULL pointer")));
		return IM_RET_INVALID_PARAMETER;
	}

	return ((IM_BuffAlloc *)alc)->freeBuffer(buffer);
}

BUFFALLOC_API IM_RET alc_map_useraddr(IN ALCCTX inst, IN void *usrVirAddr, IN IM_INT32 size, OUT IM_Buffer *buffer, OUT IM_UINT32 *devAddr)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_unmap_useraddr(IN ALCCTX inst, IN IM_Buffer *buffer)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_flush_buffer(IN ALCCTX inst, IN IM_Buffer *buffer, IN IM_INT32 flag)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_get_devaddr(IN ALCCTX inst, IN IM_Buffer *buffer, OUT IM_UINT32 *devAddr)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_statc_list_owner(IN ALCCTX inst, INOUT im_list_handle_t list)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_statc_list_owner_buffer(IN ALCCTX inst, IN IM_TCHAR *owenr, INOUT im_list_handle_t list)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_init_bigmem(IN ALCCTX inst, IN IM_INT32 blockSize, IN IM_INT32 blockNum, IN IM_BOOL isCache)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

BUFFALLOC_API IM_RET alc_deinit_bigmem(IN ALCCTX inst)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	return IM_RET_NOTSUPPORT;
}

//#############################################################################
IM_BuffAlloc::IM_BuffAlloc(IM_TCHAR *owner) :
	mMmInst(IM_NULL)
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), (owner==IM_NULL)?IM_STR("unknown"):owner));
	if(owner != IM_NULL){
		IM_Strcpy(mOwner, owner);
	}else{
		IM_Strcpy(mOwner, IM_STR("unknown"));
	}

	mBuffList.clear();
}

IM_BuffAlloc::~IM_BuffAlloc()
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	close();
}

IM_RET IM_BuffAlloc::open()
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	mMmInst = MmOpen();
	if(mMmInst == IM_NULL){
		IM_ERRMSG((IM_STR("[%s] MmOpen() failed"), mOwner));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET IM_BuffAlloc::close()
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	IM_Buffer *bf = mBuffList.begin();
	while(bf != IM_NULL){
		freeBuffer(bf);
		bf = mBuffList.erase(bf);
	}

	if(mMmInst != IM_NULL){
		MmClose(mMmInst);
		mMmInst = IM_NULL;
	}

	return IM_RET_OK;
}

IM_RET IM_BuffAlloc::allocBuffer(IM_UINT32 size, IM_Buffer *buff, IM_INT32 flag)
{
	IM_INFOMSG((IM_STR("%s(owner=%s, size=%d)"), IM_STR(_IM_FUNC_), mOwner, size));
	IM_RET ret;

	if((buff == IM_NULL) || (size == 0)){
		IM_ERRMSG((IM_STR("[%s] invalid parameter"), mOwner));
		return IM_RET_INVALID_PARAMETER;
	}

	if((flag & ALC_FLAG_PHY_PREFER) || (flag & ALC_FLAG_PHY_MUST)){
		ret = allocPhyBuffer(size, buff, flag);
	}else{
		ret = allocVirBuffer(size, buff);
	}

	if((ret == IM_RET_OK) || (ret == IM_RET_FALSE)){
		mBuffList.put_back(buff);
	}
	return ret;
}

IM_RET IM_BuffAlloc::freeBuffer(IM_Buffer *buff)
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	IM_RET ret;

	if((buff == IM_NULL) || (buff->size == 0)){
		IM_ERRMSG((IM_STR("[%s] invalid parameter"), mOwner));
		return IM_RET_INVALID_PARAMETER;
	}

	IM_Buffer *bf = mBuffList.begin();
	while(bf != IM_NULL){
		if(bf->vir_addr == buff->vir_addr){
			if(bf->flag & IM_BUFFER_FLAG_PHY){
				if(bf->phy_addr == buff->phy_addr){
					break;
				}
			}else{
				break;
			}
		}
		bf = mBuffList.next();
	}
	if(bf == IM_NULL){
		IM_ERRMSG((IM_STR("[%s] invalid parameter"), mOwner));
		return IM_RET_INVALID_PARAMETER;
	}

	if(bf->flag & IM_BUFFER_FLAG_PHY){
		ret = freePhyBuffer(bf);
	}else{
		ret = freeVirBuffer(bf);
	}

	mBuffList.erase(bf);

	return ret;
}

IM_RET IM_BuffAlloc::allocVirBuffer(IM_UINT32 size, IM_Buffer *buff)
{
	IM_INFOMSG((IM_STR("%s(owner=%s, size=%d)"), IM_STR(_IM_FUNC_), mOwner, size));
	buff->vir_addr = malloc(size);
	if(buff->vir_addr == IM_NULL){
		IM_ERRMSG((IM_STR("[%s] malloc(%d) failed"), mOwner, size));
		return IM_RET_FAILED;
	}

	buff->phy_addr = 0;
	buff->size = size;
	buff->flag = 0;
	return IM_RET_OK;
}

IM_RET IM_BuffAlloc::allocPhyBuffer(IM_UINT32 size, IM_Buffer *buff, IM_INT32 flag)
{
	IM_INFOMSG((IM_STR("%s(owner=%s, size=%d)"), IM_STR(_IM_FUNC_), mOwner, size));
	struct_mm_page page;
	struct_mm_alloc alloc;

	page.size = size;
	alloc.page = &page;
	alloc.align = MMALIGN_64B;
	alloc.mpool = IM_FALSE;	
	if(MMRET_OK != MmAlloc(mMmInst, &alloc)){
		if(flag & ALC_FLAG_PHY_MUST){
			IM_ERRMSG((IM_STR("[%s] MmAlloc() failed"), mOwner));
			return IM_RET_FAILED;
		}else{
			return allocVirBuffer(size, buff);
		}
	}
	buff->vir_addr = page.vir_addr;
	buff->phy_addr = page.phy_addr;
	buff->size = page.size;
	buff->flag = IM_BUFFER_FLAG_PHY;	
	return IM_RET_OK;
}

IM_RET IM_BuffAlloc::freeVirBuffer(IM_Buffer *buff)
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	free(buff->vir_addr);
	return IM_RET_OK;
}

IM_RET IM_BuffAlloc::freePhyBuffer(IM_Buffer *buff)
{
	IM_INFOMSG((IM_STR("%s(owner=%s)"), IM_STR(_IM_FUNC_), mOwner));
	struct_mm_page page;
	page.vir_addr = buff->vir_addr;
	page.phy_addr = buff->phy_addr;
	page.size = buff->size;
	if(MMRET_OK != MmFree(mMmInst, &page)){
		IM_ERRMSG((IM_STR("[%s] MmFree() failed"), mOwner));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

