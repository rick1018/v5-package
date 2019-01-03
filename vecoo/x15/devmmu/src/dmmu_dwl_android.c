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
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_buffallocapi.h>
#include <IM_devmmuapi.h>
#include <pmm_lib.h>
#include <dmmu_dwl.h>


#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"DMMUDWL_I:"
#define WARNHEAD	"DMMUDWL_I:"
#define ERRHEAD		"DMMUDWL_I:"
#define TIPHEAD		"DMMUDWL_I:"

//
#define PMM_DEV_NAME	"/dev/pmm"

//
typedef struct{
	IM_INT32	pmm_fd;
	IM_INT32	devid;
}dmmudwl_context_t;


dmmudwl_t dmmudwl_init(IM_INT32 devid)
{
	dmmudwl_context_t *dwl;
	IM_TCHAR owner[ALC_OWNER_LEN_MAX];
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

	//
	dwl = (dmmudwl_context_t *)dmmudwl_malloc(sizeof(dmmudwl_context_t));
	if(dwl == IM_NULL){
		IM_ERRMSG((IM_STR("malloc(dwl) failed")));
		return IM_NULL;
	}
	memset((void *)dwl, 0, sizeof(dmmudwl_context_t));

	dwl->pmm_fd = open(PMM_DEV_NAME, O_RDWR);
	if(dwl->pmm_fd < 0){
		IM_ERRMSG((IM_STR("open(%s) failed"), PMM_DEV_NAME));
		goto Fail;
	}

	sprintf(owner, "dmmu%d", devid);
	if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_INIT, owner)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_INIT) failed")));
		goto Fail;
	}
	
	if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DMMU_INIT, &devid)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DMMU_INIT) failed")));
		goto Fail;
	}
	dwl->devid = devid;

	return (dmmudwl_t)dwl;
Fail:
	dmmudwl_deinit((dmmudwl_t)dwl);
	return IM_NULL;
}

IM_RET dmmudwl_deinit(dmmudwl_t handle)
{
	dmmudwl_context_t *dwl = (dmmudwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), dwl->devid));

	if(dwl->pmm_fd != 0){
		if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DMMU_DEINIT, IM_NULL)){
			IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DMMU_DEINIT) failed")));
		}
		if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DEINIT, IM_NULL)){
			IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DEINIT) failed")));
		}
		close(dwl->pmm_fd);
	}
	dmmudwl_free((void *)dwl);

	return IM_RET_OK;
}

void * dmmudwl_malloc(IM_INT32 size)
{
	return malloc(size);
}

void dmmudwl_free(void *p)
{
	free(p);
}

IM_RET dmmudwl_enable(dmmudwl_t handle)
{
	dmmudwl_context_t *dwl = (dmmudwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), dwl->devid));
	if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DMMU_ENABLE, IM_NULL)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DMMU_ENABLE) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}

IM_RET dmmudwl_disable(dmmudwl_t handle)
{
	dmmudwl_context_t *dwl = (dmmudwl_context_t *)handle;
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), dwl->devid));
	if(ioctl(dwl->pmm_fd, PMMLIB_IOCTL_DMMU_DISABLE, IM_NULL)){
		IM_ERRMSG((IM_STR("ioctl(PMMLIB_IOCTL_DMMU_DISABLE) failed")));
		return IM_RET_FAILED;
	}
	return IM_RET_OK;
}



