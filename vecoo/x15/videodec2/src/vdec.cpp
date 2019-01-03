#include "IM_vdecapi2.h"
#include "IM_vdeccommon.h"
#include "IM_Mp4HwDec.h"
#include "IM_H264HwDec.h"
#include "IM_RvHwDec.h"
#include "IM_VC1HwDec.h"
#ifdef VDEC_LIB_G1
#include "IM_Vp6HwDec.h"
#include "IM_Vp8HwDec.h"
#include "IM_AvsHwDec.h"
#endif
#include "IM_MjpegHwDec.h"
#include "IM_Mpeg2HwDec.h"
#include "IM_SwFFmpegDec.h"
#include "IM_VdecLog.h"

#define VER_MAJOR       1
#define VER_MINOR       0
#define VER_PATCH       0
#define VER_STRING      "1.0.2"

IM_VideoDec *create_hw_decoder(IM_VDEC_INTYPE *inType)
{
	IM_VideoDec *vdec = NULL;
	switch(inType->codec)
	{
		case IM_STRM_VIDEO_H263:
		case IM_STRM_VIDEO_MPEG4:
		case IM_STRM_VIDEO_FLV1:
			vdec = new IM_Mp4HwDec(inType);
			break;
		case IM_STRM_VIDEO_H264:
			vdec =  new IM_H264HwDec(inType);
			break;
		case IM_STRM_VIDEO_RV30:
		case IM_STRM_VIDEO_RV40:
			vdec =  new IM_RvHwDec(inType);
			break;
		case IM_STRM_VIDEO_WMV3:
		case IM_STRM_VIDEO_VC1:
			vdec =  new IM_VC1HwDec(inType);
			break;
#ifdef VDEC_LIB_G1
		case IM_STRM_VIDEO_VP6:
		case IM_STRM_VIDEO_VP6F:
			vdec =  new IM_Vp6HwDec(inType);
			break;
		case IM_STRM_VIDEO_VP8:
			vdec =  new IM_Vp8HwDec(inType);
			break;
		case IM_STRM_VIDEO_AVS:
			vdec =  new IM_AvsHwDec(inType);
			break;
#endif
		case IM_STRM_VIDEO_MPEG1VIDEO:
		case IM_STRM_VIDEO_MPEG2VIDEO:
			vdec =  new IM_Mpeg2HwDec(inType);
			break;
		case IM_STRM_VIDEO_MJPEG:	
			vdec =  new IM_MjpegHwDec(inType);
			break;
		case IM_STRM_VIDEO_MSMPEG4V3:
		case IM_STRM_VIDEO_MSMPEG4V2:
		case IM_STRM_VIDEO_MSMPEG4V1:
		case IM_STRM_VIDEO_WMV1:
		case IM_STRM_VIDEO_WMV2:
		case IM_STRM_VIDEO_FFMPEG:
		default:
			break;
	}

	if(vdec && !vdec->isSupported())
	{
		delete vdec;
		vdec = NULL;
	}	

	return vdec;
}

IM_VideoDec *create_sw_decoder(IM_VDEC_INTYPE *inType)
{	
	IM_VideoDec *vdec = NULL;
	switch(inType->codec)
	{
		case IM_STRM_VIDEO_H263:
		case IM_STRM_VIDEO_MPEG4:
		case IM_STRM_VIDEO_FLV1:
		case IM_STRM_VIDEO_H264:
		case IM_STRM_VIDEO_RV30:
		case IM_STRM_VIDEO_RV40:
		case IM_STRM_VIDEO_WMV3:
		case IM_STRM_VIDEO_VC1:
		case IM_STRM_VIDEO_VP6:
		case IM_STRM_VIDEO_VP6F:
		case IM_STRM_VIDEO_VP8:
		case IM_STRM_VIDEO_MPEG1VIDEO:
		case IM_STRM_VIDEO_MPEG2VIDEO:
		case IM_STRM_VIDEO_MJPEG:	
		case IM_STRM_VIDEO_AVS:
		case IM_STRM_VIDEO_MSMPEG4V3:
		case IM_STRM_VIDEO_MSMPEG4V2:
		case IM_STRM_VIDEO_MSMPEG4V1:
		case IM_STRM_VIDEO_WMV1:
		case IM_STRM_VIDEO_WMV2:
		case IM_STRM_VIDEO_FFMPEG:
		default:
			vdec = new IM_SwFFmpegDec(inType);
			break;
	}

	if(vdec && !vdec->isSupported())
	{
		delete vdec;
		vdec = NULL;
	}	

	return vdec;
}

