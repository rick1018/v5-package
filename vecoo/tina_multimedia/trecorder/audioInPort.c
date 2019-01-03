#include"audioInPort.h"

aInPort *CreateAlsaAport(int index)
{
	aInPort *aport = (aInPort *)malloc(sizeof(aInPort));

	if(aport == NULL)
		return NULL;
	if(index == 0)
	{
		aport->readData = alsaAportReadData;
		aport->clear = alsaAportClear;
	}
	else
	{
		aport->readData = alsaAportReadData2;
		aport->clear = alsaAportClear2;
	}

	aport->getEnable = alsaAportGetEnable;
	aport->getFormat = alsaAportGetFormat;
	aport->getAudioSampleRate = alsaAportGetSamplerate;
	aport->getAudioChannels = alsaAportGetAudioChannels;
	aport->getAudioBitRate = alsaAportGetAudioBitrate;
	aport->init = alsaAportInit;
	aport->getpts = alsaAportgetPts;
	aport->deinit = alsaAportDeinit;

	aport->setEnable = alsaAportSetEnable;
	aport->setFormat = alsaAportSetFormat;
	aport->setAudioSampleRate = alsaAportSetSamplerate;
	aport->setAudioChannels = alsaAportSetAudioChannels;
	aport->setAudioBitRate = alsaAportSetAudioBitrate;
	//aport->setPts = alsaAportSetPts;
	//aport.setEnable(&aport,1);
	//aport.setFormat(&aport,TR_AUDIO_PCM);
	//aport.setAudioSampleRate(&aport,44100);
	//aport.setAudioChannels(&aport,2);
	//aport.setAudioBitRate(&aport,8000);
	return aport;
}


int DestroyAlsaAport(aInPort *hdl)
{
	if(hdl != NULL)
		free(hdl);
}
