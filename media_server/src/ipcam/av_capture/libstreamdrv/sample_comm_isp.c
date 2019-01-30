/******************************************************************************
  Some simple Hisilicon HI3516 isp functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"

static pthread_t gs_IspPid;
HI_S32 awb_isp = WB_AUTO;

#ifdef ISP_V1
static HI_U32 u32IspExtBase = 0;

#define  MAX_STRLEN		128
#define  MAX_REGLEN		32
#define  REAL_REGLEN	strlen("0x00000000:0x00000000")
#define ISP_REG_SIZE_CFG	0x1014
#define ISP_EXT_SIZE_CFG	0x0264

#define CHECKREGADDR(regaddr)		\
do{\
	if ((regaddr < (unsigned int)ISP_REG_BASE)){return HI_FAILURE;}\
	else if(regaddr > (unsigned int)(u32IspExtBase + ISP_EXT_SIZE_CFG)){return HI_FAILURE;}\
	else if ((regaddr > (unsigned int)(ISP_REG_BASE + ISP_REG_SIZE_CFG)) && (regaddr < (unsigned int)u32IspExtBase) ){return HI_FAILURE;}\
	else{;};\
}while(0)


HI_VOID SAMPLE_COMM_ISP_CircleWriteReg(HI_U32 u32RegAddr,  FILE* fd)
{
	HI_S32 s32Ret = 0;
	HI_CHAR acRegFmt[MAX_REGLEN];
	HI_U32 u32RegVal = 0;

	s32Ret = HI_MPI_ISP_GetCfg(u32RegAddr, &u32RegVal);
	if (s32Ret != HI_SUCCESS)
	{
		printf("HI_MPI_ISP_SetCfg ERR(%x) Reg%x: 0x%x\n", s32Ret, u32RegAddr, u32RegVal);
	}

	snprintf(acRegFmt, sizeof(acRegFmt), "0x%08x:0x%08x\r\n", u32RegAddr, u32RegVal);
	if (1 != fwrite(acRegFmt, strlen(acRegFmt), 1, fd))
	{
		printf("fwrite ERR regaddr %8x : regval%8x \n", u32RegAddr, u32RegVal);
		return ;
	}
	return ;

}

HI_S32  SAMPLE_COMM_ISP_GetLine(HI_CHAR * pcSrcBuf,HI_S32 s32SrcLen)
{
    HI_S32 i = 0;
    HI_S32 s32LineLen = 0;

    if((pcSrcBuf == NULL)||(s32SrcLen <= 0))
    {
        printf("the buf is nul or len is invalid:%d\n",s32SrcLen);
        return -1;
    }

	i = 0;
	while(i < s32SrcLen)
    {
        if((pcSrcBuf[i] != '\r')&&(pcSrcBuf[i] != '\n'))
        {
            i++;
            s32LineLen ++;
        }
        else if(pcSrcBuf[i] == '\r')
        {
            i++;
            s32LineLen ++;
            if(pcSrcBuf[i] == '\n')
            {
                i++;
                s32LineLen ++;

            }
            break;
        }
        else /*pcSrcBuf[i] == '\n'**/
        {
            i++;
            s32LineLen ++;
            break;
        }
    }
    return s32LineLen;

}

