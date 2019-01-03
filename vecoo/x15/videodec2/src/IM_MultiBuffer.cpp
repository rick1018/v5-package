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
 * @file        IM_Multibuffer.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/

#include "IM_MultiBuffer.h"
#include "IM_VdecLog.h"

IM_MultiBuffer::IM_MultiBuffer(IM_INT32 multiBufferPpSize)
{
	queue = IM_NULL;
	IM_INT32 qSize = sizeof(IM_INT32) + multiBufferPpSize * sizeof(MULTIBUFF_ELEM);
	
	queue = (MULTIBUFF_ELEMS *)malloc(qSize);
	if(queue == NULL)
	{
		IM_VDEC_ERR("MALLOC multibuffer queue failed");
		return;
	}
	
	memset(queue, 0, qSize);
	currentSize = 0;
	builded = IM_FALSE;

	queue->maxElems = multiBufferPpSize;
}

IM_MultiBuffer::~IM_MultiBuffer()
{
	if(queue)
	{
		free(queue);
		queue = NULL;
	}
}

IM_RET  IM_MultiBuffer::buildMultiBufferList(IM_VDEC_BUFFERS *vdecBuffers)
{
	IM_VDEC_INFO("+++");
	if(queue == IM_NULL)
	{
		IM_VDEC_ERR("queue is null");
		return IM_RET_VDEC_NOT_INITIALIZED; 
	}
	if(vdecBuffers == NULL)
	{
		IM_VDEC_ERR("INVALID parameters");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(vdecBuffers->num != queue->maxElems)
	{
		IM_VDEC_ERR("multibuffer num error");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(builded)
	{	
		IM_VDEC_ERR("MultiBuffer List builed ");
		return IM_RET_VDEC_FAIL;
	}

	IM_VDEC_INFO("multibuffer num = %d", vdecBuffers->num);	
	for(IM_INT32 i = 0; i < vdecBuffers->num; i++)
	{
		memcpy(&queue->elem[i].buffer, &vdecBuffers->buffer[i], sizeof(IM_VDEC_BUFFER));
		IM_VDEC_INFO("BUFF[%d].virtAddr = %p, phyAddr = %x", i, queue->elem[i].buffer.virtAddr,queue->elem[i].buffer.busAddr);
		queue->elem[i].index = i;
		queue->elem[i].empty = IM_FALSE;
		currentSize++;
	}

	builded = IM_TRUE;
	IM_VDEC_INFO("---");
	return IM_RET_VDEC_OK;
}

IM_RET  IM_MultiBuffer::getElembyIndex(IM_INT32 index, MULTIBUFF_ELEM *elem)
{
	IM_VDEC_INFO("index = %d", index);
	if(queue == IM_NULL)
	{
		IM_VDEC_ERR("queue is null");
		return IM_RET_VDEC_NOT_INITIALIZED; 
	}

	if(elem == NULL || index >= queue->maxElems)
	{
		IM_VDEC_ERR("INVALID parameters");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(queue->elem[index].empty == IM_TRUE)
	{
		IM_VDEC_ERR("There is no spec elem, it is empty");
		return IM_RET_VDEC_FAIL;
	}

	memcpy((void *)elem, &queue->elem[index], sizeof(MULTIBUFF_ELEM));
	//queue->elem[index].empty = IM_TRUE;
	//currentSize--;
	return IM_RET_VDEC_OK;
}

IM_RET IM_MultiBuffer::getElembybusAddr(IM_UINT32 busAddr, MULTIBUFF_ELEM *elem)
{

	if(queue == IM_NULL)
	{
		IM_VDEC_ERR("queue is null");
		return IM_RET_VDEC_NOT_INITIALIZED; 
	}

	if(elem == NULL || busAddr == 0)
	{
		IM_VDEC_ERR("INVALID parameters");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	for(IM_INT32 i = 0; i < queue->maxElems; i++)
	{
		if(queue->elem[i].buffer.busAddr == busAddr)
		{
			if(queue->elem[i].empty == IM_TRUE)
			{
				IM_VDEC_ERR("There is no spec elem, it is empty");
				break;
			}

			memcpy((void *)elem, &queue->elem[i], sizeof(MULTIBUFF_ELEM));

			queue->elem[i].empty = IM_TRUE;
			IM_VDEC_INFO("index = %d", elem->index);
			currentSize--;
			return IM_RET_VDEC_OK; 
		}
	}

	IM_VDEC_ERR("not found the spec elem");
	return IM_RET_VDEC_FAIL;
}

IM_RET IM_MultiBuffer::fillElem(IM_INT32 index, MULTIBUFF_ELEM *elem)
{
	if(queue == IM_NULL)
	{
		IM_VDEC_ERR("queue is null");
		return IM_RET_VDEC_NOT_INITIALIZED; 
	}

	if(elem == NULL || index >= queue->maxElems)
	{
		IM_VDEC_ERR("INVALID parameters");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(queue->elem[index].empty == IM_FALSE)
	{
		IM_VDEC_ERR("the spec empty is not empty, do not fill");
		return IM_RET_VDEC_FAIL;
	}
	
	queue->elem[index].buffer = elem->buffer;
	IM_VDEC_INFO("index = %d", index);
	queue->elem[index].index = index;
	queue->elem[index].empty = IM_FALSE;
	currentSize++;

	return IM_RET_VDEC_OK;
}

IM_RET IM_MultiBuffer::release()
{
	if(builded == IM_FALSE)
	{
		return IM_RET_VDEC_OK;
	}

	for(IM_INT32 i = 0; i < queue->maxElems; i++)
	{
		memset(&queue->elem[i].buffer, 0, sizeof(IM_VDEC_BUFFER));
		queue->elem[i].index = i;
		queue->elem[i].empty = IM_TRUE;
	}
	currentSize = 0;
	builded = IM_FALSE;

	return IM_RET_VDEC_OK;
}

IM_INT32 IM_MultiBuffer::maxSize()
{
	if(queue == IM_NULL)
	{
		IM_VDEC_ERR("queue is null");
		return 0; 
	}
	return queue->maxElems;
}

IM_INT32 IM_MultiBuffer::curSize()
{
	IM_VDEC_INFO("currentSize = %d", currentSize);
	return  currentSize;
}
