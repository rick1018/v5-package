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
** v1.0.1	leo@2012/03/15: first commit.
** v1.1.0	leo@2012/03/16: no modification except version.
** v1.2.0	leo@2012/03/22: compiler ok for android4.0 & linux3.0.
** v1.3.0	leo@2012/04/05: update version number.
** v1.4.0	leo@2012/08/15: update version number.
** v2.0.1   leo@2012/08/22: update version to v2.0.1.
** v2.0.2   leo@2012/08/22: update version to v2.0.2.
**
*****************************************************************************/ 


#ifndef __INFOTMMEDIA_H__
#define __INFOTMMEDIA_H__

//
// system defination.
//
#define FS_ANDROID		(0x1)
#define FS_WINCE		(0x2)
#define FS_LINUX		(0x3)
#define RVDS			(0x4)
#define KERNEL_LINUX	(0x1001)
#define KERNEL_WINCE	(0x1002)

// all of InfotmMedia related headers.
#include <IM_types.h>
#include <IM_common.h>
#include <IM_dbgmsg.h>
#include <IM_format.h>

#ifdef __cplusplus
	#include <IM_mempool.h>
	#include <IM_list.h>
	#include <IM_argparse.h>
#else
	#include <IM_mempool_c.h>
	#include <IM_list_c.h>
	#include <IM_argparse_c.h>
#endif

#if (TARGET_SYSTEM != KERNEL_LINUX) && (TARGET_SYSTEM != KERNEL_WINCE)
#ifdef __cplusplus 
	#include <IM_oswl.h>
	#include <IM_looper.h>
	#include <IM_kvpair.h>
#else
	#include <IM_oswl_c.h>
	#include <IM_looper_c.h>
	#include <IM_kvpair_c.h>
#endif
#endif


#define FOUNDATIONS_VER_MAJOR	2
#define FOUNDATIONS_VER_MINOR	0
#define FOUNDATIONS_VER_PATCH	2
#define FOUNDATIONS_VER_STRING	IM_STR("2.0.2")

#endif	// __INFOTMMEDIA_H__