/******************************************************************************
* funciton : isp register to cfg file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Reg2File(HI_CHAR* pcCfgFile)
{
    FILE* fdcfg = NULL;
    HI_CHAR* pstr = NULL;
    HI_U32 u32RegVal = 0;
    HI_U32 u32RegAddr = 0;
    HI_CHAR strReg[9];
    HI_S32 s32Ret = 0;
    HI_S32 s32NewFlag = 0;
    HI_CHAR RegFmt[18];
    HI_S32 i = 0;
    HI_S32 s32FileLen = 0;
    struct stat filestat;
    HI_CHAR* pbuf = NULL;

    if ((pcCfgFile == NULL) || (strlen(pcCfgFile) == 0))
    {
        printf("%s: Invalid Filename \n", __FUNCTION__);
        return HI_FAILURE;
    }

    printf("pcCfgFile is %s!\n", pcCfgFile);

    if (0 == u32IspExtBase)
    {
		if (HI_MPI_ISP_GetExtRegAddr(&u32IspExtBase))
		{
		    printf("Get ISP extent address failed!\n");
		}
		else
		{
		    printf("Get ISP extent address: %#x\n", u32IspExtBase);
		}
    }

    if (access(pcCfgFile, F_OK) == 0)
    {
        if (stat(pcCfgFile, &filestat) != 0)
        {
            printf("%s: stat err!\n", __FUNCTION__);
            return HI_FAILURE;
        }
        fdcfg = fopen(pcCfgFile, "r+");
        s32NewFlag = 0;
        printf("isp configure file exist !\n");
    }
    else
    {
        fdcfg = fopen(pcCfgFile, "w+");
        s32NewFlag = 1;
        printf("create new isp configure file!!\n");
    }

    if (s32NewFlag)
    {
        u32RegAddr = ISP_REG_BASE;
        for (i = 0; i < (int)(ISP_REG_SIZE_CFG); i+=4)
        {
            SAMPLE_COMM_ISP_CircleWriteReg(u32RegAddr, fdcfg);
            u32RegAddr +=4;
        }

        u32RegAddr = u32IspExtBase;
        for (i = 0; i <= (int)ISP_EXT_SIZE_CFG; i+=4)
        {
            SAMPLE_COMM_ISP_CircleWriteReg(u32RegAddr, fdcfg);
            u32RegAddr +=4;
        }

        fclose(fdcfg);
        chmod(pcCfgFile, 0666);
        return HI_SUCCESS;
    }

    pbuf = (char*)malloc(filestat.st_size);
    if (pbuf == NULL)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    if (fread(pbuf, filestat.st_size, 1, fdcfg) != 1)
    {
        printf("%s: fread failed!\n", __FUNCTION__);
        free(pbuf);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    s32FileLen = 0;
    fseek(fdcfg, 0, SEEK_SET);
    while(s32FileLen < filestat.st_size)
    {
        int s32RdLen = 0;
        char  curbuf[MAX_STRLEN];

        memcpy(curbuf, pbuf+s32FileLen, MAX_STRLEN);
        s32RdLen = SAMPLE_COMM_ISP_GetLine(curbuf, MAX_STRLEN);
        if(s32RdLen<= 0)
        {
            printf("%s: the file is not the register value!\n", __FUNCTION__);
            free(pbuf);
            return HI_FAILURE;
        }
        curbuf[s32RdLen] ='\0';
        s32FileLen +=s32RdLen;

        if (curbuf[0] == '#')
        {
            fwrite(curbuf, s32RdLen, 1, fdcfg);
            continue;
        }
        else if ((pstr = strchr(curbuf, ':')) != NULL )
        {
            strncpy(strReg, pstr+1, sizeof(strReg));
            strReg[9] = '\0';

            u32RegVal = strtoll(strReg, NULL, 16);

            u32RegAddr = strtoll(curbuf, NULL, 16);

            CHECKREGADDR(u32RegAddr);

            s32Ret = HI_MPI_ISP_GetCfg(u32RegAddr, &u32RegVal);
            if (s32Ret != HI_SUCCESS)
            {
                printf("%s: HI_MPI_ISP_SetCfg Reg%x: 0x%x failed with %#x!\n", __FUNCTION__, u32RegAddr, u32RegVal, s32Ret);
                goto END;
            }

            snprintf(RegFmt, sizeof(RegFmt), "0x%08x:0x%08x", u32RegAddr, u32RegVal);

            memcpy(&curbuf[0], RegFmt, strlen(RegFmt));
            s32Ret = fwrite(curbuf, s32RdLen, 1, fdcfg);
            if (1 != s32Ret)
            {
                printf("%s: fwrite err \n", __FUNCTION__);
                goto END;
            }
            continue;
        }
        else
        {
            fwrite(curbuf, s32RdLen, 1, fdcfg);
            continue;
        }
    }
    s32Ret = HI_SUCCESS;
END:
    free(pbuf);
    fclose(fdcfg);
    return s32Ret;
}

/******************************************************************************
* funciton : isp cfg file to register
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_File2Reg(HI_CHAR* pCfgFile)
{
    FILE* fdcfg = NULL;
    HI_CHAR* pstr = NULL;
    HI_U32 u32RegVal = 0;
    HI_U32 u32RegAddr = 0;
    HI_CHAR strReg[REAL_REGLEN+1];
    HI_S32 s32Ret = 0;
    HI_S32 s32FileLen = 0;
    struct stat filestat;
    HI_CHAR* pbuf = NULL;

    if ((pCfgFile == NULL) || (strlen(pCfgFile) == 0))
    {
        printf("%s: Invalid Filename!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    if (0 == u32IspExtBase)
    {
		if (HI_MPI_ISP_GetExtRegAddr(&u32IspExtBase))
		{
		    printf("Get ISP extent address failed!\n");
		}
		else
		{
		    printf("Get ISP extent address: %#x\n", u32IspExtBase);
		}
    }

    if (stat(pCfgFile, &filestat) != 0)
    {
        printf("%s: stat error!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fdcfg = fopen(pCfgFile, "r");
    if (fdcfg == NULL)
    {
        printf("%s: fopen failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    pbuf = (char*)malloc(filestat.st_size);
    if (pbuf == NULL)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        fclose(fdcfg);
        return HI_FAILURE;
    }

    if (fread(pbuf, filestat.st_size, 1, fdcfg) != 1)
    {
        printf("%s: fread failed!\n", __FUNCTION__);
        free(pbuf);
        fclose(fdcfg);
        return HI_FAILURE;
    }
    fclose(fdcfg);

    s32FileLen = 0;
    while(s32FileLen < filestat.st_size)
    {
        HI_S32 s32RdLen = 0;
        HI_CHAR curbuf[MAX_STRLEN];

        memcpy(curbuf, pbuf+s32FileLen, MAX_STRLEN);
        s32RdLen = SAMPLE_COMM_ISP_GetLine(curbuf, MAX_STRLEN);
        if(s32RdLen<= 0)
        {
            printf("%s: the file is not the register value\n", __FUNCTION__);
            free(pbuf);
            return HI_FAILURE;
        }
        curbuf[s32RdLen] ='\0';
        s32FileLen +=s32RdLen;

        if (curbuf[0] == '#')
        {
            continue;
        }
        else if ((curbuf[0] == '\n') ||(curbuf[1] == '\n'))
        {
            continue;
        }
        else if((pstr=strchr(curbuf, ':') )!= NULL)
        {
            strncpy(strReg, pstr+1, sizeof(strReg));
            strReg[REAL_REGLEN] = '\0';

            u32RegVal = strtol(strReg, NULL, 16);
            u32RegAddr = strtoll(curbuf, NULL, 16);

            CHECKREGADDR(u32RegAddr);

            s32Ret = HI_MPI_ISP_SetCfg(u32RegAddr, u32RegVal);
            if (s32Ret != HI_SUCCESS)
            {
                printf("%s: HI_MPI_ISP_SetCfg Reg%x: 0x%x failed with %#x!\n", __FUNCTION__, u32RegAddr, u32RegVal, s32Ret);
                free(pbuf);
                return HI_FAILURE;
            }
        }
        else
        {
            continue;
        }
    }

    free(pbuf);

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : load isp regist define file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_LoadRegFile(SAMPLE_CFG_OPT_E enCfgOpt, HI_CHAR *pcIspCfgFile)
{
    if (enCfgOpt == CFG_OPT_LOAD)
    {
        if (HI_SUCCESS != SAMPLE_COMM_ISP_File2Reg(pcIspCfgFile))
        {
            printf("%s: Load isp configure file failed!\n", __FUNCTION__);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Export isp regist define file
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_SaveRegFile(SAMPLE_CFG_OPT_E enCfgOpt, HI_CHAR *pcIspCfgFile)
{
    if (enCfgOpt == CFG_OPT_SAVE)
    {
        if (HI_SUCCESS != SAMPLE_COMM_ISP_Reg2File(pcIspCfgFile))
        {
            printf("%s: Export isp configure file failed!\n", __FUNCTION__);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}
#endif

/******************************************************************************
* funciton : Sensor init
* note : different sensor corresponding different lib. So,  you can change
		  SENSOR_TYPE in Makefile.para, instead of modify program code.
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_SensorInit(void)
{
    HI_S32 s32Ret;
#ifdef ISP_V1
    /* 1. sensor init */
    sensor_init();

    /* 0: linear mode, 1: WDR mode */
    sensor_mode_set(0);
