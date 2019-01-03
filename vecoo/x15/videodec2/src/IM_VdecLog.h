/*
 * Copyright (c) 2012, InfoTM Microelectronics Co.,Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of  InfoTM Microelectronics Co. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANIMES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANIMES OF MERCHANTABILITY AND FITNESS FOR A PARIMCULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENIMAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSIMTUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPIMON) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file        IM_VdecLog.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/

#ifndef __IM_VDEC_LOG_H_
#define __IM_VDEC_LOG_H_

#if (TARGET_SYSTEM == FS_ANDROID)//def ANDROID
#include <cutils/log.h>
#ifndef  LOGD
#define LOGD  ALOGD
#endif
#endif
//DBG_LEVEL
//FATAL ERRORS 1
//WARNING 2
//GENERAL INFOMATION 3

#if (IM_VDEC_DBG_LEVEL > 2)
#define IM_VDEC_INFO(fmt,args...)     IM_VDEC_DBG("INFO:%s:%d: " fmt,__FUNCTION__, __LINE__ , ##args)
#else
#define IM_VDEC_INFO(fmt,args...) 
#endif

#if (IM_VDEC_DBG_LEVEL > 1)
#define IM_VDEC_WARN(fmt,args...)     IM_VDEC_DBG(__FILE__ ":WARNING:%s:%d: " fmt, __FUNCTION__,__LINE__ , ##args)
#else
#define IM_VDEC_WARN(fmt,args...) 
#endif

#if (IM_VDEC_DBG_LEVEL > 0)
#define IM_VDEC_ERR(fmt,args...)      IM_VDEC_DBG(__FILE__ ":ERROR:%s:%d: " fmt, __FUNCTION__,__LINE__ , ##args)
#else
#define IM_VDEC_ERR(fmt,args...)
#endif

#define IM_VDEC_ASSERT(a) if(!(a))IM_VDEC_ERR()
#ifdef IM_DEBUG
#if (TARGET_SYSTEM == FS_WINCE)
	#define IM_VDEC_DBG(str, args,...)      RETAILMSG(1, (TEXT(str), ##args))
#elif (TARGET_SYSTEM == FS_ANDROID)
	#define IM_VDEC_DBG(str,args...)      LOGD(str, ##args)
#elif (TARGET_SYSTEM == FS_LINUX)
	#define IM_VDEC_DBG(str, args...)     printf(str,##args);printf("\n");
#else
	#define IM_VDEC_DBG(str, args...)     
#endif
#else
	#define IM_VDEC_DBG(str, args...)     
#endif
#endif


#if (TARGET_SYSTEM == FS_ANDROID || TARGET_SYSTEM == FS_LINUX)
static int64_t GetNowUs()
{
	struct timeval tv_now;
	gettimeofday(&tv_now,NULL);
	return (int64_t)tv_now.tv_sec * 1000000 + tv_now.tv_usec;
}
#endif
