/**
* @brief Get_Video_Resolution
* @param AV Codec Stream Resolution Settings
* @retval 0 Success
* @retval -1 Fail.
*/	  

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include "queue.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "sample_comm.h"
#include <ipnc_ver.h> 

#define OSA_SOK      0  ///< Status : OK
#define OSA_EFAIL   -1  ///< Status : Generic error
#define	AD_IN_RTP_HD_SIZE	12
int  AUD_IN_PK_SIZE;//	(INPUTBUFSIZE+AD_IN_RTP_HD_SIZE)

typedef struct P_BUF {	
	struct P_BUF *pNext;		
	char *buf;
} P_BUF;


typedef struct {
//	OSA_TskHndl 	audioTsk;
//	DRV_spkHndl 	audioHndl;
	pthread_t       spkHndl;
	int 			streamId;
	unsigned char 	*decodedBuffer;
	int				decodeBufSize;
	unsigned char 	*inputBuffer;
	int				inputBufSize;
} SPK_Ctrl;


QUEUE(P_BUF) qa_BUF; 
QUEUE(P_BUF) qf_BUF; 
int free_count = 0, count = 0;
int sock = -1;

HI_S32 s32AudioChnNum = 1;
SPK_Ctrl   gSPK_ctrl;

/* The sample rate of the audio codec */
#define SAMPLE_RATE            (8000)
#define AUD_AMMR 0
#define AUD_G726 0

/* Number of samples to process at once */
#define NUMSAMPLES             1024
#define RAWBUFSIZE             NUMSAMPLES * 2
#define INPUTBUFSIZE           RAWBUFSIZE
/* The max amount of bytes of speech data to process at once */
#define DECODEDBUFSIZE        NUMSAMPLES  *4  //Default for G711

unsigned short gAudioPort = AUDIO_SPEECH_PORT; 
int gPortChanging = 0;

//#ifdef  INET6
#if 0
struct sockaddr_in6 srvSock;
#else
struct sockaddr_in srvSock;
#endif

int sock_len = sizeof(struct sockaddr);
int step_size[4] = {2,2,8,4};   

unsigned int SPK_GetTimeStamp(void)
{
	struct timeval timeval;

	gettimeofday(&timeval, NULL);

	return (timeval.tv_sec * 1000) + (timeval.tv_usec + 500) / 1000;
}

int  Open_Server(int PKT_SIZE, int port)
{
	if (sock != -1)
		close(sock);  
//#ifdef INET6
#if 0
	if (-1 == (sock = socket(AF_INET6, SOCK_DGRAM, 0)))
#else
	if (-1 == (sock = socket(AF_INET, SOCK_DGRAM, 0)))
#endif
	{      // Open RTP Server 
		printf("\nRS DBG SPK Client:  Error opening socket");
		sock = -1;
		return OSA_EFAIL;
	}
	else
	{
		printf("\nRS DBG SPK Client: opening socket !!!!!!!!!!!\n");
	}

	memset((char *)&srvSock, 0, sizeof(srvSock));
//#ifdef INET6
#if 0
	srvSock.sin6_family = AF_INET6;
	srvSock.sin6_port   = htons(port);
	memcpy(&srvSock.sin6_addr, &in6addr_any, sizeof(in6addr_any));
#else
	srvSock.sin_family = AF_INET;
	srvSock.sin_port   = htons(port);
	srvSock.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	if (sock != -1)
	{
		if (bind(sock, (struct sockaddr *)&srvSock, sizeof(srvSock)) == - 1)
		{
			printf("DBG SPK : Bind error\n");
			sock = -1;
			return OSA_EFAIL;
		}
		else
		{
			printf("DBG SPK :  Bind OK \n");
		}
	}
	//AUD_IN_PK_SIZE=PKT_SIZE+AD_IN_RTP_HD_SIZE;
	AUD_IN_PK_SIZE=PKT_SIZE;
	return OSA_SOK;
}

void Close_Server()
{
	if (sock != -1)
		close(sock);  
}

int RecvData(int codec, int size, unsigned char *ip_buffer)
{
	int i, numBytes = 0;

	P_BUF *tmppk = 0;
	int flags = fcntl(sock, F_GETFL, 0);

	if (free_count >= AUD_IN_PK_SIZE) 
	{
		fcntl(sock, F_SETFL, flags|O_NONBLOCK);
		numBytes = recvfrom(sock, ip_buffer,(AUD_IN_PK_SIZE+AD_IN_RTP_HD_SIZE), 0,(struct sockaddr *)&srvSock, &sock_len);

		if (numBytes > 0)
		{
		//	printf("\n Request  %d , Receive audio numBytes = %d !!!!!\n", AUD_IN_PK_SIZE+AD_IN_RTP_HD_SIZE,numBytes);

			for (i=AD_IN_RTP_HD_SIZE;i<numBytes;i++)
			{
				DeQueue(qf_BUF, &tmppk);
				memcpy(tmppk->buf,&ip_buffer[i],1);
				EnQueue(qa_BUF, tmppk);
				count++;
				free_count--;
			}
		}
		else
		{
		//	printf("\n Not Data Get !!........\n");
			return 0;
		}
	}

	if (size >count)
		size = count;

	for (i=0;i<size;i++)
	{
		DeQueue(qa_BUF, &tmppk);
		memcpy(&ip_buffer[i],tmppk->buf,1);
		EnQueue(qf_BUF, tmppk);
		free_count++;
		count--;
	}
 //   printf("\n Requst = %d ,current =%d\n",size,count);
	return size;
}

