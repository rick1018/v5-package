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

#include "IM_Audio_Timestamp.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_Trace.h"

//Initialize the parameters
static void setParameters(AudioDec_TimeStamp *time, OMX_U32 aFreq, OMX_U32 aSamples)
{
    if (0 != aFreq)
    {
        time->samplingFreq = aFreq;
    }

    time->samplesPerFrame = aSamples;
}


//Set the current timestamp equal to the input buffer timestamp
static void setFromInputTimestamp(AudioDec_TimeStamp *time, OMX_TICKS aValue)
{
    if(time->currentTimestamp != aValue) {
        time->currentTimestamp = aValue;
        time->currentSamples = 0;
    }
}

static void updateTimestamp(AudioDec_TimeStamp *time, OMX_U32 aValue)
{
    // rollover is not considered. Since samples are reset to 0
    // every time a new TS is applied to an output buffer - practically - rollover can't happen
    time->currentSamples += aValue;
}

/* Convert current samples into the output timestamp */
static OMX_TICKS getConvertedTimestamp(AudioDec_TimeStamp *time)
{
    OMX_TICKS Value = time->currentTimestamp;

	IM_OSAL_Info("value = %lld, time->samplingFreq = %d, time->currentSamples = %d", Value, time->samplingFreq, time->currentSamples);
    //Formula used: TS in OMX ticks = (samples * 10^6/sampling_freq)
    //Rounding added (add 0.5 to the result), extra check for divide by zero
    if (0 != time->samplingFreq)
    {
        Value = time->currentTimestamp + (((OMX_TICKS)time->currentSamples * 1000000 + (time->samplingFreq >> 1)) / time->samplingFreq);
    }
	
    //time->currentTimestamp = Value;
    //time->currentSamples = 0;

    return (Value);
}


/* Do not update iCurrentTs value, just calculate & return the current timestamp */
OMX_TICKS getCurrentTimestamp(AudioDec_TimeStamp *time)
{
    OMX_TICKS Value = time->currentTimestamp;

    if (0 != time->samplingFreq)
    {
        Value = time->currentTimestamp + (((OMX_TICKS)time->currentSamples * 1000000 + (time->samplingFreq >> 1)) / time->samplingFreq);
    }

    return (Value);
}

//Calculate the duration of single frame (in omx ticks)
OMX_TICKS getFrameDuration(AudioDec_TimeStamp *time)
{
    OMX_TICKS Value = 0;

    if (0 != time->samplingFreq)
    {
        Value = ((OMX_TICKS)time->samplesPerFrame * 1000000ll + (time->samplingFreq >> 1)) / time->samplingFreq;
    }

    return (Value);
}

int adec_timestamp_init(AudioDec_TimeStamp **time)
{
	AudioDec_TimeStamp *time_cal = (AudioDec_TimeStamp *)IM_OSAL_Malloc(sizeof(AudioDec_TimeStamp));
	if(time_cal == NULL) 
		return -1;

	IM_OSAL_Memset(time_cal, 0, sizeof(AudioDec_TimeStamp));

	time_cal->setParameters = setParameters;
	time_cal->setFromInputTimestamp = setFromInputTimestamp;
	time_cal->updateTimestamp = updateTimestamp;
	time_cal->getConvertedTimestamp = getConvertedTimestamp;
	time_cal->getCurrentTimestamp = getCurrentTimestamp;
	time_cal->getFrameDuration = getFrameDuration;

	*time = time_cal;
	return 0;
}

void adec_timestamp_deinit(AudioDec_TimeStamp *time)
{
	IM_OSAL_Free(time);
	
	return;
}
