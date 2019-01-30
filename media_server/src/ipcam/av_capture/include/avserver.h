#ifndef _AV_SERVER_H_
#define _AV_SERVER_H_

#include "avserver_debug.h"
#include "avserver_config.h"
//#include <avserver_thr.h>
//#include <video.h>
//#include <audio.h>
//#include <spk.h> //RS,20091007
//#include <avserver_api.h>
//#include <videoSwosd.h>
//#include <ipnc_feature.h>


// from AVSERVER Main to other TSKs
#define AVSERVER_CMD_CREATE    (0x300)
#define AVSERVER_CMD_DELETE    (0x301)
#define AVSERVER_CMD_START     (0x302)
#define AVSERVER_CMD_NEW_DATA  (0x303)

// from UI thread to AVSERVER Main
#define AVSERVER_MAIN_CMD_START  (0x400)
#define AVSERVER_MAIN_CMD_STOP   (0x401)

// AVSERVER Main State's
#define AVSERVER_MAIN_STATE_IDLE       (0)
#define AVSERVER_MAIN_STATE_RUNNING    (1)

/*RS , VIDEO/IMAGE Quality ,20091113*/
#if 0
#define MPEG_VERY_HIGH	3
#define MPEG_HIGH	4
#define MPEG_NORMAL	5
#define MPEG_LOW	6
#define MPEG_VERY_LOW	7
#define JPEG_VERY_HIGH 60 
#define	JPEG_HIGH	50
#define JPEG_NORMAL	40
#define JPEG_LOW	30
#define JPEG_VERY_LOW 20
#endif
#define MOTION_WINDOW 3
#define ProfileShow 0
//TSK Status,RS,20091118
#define VIDEO_VS_THR 0
#if PRIVACY_MASK_FEATURE
#define VIDEO_FD_THR 1
#else
#define VIDEO_FD_THR 0
#endif
#define VIDEO_CRP_THR 0
#define VIDEO_IPIPE_HIS 0
#define TEST_FUNCTION 0            //RS,20091216
#define OSD_JPEG_SIZE (64*KB)  //RS,20091216
#define IMAGE_2A_MASK 0x3  //RS,20091231
#define Auto_Iris_Mask 0x4 //RS,20100317
#define GBCE 0 //RS,20100316 //byfrank
#define PRIVACY_MASK_ZONE 5 /* PRIVACY_MASK_FEATURE - Dean - 20130801 */

#define VIDEO_IVS_FEATURE			IVS_FEATURE // 1  [dean huang video ivs] - 20111205 - IVS_FEATURE

typedef struct {

  OSA_TskHndl   mainTsk;
  OSA_MbxHndl   uiMbx;
  OSA_MutexHndl lockMutex;
  
  OSA_PrfHndl capturePrf;
  OSA_PrfHndl ipipePrf;
  OSA_PrfHndl ldcPrf[4];
  OSA_PrfHndl resizePrf;  
  OSA_PrfHndl vsPrf;
  OSA_PrfHndl fdPrf;
  OSA_PrfHndl aewbPrf;
  OSA_PrfHndl afPrf;
  OSA_PrfHndl encodePrf[4];
  OSA_PrfHndl streamPrf;
  OSA_PrfHndl miscPrf[4];
  OSA_PrfHndl vnfPrf[4];
  OSA_PrfHndl swosdPrf[4];
  OSA_PrfHndl audioPrf;
  OSA_PrfHndl motionPrf;
  OSA_PrfHndl saldrePrf;

} AVSERVER_Ctrl;

extern AVSERVER_Ctrl gAVSERVER_ctrl;
extern AVSERVER_Config gAVSERVER_config;
extern AVSERVER_Config_Extend gAVSERVER_config_extend;		/* Add Extend Structure - Dean - 20130802 */
extern VIDEO_Ctrl gVIDEO_ctrl;
extern AUDIO_Ctrl gAUDIO_ctrl;
extern SPK_Ctrl gSPK_ctrl;//20091007,RS

OSA_BufInfo *AVSERVER_bufGetEmpty(int tskId, int streamId, int *bufId, int timeout);
OSA_BufInfo *AVSERVER_bufPutFull(int tskId, int streamId, int bufId);

OSA_BufInfo *AVSERVER_bufGetFull(int tskId, int streamId, int *bufId, int timeout);
OSA_BufInfo *AVSERVER_bufPutEmpty(int tskId, int streamId, int bufId);

Uint32 AVSERVER_getFrameSkipMask(int fps);

int AVSERVER_tskConnectInit();
int AVSERVER_tskConnectReset();
int AVSERVER_tskConnectExit();

int AVSERVER_bufAlloc();

int AVSERVER_mainCreate();
int AVSERVER_mainDelete();

int AVSERVER_getCaptureFrameRate(float *fr) ;
#endif  /*  _AV_SERVER_H_  */