VDEC_API IM_UINT32 vdec_version(OUT IM_TCHAR *ver_string)
{
	if(ver_string)
	{
		IM_Strcpy(ver_string, (char *)VER_STRING);
		IM_VDEC_WARN("vdec version =%s", ver_string);
	}
	return IM_MAKE_VERSION(VER_MAJOR, VER_MINOR, VER_PATCH);
}

VDEC_API IM_RET vdec_create_ctx(INOUT IM_VDEC_CTX *vdecCtx, IN IM_VDEC_INTYPE *inType, IN IM_VDEC_CREATION_FLAG flag)
{
	IM_RET ret = IM_RET_VDEC_OK;
	IM_VideoDec *vdec = NULL;
	IM_BOOL bSwFailed = IM_FALSE;

	if((flag & kSoftwareCodecsOnly) || (flag & kPreferSoftwareCodecs))
	{
		vdec = create_sw_decoder(inType);
		if(vdec == NULL)
		{
			*vdecCtx = NULL;
			if(flag & kSoftwareCodecsOnly)
			{
				return IM_RET_VDEC_FAIL;
			}
			bSwFailed = IM_TRUE;
		}
		else
		{
			*vdecCtx = vdec;
			return IM_RET_VDEC_OK;
		}
	}

	if((flag & kHardwareCodecsOnly) || (flag & kPreferHardwareCodecs) || bSwFailed)
	{
		vdec = create_hw_decoder(inType);
		if(vdec == NULL)
		{
			*vdecCtx = NULL;
			if((flag & kHardwareCodecsOnly) || bSwFailed)
			{
				return IM_RET_VDEC_FAIL;
			}
		}
		else
		{
			*vdecCtx = vdec;
			return IM_RET_VDEC_OK;
		}
	}
		
	vdec = create_sw_decoder(inType);
	if(vdec == NULL)
	{
		*vdecCtx = NULL;
		return IM_RET_VDEC_FAIL;
	}
	else
	{
		*vdecCtx = vdec;
		return IM_RET_VDEC_OK;
	}
}


VDEC_API IM_RET vdec_get_config(IN IM_VDEC_CTX vdecCtx, OUT IM_VDEC_CONFIG *vdecConfig)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->GetConfig(vdecConfig);
}

VDEC_API IM_RET vdec_set_config(IN IM_VDEC_CTX vdecCtx,	IN IM_VDEC_CONFIG *vdecConfig)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->SetConfig(vdecConfig);
}

VDEC_API IM_RET vdec_init(IN IM_VDEC_CTX vdecCtx, IN IM_VDEC_SPEC_DATA *specData)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->Init(specData);
}

VDEC_API IM_RET vdec_decode(IN IM_VDEC_CTX vdecCtx, IN IM_VDEC_IN *vdecIn, OUT IM_VDEC_OUT *vdecOut, INOUT IM_VDEC_PICTURE *vdecPicture)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->Decode(vdecIn, vdecOut, vdecPicture);
}

VDEC_API IM_RET vdec_get_info(IM_VDEC_CTX vdecCtx, OUT IM_VDEC_INFO *vdecInfo)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->GetVdecInfo(vdecInfo);
}

VDEC_API IM_RET vdec_set_multibuffer(IN IM_VDEC_CTX vdecCtx, IN IM_VDEC_BUFFERS *vdecBuffers)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->SetMultiBuffers(vdecBuffers);
}

VDEC_API IM_RET vdec_get_next_picture(IM_VDEC_CTX vdecCtx, IM_VDEC_PICTURE *vdecPicture)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->GetNextPicture(vdecPicture);
}

VDEC_API IM_RET vdec_flush(IM_VDEC_CTX vdecCtx)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->Flush();
}

VDEC_API IM_RET vdec_deinit(IM_VDEC_CTX vdecCtx)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	return vdec->DeInit();
}

VDEC_API IM_RET vdec_destroy(IM_VDEC_CTX vdecCtx)
{
	IM_VideoDec *vdec = (IM_VideoDec *)vdecCtx;
	delete vdec;
	
	return IM_RET_VDEC_OK;
}
