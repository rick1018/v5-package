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
** v1.2.0	leo@2012/03/22: no modification except version.
** v1.2.1	leo@2012/04/05: update version number.
** v1.2.4	leo@2012/05/14: update version to v1.2.4.
** v1.2.5	leo@2012/06/12: update version to v1.2.5, in pmm_pwl_linux.c, 
**				support read reserved memory from system.
** v1.2.7	leo@2012/07/11: update version to v1.2.7.
** v1.2.12	leo@2012/07/11: update version to v1.2.12.
**
*****************************************************************************/ 

#ifndef __BUFFALLOC_H__
#define __BUFFALLOC_H__


#define VER_MAJOR	1
#define VER_MINOR	2
#define VER_PATCH	12
#define VER_STRING	"1.2.12"

#ifdef __cplusplus
class IM_BuffAlloc{
public:
	IM_BuffAlloc(IM_TCHAR *owner=IM_NULL);
	virtual ~IM_BuffAlloc();
	IM_RET open();
	IM_RET close();
	IM_RET allocBuffer(IN IM_UINT32 size, OUT IM_Buffer *buff, IM_INT32 flag=0);
	IM_RET freeBuffer(IN IM_Buffer *buff);

private:
	MMINST mMmInst;
	IM_List<IM_Buffer> mBuffList;
	IM_TCHAR mOwner[ALC_OWNER_LEN_MAX + 4];

	IM_RET allocVirBuffer(IM_UINT32 size, IM_Buffer *buff);
	IM_RET allocPhyBuffer(IM_UINT32 size, IM_Buffer *buff, IM_INT32 flag);
	IM_RET freeVirBuffer(IM_Buffer *buff);
	IM_RET freePhyBuffer(IM_Buffer *buff);	
};
#endif	// __cplusplus

#endif	// __BUFFALLOC_H__
