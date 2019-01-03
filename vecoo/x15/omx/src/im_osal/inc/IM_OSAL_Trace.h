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
 * *  Neither the name of InfoTM Microelectronics Co. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * @file        IM_OSAL_Trace.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
 */



#ifndef _IM_OSAL_TRACES_H_
#define _IM_OSAL_TRACES_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

/*******************************************************************************
* Traces
*******************************************************************************/


/******************************************************************************
* Debug Trace defines
******************************************************************************/

	typedef enum IM_OSAL_TRACEGRP_TYPE
	{
		IM_OSAL_TRACEGRP_SYSTEM = 1,
		IM_OSAL_TRACEGRP_OMXBASE = (1 << 1),
		IM_OSAL_TRACEGRP_DOMX = (1 << 2),
		IM_OSAL_TRACEGRP_OMXVIDEOENC = (1 << 3),
		IM_OSAL_TRACEGRP_OMXVIDEODEC = (1 << 4),
		IM_OSAL_TRACEGRP_OMXCAM = (1 << 5),
		IM_OSAL_TRACEGRP_OMXIMGDEC = (1 << 6),
		IM_OSAL_TRACEGRP_DRIVERS = (1 << 7),
		IM_OSAL_TRACEGRP_SIMCOPALGOS = (1 << 8)
	} IM_OSAL_TRACEGRP;


/**
* The OSAL debug trace level can be set at runtime by defining the environment
* variable IM_OSAL_DEBUG_TRACE_LEVEL=<Level>.  The default level is 1
* The debug levels are:
* Level 0 - No trace
* Level 1 - Error   [Errors]
* Level 2 - Warning [Warnings that are useful to know about]
* Level 3 - Info    [General information]
* Level 4 - Debug   [most-commonly used statement for us developers]
* Level 5 - Trace   ["ENTERING <function>" and "EXITING <function>" statements]
*
* Example: if IM_OSAL_DEBUG_TRACE_LEVEL=3, then level 1,2 and 3 traces messages
* are enabled.
*/

/**
 * Information about the trace location/type, passed as a single pointer to
 * internal trace function.  Not part of the public API
 */
	typedef struct
	{
		const char *file;
		const char *function;
		const int line;
		const short level;
		const short tracegrp;	/* IM_OSAL_TRACEGRP */
	} __IM_OSAL_TRACE_LOCATION;

/**
 * Trace implementation function.  Not part of public API.  Default
 * implementation uses printf(), but you can use LD_PRELOAD to plug in
 * alternative trace system at runtime.
 */
	void __IM_OSAL_TraceFunction(const __IM_OSAL_TRACE_LOCATION * loc,
	    const char *fmt, ...);

/** 
 *check trace level  and set
 */
void IM_OSAL_TraceLevelSet();


/**
 * Internal trace macro.  Not part of public API.
 */
#define __IM_OSAL_Trace(level, tracegrp, fmt, ...)                          \
    do {                                                                      \
        static const __IM_OSAL_TRACE_LOCATION loc = {                       \
                __FILE__, __FUNCTION__, __LINE__, (level), (tracegrp)         \
        };                                                                    \
        __IM_OSAL_TraceFunction(&loc, fmt"\n", ##__VA_ARGS__);              \
    } while(0)

/**
* IM_OSAL_Error() -- Fatal errors
*/
#define IM_OSAL_Error(fmt,...)  IM_OSAL_ErrorExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)

/**
* IM_OSAL_Warning() -- Warnings that are useful to know about
*/
#define IM_OSAL_Warning(fmt,...)  IM_OSAL_WarningExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)

/**
* IM_OSAL_Info() -- general information
*/
#define IM_OSAL_Info(fmt,...)  IM_OSAL_InfoExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)

/**
* IM_OSAL_Debug() -- debug traces, most-commonly useful for developers
*/
#define IM_OSAL_Debug(fmt,...)  IM_OSAL_DebugExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)

/**
* IM_OSAL_Entering() -- "ENTERING <function>" statements
* IM_OSAL_Exiting()  -- "EXITING <function>" statements
*/
#define IM_OSAL_Entering(fmt,...)  IM_OSAL_EnteringExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)
#define IM_OSAL_Exiting(fmt,...)  IM_OSAL_ExitingExt(IM_OSAL_TRACEGRP_SYSTEM, fmt, ##__VA_ARGS__)

/*******************************************************************************
** New Trace to be used by Applications
*******************************************************************************/

/**
* IM_OSAL_ErrorExt() -- Fatal errors
*/
#define IM_OSAL_ErrorExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(1, tracegrp, "ERROR: "fmt, ##__VA_ARGS__)

/**
* IM_OSAL_WarningExt() -- Warnings that are useful to know about
*/
#define IM_OSAL_WarningExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(2, tracegrp, "WARNING: "fmt, ##__VA_ARGS__)

/**
* IM_OSAL_InfoExt() -- general information
*/
#define IM_OSAL_InfoExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(3, tracegrp, "INFO: "fmt, ##__VA_ARGS__)

/**
* IM_OSAL_DebugExt() -- most-commonly used statement for us developers
*/
#define IM_OSAL_DebugExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(4, tracegrp, "TRACE: "fmt, ##__VA_ARGS__)

/**
* IM_OSAL_EnteringExt() -- "ENTERING <function>" statements
* IM_OSAL_ExitingExt()  -- "EXITING <function>" statements
*/
#define IM_OSAL_EnteringExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(5, tracegrp, "ENTER: "fmt, ##__VA_ARGS__)
#define IM_OSAL_ExitingExt(tracegrp, fmt, ...)  __IM_OSAL_Trace(5, tracegrp, "EXIT: "fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* _IM_OSAL_TRACES_H_ */