#endif
    /* 2. sensor register callback */
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Init(void)
{
    HI_S32 s32Ret;
    ISP_IMAGE_ATTR_S stImageAttr;
    ISP_INPUT_TIMING_S stInputTiming;
#ifdef ISP_V2
    ALG_LIB_S stLib;

    /* 1. register ae lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 2. register awb lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 3. register af lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_Register(&stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }
#endif
    /* 4. isp init */
    s32Ret = HI_MPI_ISP_Init();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 5. isp set image attributes */
    /* note : different sensor, different ISP_IMAGE_ATTR_S define.
              if the sensor you used is different, you can change
              ISP_IMAGE_ATTR_S definition */

    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case SONY_ICX692_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case SONY_IMX104_DC_720P_30FPS:
        case SONY_IMX138_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_GBRG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;

        case OMNI_OV9712_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
			
       case OMNI_OV2710_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;

       case SOI_H22_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
			
	   case HIMAX_1375_DC_720P_30FPS:
            stImageAttr.enBayer      = BAYER_BGGR;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;	
			
        case SONY_IMX122_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_RGGB;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;
            
        case APTINA_MT9P006_DC_1080P_30FPS:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1920;
            stImageAttr.u16Height    = 1080;
            break;

        default:
            stImageAttr.enBayer      = BAYER_GRBG;
            stImageAttr.u16FrameRate = 30;
            stImageAttr.u16Width     = 1280;
            stImageAttr.u16Height    = 720;
            break;
    }

    s32Ret = HI_MPI_ISP_SetImageAttr(&stImageAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetImageAttr failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 6. isp set timing */
    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
        case OMNI_OV9712_DC_720P_30FPS:
        case OMNI_OV2710_DC_1080P_30FPS:
        case SOI_H22_DC_720P_30FPS:        
            stInputTiming.enWndMode = ISP_WIND_NONE;
            break;

		case HIMAX_1375_DC_720P_30FPS:
			stInputTiming.enWndMode = ISP_WIND_ALL; //for sony or pana
        	stInputTiming.u16HorWndLength = 1280;
        	stInputTiming.u16HorWndStart = 0;
        	stInputTiming.u16VerWndLength = 720;
        	stInputTiming.u16VerWndStart = 4;
            break;
			
        case SONY_ICX692_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL; //for sony or pana
        	stInputTiming.u16HorWndLength = 1280;
        	stInputTiming.u16HorWndStart = 200;
        	stInputTiming.u16VerWndLength = 720;
        	stInputTiming.u16VerWndStart = 2;
            break;

        case SONY_IMX104_DC_720P_30FPS:
        case SONY_IMX138_DC_720P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 68;
            stInputTiming.u16HorWndLength = 1280;
            stInputTiming.u16VerWndStart = 20;
            stInputTiming.u16VerWndLength = 720;
            break;

        case SONY_IMX122_DC_1080P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 200;
            stInputTiming.u16HorWndLength = 1920;
            stInputTiming.u16VerWndStart = 12;
            stInputTiming.u16VerWndLength = 1080;
            break;
        case APTINA_MT9P006_DC_1080P_30FPS:
            stInputTiming.enWndMode = ISP_WIND_ALL;
            stInputTiming.u16HorWndStart = 0;
            stInputTiming.u16HorWndLength = 1920;
            stInputTiming.u16VerWndStart = 2;
            stInputTiming.u16VerWndLength = 1080;
            break;
            
        default:
            printf("%s: SENSOR_TYPE %d not support!\n", __FUNCTION__, SENSOR_TYPE);
			stInputTiming.enWndMode = ISP_WIND_NONE;
            break;
    }
    s32Ret = HI_MPI_ISP_SetInputTiming(&stInputTiming);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetInputTiming failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

