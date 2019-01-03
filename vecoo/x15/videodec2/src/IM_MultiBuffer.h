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
 * @file        IM_Multibuffer.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#ifndef __IM_MULTIBUFFER__
#define __IM_MULTIBUFFER__
#include "IM_vdeccommon.h"

typedef struct _QElem
{
    IM_VDEC_BUFFER  buffer;
    IM_INT32        index;
	IM_BOOL			empty;
} MULTIBUFF_ELEM;

typedef struct
{
	IM_INT32     maxElems;
	MULTIBUFF_ELEM elem[1];
} MULTIBUFF_ELEMS;


class IM_MultiBuffer
{
public:
	IM_MultiBuffer(IM_INT32 multiBufferPpSize);
	~IM_MultiBuffer();

	IM_RET buildMultiBufferList(IM_VDEC_BUFFERS *vdecBuffers);

	IM_RET getElembyIndex(IM_INT32 index, MULTIBUFF_ELEM *elem);

	IM_RET getElembybusAddr(IM_UINT32 busAddr, MULTIBUFF_ELEM *elem);
	
	IM_RET fillElem(IM_INT32 index, MULTIBUFF_ELEM *elem);

	IM_RET release();

	IM_INT32 maxSize();
	IM_INT32 curSize();

private:
	MULTIBUFF_ELEMS *queue;
	IM_INT32 currentSize;
	IM_BOOL builded;
};

#endif
