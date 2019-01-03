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

#ifndef __DMMU_DWL_H__
#define __DMMU_DWL_H__


typedef void * dmmudwl_t;

dmmudwl_t dmmudwl_init(IM_INT32 devid);
IM_RET dmmudwl_deinit(dmmudwl_t handle);
IM_RET dmmudwl_enable(dmmudwl_t handle);
IM_RET dmmudwl_disable(dmmudwl_t handle);

void * dmmudwl_malloc(IM_INT32 size);
void dmmudwl_free(void *p);

#endif	// __DMMU_DWL_H__

