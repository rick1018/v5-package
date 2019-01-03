#ifndef IM_VIDEO_DEC_H
#define IM_VIDEO_DEC_H

#include "InfotmMedia.h"
#include "IM_vdeccommon.h"
#include "ppapi.h"
#include "IM_MultiBuffer.h"
#define MAX_SKIP_PIC_NUM 60


class IM_VideoDec
{
public:
	IM_VideoDec(){};
	virtual ~IM_VideoDec(){};

	// vdec init
	virtual IM_RET Init(IM_VDEC_SPEC_DATA *specData, IM_REF_FRM_FORMAT refFrmFormat = IM_DEC_REF_FRM_RASTER_SCAN) = 0;
	virtual IM_BOOL isSupported() = 0;

	virtual IM_RET GetConfig(IM_VDEC_CONFIG *vdecConfig);
	virtual IM_RET SetConfig(IM_VDEC_CONFIG *vdecConfig);
			IM_RET GetVdecInfo(IM_VDEC_INFO *vdecInfo);
			IM_RET SetMultiBuffers(IM_VDEC_BUFFERS *vdecBuffers);
	virtual IM_RET Decode(const IM_VDEC_IN *pDecIn, IM_VDEC_OUT *pDecOut, IM_VDEC_PICTURE *pVdecPicture) = 0;
	virtual IM_RET GetNextPicture(IM_VDEC_PICTURE *pOutPicture) = 0;
	virtual IM_RET Flush() = 0;
	virtual IM_RET DeInit() = 0;

protected :
	virtual IM_RET setPpCfg(IM_VDEC_PICTURE *vdecPicture);
	virtual IM_RET getDefaultConfig(IM_VDEC_CONFIG *vdecConfig);
	virtual IM_RET checkConfig(IM_VDEC_CONFIG *vdecConfig);
	virtual IM_RET checkInputType(IM_VDEC_INTYPE *inType);
	virtual IM_RET checkOutputType(IM_VDEC_OUTTYPE *outType);
	virtual IM_RET checkVdecMode(IM_VDEC_MODE *mode);
	virtual IM_RET checkMultiBuffer(IM_UINT32 multiBufferNum);
	IM_RET setMultiBuffers(IM_MultiBuffer *multiBuffer);

	IM_RET vdecMemCpy(void *dst, void *src, IM_UINT32 size);
	
	IM_VDEC_CONFIG mVdecConfig;
	IM_VDEC_CONFIG mSuggestConfig;
	IM_VDEC_IN   mVdecIn;
	IM_VDEC_OUT  mVdecOut;
	IM_VDEC_INFO mVdecInfo;
	IM_UINT32 mUseVideoFreezeConcealment;
	IM_UINT32 nExtraFrameBuffers;
	IM_INT32 mDirectShow;

	IM_UINT32 nCropWidth;
	IM_UINT32 nCropHeight;

	IM_UINT32 nDecodedPicSize;
	IM_UINT32 nSkippedPicAfterFlush;
	
		
	IM_MultiBuffer *pMultiBuffList;

	PPInst PpInst;
	PPConfig PpCfg;

	IM_BOOL bHeaderReady;
	IM_BOOL bNoInputSize;
	IM_BOOL bVdecInfoValid;
	IM_BOOL bPicInfoValid;
	IM_BOOL bRunTimeValid;
	IM_BOOL bPpCombineModeEnable;
	IM_BOOL bVdecConfigSetted;
	IM_BOOL bMultiBufferReset;
	IM_BOOL bFlushed;
	IM_BOOL bSwPp;
	IM_BOOL bOutputChanged;

#ifdef STATISTICAL_INFOMATION
	IM_UINT64 inFrameCount;
	IM_UINT64 outPicCount;
	IM_UINT32 skipPicCount;
	IM_UINT8  vdecType[128];
#endif 

};

#endif