//SAMPLE_COMM_ISP_AGain_Min(0x400);
//SAMPLE_COMM_ISP_AGain_Max(0x800);
SAMPLE_COMM_ISP_DGain_Min(0x400);
SAMPLE_COMM_ISP_DGain_Max(0x700);
ISP_EXP_STA_INFO_S pstExpStatistic;
HI_MPI_ISP_GetExpStaInfo(&pstExpStatistic);
char szMsg[100];
sprintf(szMsg, "ExpSta:HH:%d, %d, %d, %d, HT:%d, %d, %d, %d, %d, al:%d\n",
 pstExpStatistic.u8ExpHistThresh[0], pstExpStatistic.u8ExpHistThresh[1], 
 pstExpStatistic.u8ExpHistThresh[2], pstExpStatistic.u8ExpHistThresh[3], 
 pstExpStatistic.u8ExpHistTarget[0], pstExpStatistic.u8ExpHistTarget[1], 
 pstExpStatistic.u8ExpHistTarget[2], pstExpStatistic.u8ExpHistTarget[3], 
 pstExpStatistic.u8ExpHistTarget[4], pstExpStatistic.u8AveLum);
 fprintf(stderr, szMsg);
 pstExpStatistic.u8ExpHistThresh[0] = 0x28;
 pstExpStatistic.u8ExpHistThresh[1] = 0x60;
 pstExpStatistic.u8ExpHistThresh[2] = 0xa0;
 pstExpStatistic.u8ExpHistThresh[3] = 0xc0;
pstExpStatistic.u8ExpHistTarget[0]= 0x60;
pstExpStatistic.u8ExpHistTarget[1]= 0x50;
pstExpStatistic.u8ExpHistTarget[2]= 0x28;
pstExpStatistic.u8ExpHistTarget[3]= 0x18;
pstExpStatistic.u8ExpHistTarget[4]= 0x10;

