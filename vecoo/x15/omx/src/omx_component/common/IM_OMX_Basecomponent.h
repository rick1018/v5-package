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
 * @file       IM_OMX_Basecomponent.h
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#ifndef IM_OMX_BASECOMP
#define IM_OMX_BASECOMP

#include "IM_OMX_Def.h"
#include "OMX_Component.h"
#include "IM_OSAL_Queue.h"
#include "IM_OMX_Baseport.h"


typedef struct _IM_OMX_MESSAGE
{
    OMX_U32 messageType;
    OMX_U32 messageParam;
    OMX_PTR pCmdData;
} IM_OMX_MESSAGE;

typedef struct _IM_OMX_DATABUFFER
{
    OMX_HANDLETYPE        bufferMutex;
    OMX_BUFFERHEADERTYPE* bufferHeader;
    OMX_BOOL              dataValid;
	OMX_BOOL			  bDataProcessed;
    OMX_U32               nRemainDataLeft;
    OMX_U32               phyAddr; 
	OMX_PTR               virAddr;
    OMX_U32               dataSize;

	OMX_BOOL 			  bHardwareUsed;
} IM_OMX_DATABUFFER;

/* for Check TimeStamp after Seek */
typedef struct _IM_OMX_TIMESTAPM
{
    OMX_BOOL  needSetStartTimeStamp;
    OMX_BOOL  needCheckStartTimeStamp;
    OMX_TICKS startTimeStamp;
    OMX_U32   nStartFlags;
} IM_OMX_TIMESTAMP;

typedef struct _IM_OMX_BASECOMPONENT
{
    OMX_STRING               componentName;
    OMX_VERSIONTYPE          componentVersion;
    OMX_VERSIONTYPE          specVersion;

    OMX_STATETYPE            currentState;
    OMX_STATETYPE            targetState;
    IM_OMX_TRANS_STATETYPE  transientState;

    IM_CODEC_TYPE           codecType;
    IM_OMX_PRIORITYMGMTTYPE compPriority;
    OMX_MARKTYPE             propagateMarkType;
    OMX_HANDLETYPE           compMutex;

    OMX_HANDLETYPE           hCodecHandle;
    OMX_HANDLETYPE           hComponentHandle;

    /* Message Handler */
    OMX_BOOL                 bExitMessageHandlerThread;
    OMX_HANDLETYPE           hMessageHandler;
    OMX_HANDLETYPE           msgSemaphoreHandle;
    IM_QUEUE                messageQ;

    /* Buffer Process */
    OMX_BOOL                 bExitBufferProcessThread;
    OMX_HANDLETYPE           hBufferProcess;

    /* Buffer */
    IM_OMX_DATABUFFER       IMDataBuffer[2];

    /* Port */
    OMX_PORT_PARAM_TYPE      portParam;
    IM_OMX_BASEPORT        *pIMPort;

    OMX_HANDLETYPE           pauseEvent;

    /* Callback function */
    OMX_CALLBACKTYPE        *pCallbacks;
    OMX_PTR                  callbackData;

    /* Save Timestamp */
    OMX_TICKS                timeStamp[MAX_TIMESTAMP];
    IM_OMX_TIMESTAMP        checkTimeStamp;

    /* Save Flags */
    OMX_U32                  nFlags[MAX_FLAGS];

    OMX_BOOL                 getAllDelayBuffer;
    OMX_BOOL                 remainOutputData;
    OMX_BOOL                 reInputData;
	OMX_BOOL				 useMMU;

    /* Android CapabilityFlags */
    OMXComponentCapabilityFlagsType capabilityFlags;

    OMX_BOOL bUseFlagEOF;
    OMX_BOOL bSaveFlagEOS;
	OMX_BOOL bFlushBeforeDecoding;
	OMX_BOOL bSmallMemoryDevice;

    OMX_ERRORTYPE (*IM_componentInit)(OMX_COMPONENTTYPE *pOMXComponent);
    OMX_ERRORTYPE (*IM_componentTerminate)(OMX_COMPONENTTYPE *pOMXComponent);
    OMX_ERRORTYPE (*IM_checkTransitions)(OMX_COMPONENTTYPE *pOMXComponent);
    

	OMX_ERRORTYPE (*IM_SetPortState)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex, OMX_STATETYPE state);
	OMX_ERRORTYPE (*IM_SetPortFlush)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex);
	OMX_ERRORTYPE (*IM_SetPortEnable)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex);
	OMX_ERRORTYPE (*IM_SetPortDisable)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex);
    OMX_ERRORTYPE (*IM_DecoderFlush) (OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 nPortIndex);

	OMX_ERRORTYPE (*IM_PortReset)(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex);
    OMX_ERRORTYPE (*IM_bufferProcess) (OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputUseBuffer, IM_OMX_DATABUFFER *pOutputUseBuffer);

    OMX_ERRORTYPE (*IM_AllocateTunnelBuffer)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex);
    OMX_ERRORTYPE (*IM_FreeTunnelBuffer)(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex);
    OMX_ERRORTYPE (*IM_BufferProcess)(OMX_HANDLETYPE hComponent);
    OMX_ERRORTYPE (*IM_BufferReset)(OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 nPortIndex);
    OMX_ERRORTYPE (*IM_InputBufferReturn)(OMX_COMPONENTTYPE *pOMXComponent);
    OMX_ERRORTYPE (*IM_OutputBufferReturn)(OMX_COMPONENTTYPE *pOMXComponent);

} IM_OMX_BASECOMPONENT;


#ifdef __cplusplus
extern "C" {
#endif

    OMX_ERRORTYPE IM_OMX_Check_SizeVersion(OMX_PTR header, OMX_U32 size);
	OMX_ERRORTYPE IM_OMX_GetConfig(
    	OMX_IN OMX_HANDLETYPE hComponent,
    	OMX_IN OMX_INDEXTYPE  nIndex,
    	OMX_INOUT OMX_PTR     pComponentConfigStructure);

	OMX_ERRORTYPE IM_OMX_SetConfig(
    	OMX_IN OMX_HANDLETYPE hComponent,
    	OMX_IN OMX_INDEXTYPE  nIndex,
    	OMX_IN OMX_PTR        pComponentConfigStructure);
	OMX_ERRORTYPE IM_OMX_GetExtensionIndex(
    	OMX_IN OMX_HANDLETYPE  hComponent,
    	OMX_IN OMX_STRING      cParameterName,
    	OMX_OUT OMX_INDEXTYPE *pIndexType);
	OMX_ERRORTYPE IM_OMX_GetParameter(
    	OMX_IN OMX_HANDLETYPE hComponent,
    	OMX_IN OMX_INDEXTYPE  nParamIndex,
    	OMX_INOUT OMX_PTR     ComponentParameterStructure);
	OMX_ERRORTYPE IM_OMX_SetParameter(
    	OMX_IN OMX_HANDLETYPE hComponent,
    	OMX_IN OMX_INDEXTYPE  nIndex,
    	OMX_IN OMX_PTR        ComponentParameterStructure);
	OMX_ERRORTYPE IM_OMX_BaseComponent_Constructor(
    	OMX_IN OMX_HANDLETYPE hComponent);
	
	OMX_ERRORTYPE IM_OMX_BaseComponent_Destructor(
    	OMX_IN OMX_HANDLETYPE hComponent);

    OMX_ERRORTYPE IM_OMX_CommandQueue(
        IM_OMX_BASECOMPONENT *pIMComponent,
        OMX_COMMANDTYPE        Cmd,
        OMX_U32                nParam,
        OMX_PTR                pCmdData);


#ifdef __cplusplus
};
#endif

#endif
