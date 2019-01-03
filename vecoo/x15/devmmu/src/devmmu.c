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
/*#include <pmm_lib.h>*/
#include <dmmu_dwl.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"DMMU_I:"
#define WARNHEAD	"DMMU_W:"
#define ERRHEAD		"DMMU_E:"
#define TIPHEAD		"DMMU_T:"


//
#define VER_MAJOR	0
#define VER_MINOR	0
#define VER_PATCH	1
#define VER_STRING	"0.0.1"

//
typedef struct{
	dmmudwl_t	dwl;
	IM_UINT32	devid;
	IM_BOOL		enabled;
}dmmu_instance_t;


DEVMMU_API IM_UINT32 dmmu_version(IM_TCHAR *ver_string)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	if(ver_string){
		im_oswl_strcpy(ver_string, (IM_TCHAR *)IM_STR(VER_STRING));
	}
	return IM_MAKE_VERSION(VER_MAJOR, VER_MINOR, VER_PATCH);
}

DEVMMU_API IM_RET dmmu_init(OUT DMMUINST *pinst, IN IM_UINT32 devid)
{
	dmmu_instance_t *dmmu;
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));
	
	// check parameter.
	if((pinst == IM_NULL) || (devid >= DMMU_DEV_MAX)){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	// allocate instance.
	dmmu = (dmmu_instance_t *)dmmudwl_malloc(sizeof(dmmu_instance_t));
	if(dmmu == IM_NULL){
		IM_ERRMSG((IM_STR("malloc(dmmu) failed")));
		return IM_RET_FAILED;
	}
	
	dmmu->dwl = dmmudwl_init(devid);
	if(dmmu->dwl == IM_NULL){
		IM_ERRMSG((IM_STR("dmmudwl_init() failed")));
		goto Fail;
	}
	dmmu->devid = devid;
	dmmu->enabled = IM_FALSE;

	//
	*pinst = (DMMUINST)dmmu;

	return IM_RET_OK;
Fail:
	if(dmmu->dwl != IM_NULL){
		dmmudwl_deinit(dmmu->dwl);
	}

	return IM_RET_FAILED;	
}

DEVMMU_API IM_RET dmmu_deinit(IN DMMUINST inst)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	// check parameter.
	if(inst == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	dmmu_disable(inst);
	dmmudwl_deinit(((dmmu_instance_t *)inst)->dwl);
	dmmudwl_free((void *)inst);

	return IM_RET_OK;	
}

DEVMMU_API IM_RET dmmu_enable(IN DMMUINST inst)
{
	dmmu_instance_t *dmmu = (dmmu_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	// check parameter.
	if(dmmu == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//	
	if(dmmu->enabled == IM_FALSE){
		if(dmmudwl_enable(dmmu->dwl) != IM_RET_OK){
			IM_ERRMSG((IM_STR("dmmudwl_enable() failed")));
			return IM_RET_FAILED;
		}
		dmmu->enabled = IM_TRUE;
	}

	return IM_RET_OK;	
}

DEVMMU_API IM_RET dmmu_disable(IN DMMUINST inst)
{
	dmmu_instance_t *dmmu = (dmmu_instance_t *)inst;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	// check parameter.
	if(dmmu == IM_NULL){
		IM_ERRMSG((IM_STR("invalid parameter")));
		return IM_RET_INVALID_PARAMETER;
	}

	//	
	if(dmmu->enabled == IM_TRUE){
		if(dmmudwl_disable(dmmu->dwl) != IM_RET_OK){
			IM_ERRMSG((IM_STR("dmmudwl_disable() failed")));
			return IM_RET_FAILED;
		}
		dmmu->enabled = IM_FALSE;
	}

	return IM_RET_OK;	
}