HI_MPI_ISP_SetExpStaInfo(&pstExpStatistic);
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run()
{
    HI_S32 s32Ret;

    s32Ret = SAMPLE_COMM_ISP_Init();
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: ISP init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

#if 1
    if (0 != pthread_create(&gs_IspPid, 0, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }
#else
	/* configure thread priority */
	if (1)
	{
		#include <sched.h>

		pthread_attr_t attr;
		struct sched_param param;
		int newprio = 50;

		pthread_attr_init(&attr);

		if (1)
		{
			int policy = 0;
			int min, max;

			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->default thread use policy is %d --<\n", policy);

			pthread_attr_setschedpolicy(&attr, SCHED_RR);
			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->current thread use policy is %d --<\n", policy);

			switch (policy)
			{
				case SCHED_FIFO:
					printf("-->current thread use policy is SCHED_FIFO --<\n");
					break;

				case SCHED_RR:
					printf("-->current thread use policy is SCHED_RR --<\n");
					break;

				case SCHED_OTHER:
					printf("-->current thread use policy is SCHED_OTHER --<\n");
					break;

				default:
					printf("-->current thread use policy is UNKNOW --<\n");
					break;
			}

			min = sched_get_priority_min(policy);
			max = sched_get_priority_max(policy);

			printf("-->current thread policy priority range (%d ~ %d) --<\n", min, max);
		}

		pthread_attr_getschedparam(&attr, &param);

		printf("-->default isp thread priority is %d , next be %d --<\n", param.sched_priority, newprio);
		param.sched_priority = newprio;
		pthread_attr_setschedparam(&attr, &param);

		if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
		{
			printf("%s: create isp running thread failed!\n", __FUNCTION__);
			return HI_FAILURE;
		}

		pthread_attr_destroy(&attr);
	}
#endif

#ifdef ISP_V1
    /* load configure file if there is */
#ifdef SAMPLE_LOAD_ISPREGFILE
    s32Ret = SAMPLE_COMM_ISP_LoadRegFile(CFG_OPT_LOAD, SAMPLE_ISPCFG_FILE);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: load isp cfg file failed![%s]\n", __FUNCTION__, SAMPLE_ISPCFG_FILE);
        return HI_FAILURE;
    }
#endif
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : change between linear and wdr mode
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode)
{
    HI_S32 s32Ret;
#ifdef ISP_V1    
    SAMPLE_COMM_ISP_Stop();
    
    s32Ret = sensor_mode_set(u8Mode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_mode_set failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_ISP_Run();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: SAMPLE_COMM_ISP_Run failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif
#ifdef ISP_V2
    ISP_WDR_ATTR_S stWdrAttr;
    stWdrAttr.enWdrMode = u8Mode;
    s32Ret = HI_MPI_ISP_SetWdrAttr(&stWdrAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetWdrAttr failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif
    return HI_SUCCESS;
}


/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop()
{
    HI_MPI_ISP_Exit();
    pthread_join(gs_IspPid, 0);
    return;
}

/******************************************************************************
* funciton : change Saturation
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Saturation(HI_S32 saturation)
{
//    HI_S32 s32Ret;
//	ISP_SATURATION_ATTR_S pstSatAttr;

//    HI_MPI_ISP_GetSaturationAttr(&pstSatAttr);
//	pstSatAttr.u8SatTarget = saturation;
//	HI_MPI_ISP_SetSaturationAttr(&pstSatAttr);
//    fprintf(stderr,"change saturation from %d to %d\n",pstSatAttr.au8Sat,pstSatAttr.u8SatTarget);	

    VI_CSC_ATTR_S pstCSCAttr;

    HI_MPI_VI_GetCSCAttr(0, &pstCSCAttr);   
    pstCSCAttr.u32SatuVal =  ( saturation * 100) / 255;
    HI_MPI_VI_SetCSCAttr(0, &pstCSCAttr);
}	

/******************************************************************************
* funciton : change Sharpness
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Sharpness(HI_S32 sharpness)
{
//    HI_S32 s32Ret;
	ISP_SHARPEN_ATTR_S pstSharpenAttr;

    HI_MPI_ISP_GetSharpenAttr(&pstSharpenAttr);
    
    pstSharpenAttr.bEnable=1;   //add by frank 20140520
    pstSharpenAttr.bManualEnable=1;//add by frank 20140520
    
	pstSharpenAttr.u8StrengthTarget = sharpness / 2;
	pstSharpenAttr.u8StrengthMin = pstSharpenAttr.u8StrengthTarget / 4; //u8StrengthMin must be <= .u8StrengthTarget
    HI_MPI_ISP_SetSharpenAttr(&pstSharpenAttr);        
 //   fprintf(stderr,"change sharpness %d %d %d\n",pstSharpenAttr.bEnable,pstSharpenAttr.u8StrengthTarget,pstSharpenAttr.u8StrengthMin);	
}	

/******************************************************************************
* funciton : change Brightness
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Brightness(HI_S32 brightness)
{
//    HI_S32 s32Ret;
//   ISP_AE_ATTR_EX_S pstAEAttrEx;

//   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
//   pstAEAttrEx.u8ExpCompensation = brightness;
//   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	

   VI_CSC_ATTR_S pstCSCAttr;

//   HI_MPI_VI_GetCSCAttr(VI_DEV ViDev, VI_CSC_ATTR_S *pstCSCAttr);
     HI_MPI_VI_GetCSCAttr(0, &pstCSCAttr);   
     pstCSCAttr.u32LumaVal =  ( brightness * 100) / 255;
//   HI_MPI_VI_SetCSCAttr(VI_DEV ViDev, const VI_CSC_ATTR_S *pstCSCAttr);
     HI_MPI_VI_SetCSCAttr(0, &pstCSCAttr);
   
   fprintf(stderr,"change brightness\n");	
}	

/******************************************************************************
* funciton : change Contrast
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Contrast(HI_S32 contrast)
{
    HI_S32 s32Ret;
//	ISP_DRC_ATTR_S pstDRC;
    
//    HI_MPI_ISP_GetDRCAttr(&pstDRC);
//    pstDRC.u32StrengthTarget = contrast;
//	HI_MPI_ISP_SetDRCAttr(&pstDRC);
//   fprintf(stderr,"change contrast %d\n",pstDRC.u32StrengthTarget);	

	 VI_CSC_ATTR_S pstCSCAttr;

     HI_MPI_VI_GetCSCAttr(0, &pstCSCAttr);   
     pstCSCAttr.u32ContrVal =  ( contrast * 100) / 255;
     HI_MPI_VI_SetCSCAttr(0, &pstCSCAttr);
}	


/******************************************************************************
* funciton : change Exposure Time
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ME_Exposure_Time(HI_S32 time)
{
	//	  HI_S32 s32Ret;
	ISP_ME_ATTR_EX_S pstAEAttrEx;
	
	HI_MPI_ISP_SetExposureType(OP_TYPE_MANUAL);
	
	pstAEAttrEx.bManualExpLineEnable = 1;
	pstAEAttrEx.bManualAGainEnable = 0;
	pstAEAttrEx.bManualDGainEnable = 0;
  switch(time)
   {   //line = 1/(43Mhz) * 1280, exposure = shutter/line =  0.0667/ 2.976e-5
    case  1: pstAEAttrEx.u32ExpLine = 2242; break; //1_15  0.0667/2.976e-5
    case  2: pstAEAttrEx.u32ExpLine = 1120; break; //1_30  0.0333/2.976e-5
    case  3: pstAEAttrEx.u32ExpLine =  672; break; //1_50  0.02/2.976e-5
    case  4: pstAEAttrEx.u32ExpLine =  560; break; //1_60
    case  5: pstAEAttrEx.u32ExpLine =  336; break; //1_100
    case  6: pstAEAttrEx.u32ExpLine =  280; break; //1_120
    case  7: pstAEAttrEx.u32ExpLine =  134; break; //1_250     
	case  8: pstAEAttrEx.u32ExpLine =   67; break; //1_500
    case  9: pstAEAttrEx.u32ExpLine =   34; break; //1_1000
    case 10: pstAEAttrEx.u32ExpLine =   17; break; //1_2000
    case 11: pstAEAttrEx.u32ExpLine =    8; break; //1_4000
	case 12: pstAEAttrEx.u32ExpLine =    4; break; //1_10000						       
    default:
       pstAEAttrEx.u32ExpLine = 1120;
   }	
	 HI_MPI_ISP_SetMEAttrEx(&pstAEAttrEx);	
	 sleep(5);
	
	
}
HI_S32 SAMPLE_COMM_ISP_Exposure_Time(HI_S32 time)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;
   HI_MPI_ISP_SetExposureType(OP_TYPE_AUTO);

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
   switch(time)
   {   //line = 1/(43Mhz) * 1280, exposure = shutter/line =  0.0667/ 2.976e-5
    case  1: pstAEAttrEx.u32ExpTimeMax = 2242; break; //1_15  0.0667/2.976e-5
    case  2: pstAEAttrEx.u32ExpTimeMax = 1120; break; //1_30  0.0333/2.976e-5
    case  3: pstAEAttrEx.u32ExpTimeMax =  672; break; //1_50  0.02/2.976e-5
    case  4: pstAEAttrEx.u32ExpTimeMax =  560; break; //1_60
    case  5: pstAEAttrEx.u32ExpTimeMax =  336; break; //1_100
    case  6: pstAEAttrEx.u32ExpTimeMax =  280; break; //1_120
    case  7: pstAEAttrEx.u32ExpTimeMax =  134; break; //1_250    
	case  8: pstAEAttrEx.u32ExpTimeMax =   67; break; //1_500
    case  9: pstAEAttrEx.u32ExpTimeMax =   34; break; //1_1000
    case 10: pstAEAttrEx.u32ExpTimeMax =   17; break; //1_2000
    case 11: pstAEAttrEx.u32ExpTimeMax =    8; break; //1_4000
	case 12: pstAEAttrEx.u32ExpTimeMax =    4; break; //1_10000						       
    default:
       pstAEAttrEx.u32ExpTimeMax = 1120;
   }
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change exposure time %d, %d\n",pstAEAttrEx.u32ExpTimeMax,time);	
}	

/******************************************************************************
* funciton : change Exposure Time Minimux
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Exposure_Time_Min(HI_S32 time)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;
   HI_MPI_ISP_SetExposureType(OP_TYPE_AUTO);

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
   switch(time)
   {
    case  1: pstAEAttrEx.u32ExpTimeMin = 2242; break; //1_15
    case  2: pstAEAttrEx.u32ExpTimeMin = 1120; break; //1_30
    case  3: pstAEAttrEx.u32ExpTimeMin =  672; break; //1_50 
    case  4: pstAEAttrEx.u32ExpTimeMin =  560; break; //1_60
    case  5: pstAEAttrEx.u32ExpTimeMin =  336; break; //1_100
    case  6: pstAEAttrEx.u32ExpTimeMin =  280; break; //1_120
    case  7: pstAEAttrEx.u32ExpTimeMin =  134; break; //1_250    
	case  8: pstAEAttrEx.u32ExpTimeMin =   67; break; //1_500
    case  9: pstAEAttrEx.u32ExpTimeMin =   34; break; //1_1000
    case 10: pstAEAttrEx.u32ExpTimeMin =   17; break; //1_2000
    case 11: pstAEAttrEx.u32ExpTimeMin =    8; break; //1_4000
	case 12: pstAEAttrEx.u32ExpTimeMin =    4; break; //1_10000						          
    default:
       pstAEAttrEx.u32ExpTimeMin = 4;
   }
   if(pstAEAttrEx.u32ExpTimeMin > pstAEAttrEx.u32ExpTimeMax)//added by frank for bug fixed, the Hi3518's lib says "ExpTimeMax should not be less than ExpTimeMin".
   {
		fprintf(stderr,"frank say we modify the max exp time, because ExpTimeMax should not be less than ExpTimeMin\n");
		pstAEAttrEx.u32ExpTimeMax = pstAEAttrEx.u32ExpTimeMin;
		//HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);
   }
   
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change exposure time minimux %d, %d\n",pstAEAttrEx.u32ExpTimeMin,time);	
}	

/******************************************************************************
* funciton : change Analog gain 
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_AGain_Max(HI_S32 gain)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
   pstAEAttrEx.u32AGainMax = gain;
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change Gain Max time %d, %d\n",pstAEAttrEx.u32AGainMax,gain);	
}	

HI_S32 SAMPLE_COMM_ISP_AGain_Min(HI_S32 gain)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
   pstAEAttrEx.u32AGainMin = gain;
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change eSAMPLE_COMM_ISP_Gain_Min %d, %d\n",pstAEAttrEx.u32AGainMin,gain);	
}	

/******************************************************************************
* funciton : change Digital gain
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_DGain_Max(HI_S32 gain)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);   
   pstAEAttrEx.u32DGainMax = gain;
   pstAEAttrEx.u32ISPDGainMax = 0x800; //added by frank
   pstAEAttrEx.u32SystemGainMax = 0x10000; //added by frank
   
   pstAEAttrEx.enFrameEndUpdateMode = 2;//for OV9712
   pstAEAttrEx.enAEMode =AE_MODE_LOW_NOISE;//AE_MODE_FRAME_RATE
  // char szMsg[100];
   //sprintf(szMsg, "expcomenstation:%d\n",  pstAEAttrEx.u8ExpCompensation);//default is 64;
  // fprintf(stderr, szMsg);
   //pstAEAttrEx.u8ExpCompensation = 100;//It can use as the Exposure Value.By frank
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change DGain Max time %d, %d\n",pstAEAttrEx.u32DGainMax,gain);	
}	

HI_S32 SAMPLE_COMM_ISP_DGain_Min(HI_S32 gain)
{
//    HI_S32 s32Ret;
   ISP_AE_ATTR_EX_S pstAEAttrEx;

   HI_MPI_ISP_GetAEAttrEx(&pstAEAttrEx);
   pstAEAttrEx.u32DGainMin = gain;
   HI_MPI_ISP_SetAEAttrEx(&pstAEAttrEx);	
   fprintf(stderr,"change eSAMPLE_COMM_ISP_DGain_Min %d, %d\n",pstAEAttrEx.u32DGainMin,gain);	
}	

/******************************************************************************
* funciton : change Auto White Balance
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_AWB(HI_S32 awb)
{
//    HI_S32 s32Ret;
   ISP_AWB_ATTR_S pstAWBAttr;

#if 0   
   HI_MPI_ISP_GetAWBAttr(&pstAWBAttr);
   if(awb==0) 
   	{ //0:OUTDOOR
   	  awb_isp = WB_DAYLIGHT;
      pstAWBAttr.u8HighColorTemp = 100; /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
      pstAWBAttr.u8LowColorTemp = 20;   /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */
   	} 	 
   else if(awb==1) 
    { //1:INDOOR
      awb_isp = WB_FLUORESCENT;
      pstAWBAttr.u8HighColorTemp = 55;  /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
      pstAWBAttr.u8LowColorTemp = 20;   /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */
   	} 
   else if(awb==2)
   	{ //2:BLACKWHITE
   	  awb_isp = WB_BUTT;
      pstAWBAttr.u8HighColorTemp = 65;  /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
      pstAWBAttr.u8LowColorTemp = 20;   /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */
   	} 
   else if(awb==3)
   	{ //3:TUNGSTEN
   	  awb_isp = WB_LAMP;
	  pstAWBAttr.u8HighColorTemp = 36;  /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
      pstAWBAttr.u8LowColorTemp = 20;   /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */             
   	}		  
   else
   	{ //4:AWB_AUTO
   	  awb_isp = WB_AUTO;
      pstAWBAttr.u8HighColorTemp = 65;  /*RW, AWB max temperature in K/100, Range: [0x0, 0xFF], Recommended: [85, 100] */
      pstAWBAttr.u8LowColorTemp = 20;   /*RW, AWB min temperature in K/100, Range: [0x0, u8HighColorTemp), Recommended: [20, 25] */
   	}	  
   HI_MPI_ISP_SetAWBAttr(&pstAWBAttr);
#endif      
   fprintf(stderr,"change SAMPLE_COMM_ISP_AWB %d, %d\n",pstAWBAttr.u8HighColorTemp,pstAWBAttr.u8LowColorTemp);	
}	

