/* ------------------------------------------------------------------
 * Copyright (C) 2008-2014 Infotmic Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
#ifndef RA_TIMESTAMP_H_INCLUDED
#define RA_TIMESTAMP_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "OMX_Types.h"
#define DEFAULT_SAMPLING_FREQ 44100
#define DEFAULT_SAMPLES_PER_FRAME 1024
typedef struct _AudioDec_Timestamp AudioDec_TimeStamp;

struct _AudioDec_Timestamp {
	OMX_U32 samplingFreq;
	OMX_TICKS currentTimestamp;
	OMX_U32 currentSamples;
	OMX_U32 samplesPerFrame;
	/* set pcm frequence and samples */
	void (*setParameters)(AudioDec_TimeStamp *time, OMX_U32 aFreq, OMX_U32 aSamples);

	/* set input timestamp */
	void (*setFromInputTimestamp)(AudioDec_TimeStamp *time, OMX_TICKS aValue);

	/* update timestamp */
    void (*updateTimestamp)(AudioDec_TimeStamp *time, OMX_U32 aValue);

	/* get converted timestamp */ 
    OMX_TICKS (*getConvertedTimestamp)(AudioDec_TimeStamp *time);

	/* get current timestamp */
    OMX_TICKS (*getCurrentTimestamp)(AudioDec_TimeStamp *time);

	/* get audio frame duration */
   OMX_TICKS (*getFrameDuration)(AudioDec_TimeStamp *time);
};

/* return 0 means OK */
int adec_timestamp_init(AudioDec_TimeStamp **time);

void adec_timestamp_deinit(AudioDec_TimeStamp *time);
#endif  //#ifndef RA_TIMESTAMP_H_INCLUDED