void ClearQ(void)
{
	P_BUF *tmppk = 0;

	while(free_count>0)
	{
		DeQueue(qf_BUF, &tmppk);
		free(tmppk->buf);
		free(tmppk);
		free_count--;
	}

	while(count>0)
	{
		DeQueue(qa_BUF, &tmppk);
		free(tmppk->buf);
		free(tmppk);
		count--;
	}
	ClearQueue(qa_BUF);	 
	ClearQueue(qf_BUF);
}

int  InitQueue(int Size)
{
	int i;
	ClearQueue(qa_BUF);
	ClearQueue(qf_BUF);
	free_count=0; 
	count=0;
	for (i = 0; i <(Size) ; i++) //5x1024
	{
		P_BUF * tmppk = malloc(sizeof(P_BUF));	
		if (tmppk == NULL)
		return OSA_EFAIL;
		tmppk->buf = (unsigned char *)malloc(sizeof(unsigned char));
		if (tmppk->buf  == NULL)
		return OSA_EFAIL;
		EnQueue(qf_BUF, tmppk);
		free_count +=1;
	}
	return OSA_SOK;
}

int SPK_ChangePort(unsigned short port) 
{
	gPortChanging = 1;
	Close_Server();
	gAudioPort = port;
  fprintf(stderr, "SPK_ChangePort audio port = %d\n", gAudioPort);
  Open_Server(INPUTBUFSIZE, gAudioPort);
	gPortChanging = 0;
	return 1;
}


int SPK_audioTskRun(AUDIO_STREAM_S stStream)
{
	int	rc = 0,retVal = OSA_SOK;
//	int	numBytes = 0, decodedBufferSize = 0;
//    decoder_func_t decoder;

//    numBytes = RecvData(0 ,NUMSAMPLES, gSPK_ctrl.inputBuffer);//gSPK_ctrl.inputBuffer);
     stStream.u32Len = RecvData(0 ,NUMSAMPLES, stStream.pStream);//gSPK_ctrl.inputBuffer);

	if(stStream.u32Len > 0)
	{
// 	 stStream.u32Len = numBytes;
//  	 stStream.pStream = gSPK_ctrl.inputBuffer;
	 
//	 printf("Received Audio data %x %x %x\n",gSPK_ctrl.inputBuffer[0],gSPK_ctrl.inputBuffer[1],gSPK_ctrl.inputBuffer[2]);
 	 printf("Received Audio data %x %x %x\n",stStream.pStream[0],stStream.pStream[1],stStream.pStream[2]);
     HI_MPI_ADEC_SendStream(0, &stStream, HI_TRUE);
	} 
    
    return OSA_SOK;
}

int SPK_audioTskMain() //struct OSA_TskHndl *pTsk, OSA_MsgHndl *pMsg, Uint32 curState )
{
  int status;
  HI_BOOL done = HI_FALSE, ackMsg = HI_FALSE;
  AUDIO_STREAM_S stStream;
  HI_S32 s32AdecChn = 0;
  	
  stStream.pStream = (HI_U8*)malloc(sizeof(HI_U8)*MAX_AUDIO_STREAM_LEN);  

  while (!done)
    {
		if (gPortChanging == 1)
			continue;
//          status = SPK_audioTskRun(stStream);
       stStream.u32Len = RecvData(0 ,NUMSAMPLES, stStream.pStream);//gSPK_ctrl.inputBuffer);

       if(stStream.u32Len > 0)
	   {
// 	 stStream.u32Len = numBytes;
//  	 stStream.pStream = gSPK_ctrl.inputBuffer;
	 
//	 printf("Received Audio data %x %x %x\n",gSPK_ctrl.inputBuffer[0],gSPK_ctrl.inputBuffer[1],gSPK_ctrl.inputBuffer[2]);
 	      printf("Received Audio data %x %x %x\n",stStream.pStream[0],stStream.pStream[1],stStream.pStream[2]);
//          HI_MPI_ADEC_SendStream(s32AdecChn, &stStream, HI_TRUE);
	   } 
		
//		printf("Speaker Task...\n");
   	}	
  
  free(stStream.pStream);
  SPK_audioTskDelete();
}

int SPK_audioTskCreate()
{
    int status;

   /* Allocate decode buffer */
	gSPK_ctrl.decodedBuffer = (unsigned char *)malloc(DECODEDBUFSIZE); //U16
	if (gSPK_ctrl.decodedBuffer == NULL)
	{
		printf("Failed to allocate encodedBuffer\n");
		return OSA_EFAIL;
	}

   /* Allocate input buffer */
    gSPK_ctrl.inputBuffer = (unsigned char *)malloc(DECODEDBUFSIZE);//U8, MAX Buffer in G711
	if (gSPK_ctrl.inputBuffer == NULL)
	{
		printf("Failed to allocate input buffer\n");
		return OSA_EFAIL;
	}

    Open_Server(INPUTBUFSIZE, gAudioPort);
	
    if( InitQueue(DECODEDBUFSIZE) != OSA_SOK)
 	{
        printf("DBG SPK UI: Init Queue Error");
        return OSA_EFAIL;
  	}
	
//    if( pthread_create( &gSPK_ctrl.spkHndl, 0, SPK_audioTskMain, ( void * )s32AudioChnNum ))
//	{	printf("Error Speaker_tskCreate()\n");  }
//	else 
//    {   printf("Speaker_tskCreate()\n");  }
	
    return status;
}

int SPK_audioTskDelete()
{
  pthread_join( gSPK_ctrl.spkHndl, 0 );
  pthread_exit( 0 );
  
  Close_Server();
}

