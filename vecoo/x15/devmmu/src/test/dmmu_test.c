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
#include <IM_devmmuapi.h>

#define DBGINFO		1
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"DMMUTST_I"
#define WARNHEAD	"DMMUTST_I"
#define ERRHEAD		"DMMUTST_I"
#define TIPHEAD		"DMMUTST_I"


int main(int argc, char *argv[])
{
	DMMUINST dmmu;
	IM_TCHAR verString[IM_VERSION_STRING_LEN_MAX + 1];

	dmmu_version(verString);
	IM_INFOMSG((IM_STR("InfotmMediaDevMMU version: %s"), verString));

	if(dmmu_init(&dmmu, DMMU_DEV_G2D) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_init() failed")));
		goto Fail;
	}

	if(dmmu_enable(dmmu) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_enable() failed")));
		goto Fail;
	}

	if(dmmu_disable(dmmu) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_disable() failed")));
		goto Fail;
	}

Fail:
	if(dmmu_deinit(dmmu) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_deinit() failed")));
	}
	return 0;
}