HI_S32 SAMPLE_COMM_ISP_Mirror(HI_S32 mirror)
{
//   mirror = ((mirror & 0x01) << 7) | ((mirror & 0x02) << 5);
//   sensor_write_register(0x04, mirror);
// Add by Frank 20140603
    HI_S32 s32Ret;
    VI_CHN_ATTR_S stChnAttr;
    int i;
    for (i = 0; i<3 ; i++)
    {
      s32Ret = HI_MPI_VI_GetChnAttr(0, &stChnAttr);
      stChnAttr.bMirror = 0;
      stChnAttr.bFlip = 0;
        switch(mirror)
        {
            case VI_CHN_SET_MIRROR:
                stChnAttr.bMirror = HI_TRUE;
                break;
 
            case VI_CHN_SET_FLIP:
                stChnAttr.bFlip = HI_TRUE;
                break;
                
            case VI_CHN_SET_FLIP_MIRROR:
                stChnAttr.bMirror = HI_TRUE;
                stChnAttr.bFlip = HI_TRUE;
                break;
                
            default:
                break;
        }
      
      s32Ret = HI_MPI_VI_SetChnAttr(0, &stChnAttr);
    }
   fprintf(stderr,"change SAMPLE_COMM_ISP_MIRROR %d\n",mirror);	
}	

HI_S32 SAMPLE_COMM_ISP_ColorBar(HI_S32 bar)
{
   sensor_write_register(0x12, bar << 1);
//   sensor_write_register(0x97, mirror);
   fprintf(stderr,"change eSAMPLE_COMM_ISP_COLOR_BAR %d\n",bar);	
}	

HI_S32 SAMPLE_COMM_ISP_AntiFlicker(HI_S32 frequency)
{
 ISP_ANTIFLICKER_S pstAntiflicker;
 
 HI_MPI_ISP_GetAntiFlickerAttr(&pstAntiflicker);
 pstAntiflicker.bEnable = 1;
 if(frequency == 1) pstAntiflicker.u8Frequency = 60;
 else if(frequency == 2) pstAntiflicker.u8Frequency = 50;
 else  pstAntiflicker.bEnable = 0;//VIDEO_NONE
 pstAntiflicker.enMode = ISP_ANTIFLICKER_MODE_0;//I choose the mode to fix exposure time, you can choose another one.
 //char szMsg[100];
 //sprintf(szMsg, "pstAntiflicker:%d\n", frequency);
 //fprintf(stderr, szMsg);
 HI_MPI_ISP_SetAntiFlickerAttr(&pstAntiflicker);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

