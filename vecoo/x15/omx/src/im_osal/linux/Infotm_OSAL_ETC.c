
#include <stdio.h>
#include <st/*
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
 * AND ANY EXPRESS OR IMPLIED WARRANInfotmES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANInfotmES OF MERCHANTABILITY AND FITNESS FOR A PARInfotmCULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENInfotmAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSInfotmTUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPInfotmON) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * @file        Infotm_OSAL_ETC.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2010.03.01 : Create
 */
dlib.h>
#include <string.h>

#include "Infotm_OSAL_Memory.h"
#include "Infotm_OSAL_ETC.h"


#ifndef HAVE_GETLINE
ssize_t getline(char **ppLine, size_t *pLen, FILE *pStream)
{
    char *pCurrentPointer = NULL;
    size_t const chunk = 512;

    size_t defaultBufferSize = chunk + 1;
    size_t retSize = 0;

    if (*ppLine == NULL) {
        *ppLine = (char *)malloc(defaultBufferSize);
        if (*ppLine == NULL) {
            retSize = -1;
            goto EXIT;
        }
        *pLen = defaultBufferSize;
    }
    else {
        if (*pLen < defaultBufferSize) {
            *ppLine = (char *)realloc(*ppLine, defaultBufferSize);
            if (*ppLine == NULL) {
                retSize = -1;
                goto EXIT;
            }
            *pLen = defaultBufferSize;
        }
    }

    while (1) {
        size_t i;
        size_t j = 0;
        size_t readByte = 0;

        pCurrentPointer = *ppLine + readByte;

        i = fread(pCurrentPointer, 1, chunk, pStream);
        if (i < chunk && ferror(pStream)) {
            retSize = -1;
            goto EXIT;
        }
        while (j < i) {
            ++j;
            if (*pCurrentPointer++ == (char)'\n') {
                *pCurrentPointer = '\0';
                if (j != i) {
                    if (fseek(pStream, j - i, SEEK_CUR)) {
                        retSize = -1;
                        goto EXIT;
                }
                    if (feof(pStream))
                        clearerr(pStream);
                }
                readByte += j;
                retSize = readByte;
                goto EXIT;
            }
        }

        readByte += j;
        if (feof(pStream)) {
            if (readByte) {
                retSize = readByte;
                goto EXIT;
            }
            if (!i) {
                retSize = -1;
                goto EXIT;
            }
        }

        i = ((readByte + (chunk * 2)) / chunk) * chunk;
        if (i != *pLen) {
            *ppLine = (char *)realloc(*ppLine, i);
            if (*ppLine == NULL) {
                retSize = -1;
                goto EXIT;
        }
            *pLen = i;
        }
    }

EXIT:
    return retSize;
}
#endif /* HAVE_GETLINE */

OMX_PTR Infotm_OSAL_Strcpy(OMX_PTR dest, OMX_PTR src)
{
    return strcpy(dest, src);
}

OMX_PTR Infotm_OSAL_Strncpy(OMX_PTR dest, OMX_PTR src, size_t num)
{
    return strncpy(dest, src, num);
}

OMX_S32 Infotm_OSAL_Strcmp(OMX_PTR str1, OMX_PTR str2)
{
    return strcmp(str1, str2);
}

OMX_PTR Infotm_OSAL_Strcat(OMX_PTR dest, OMX_PTR src)
{
    return strcat(dest, src);
}

size_t Infotm_OSAL_Strlen(const char *str)
{
    return strlen(str);
}
