/* ===========================================================================
* @file ashare_mem.c
*
* @path $(IPNCPATH)/util/
*
* @desc Share memory functions.
* .
* Copyright (c) RS.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <string.h>
#include "ashare_mem.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "queue.h"
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

/* Video Part */
static QUEUE(PRE_BUF) qPRE_BUF; 
static QUEUE(PRE_BUF) qFRE_BUF; 
static int q_count=0,q_free_count=0;
static int Num_Frame_Interval=0;
static int frame_rate=0;
static int read_num=0;   /*Total Number for reading*/
static int imageWidth,imageHeight, imageType=-1;
static int post_cnt=0; /*post alarm  CNT*/
static int ch_handle=0; /*Change Handle*/
static  int handle; /*handle right*/
static  int  status;  /*memory state*/
static int Q_LOCK=0;      /*Lock Q*/
static int BitRate;

#ifndef Pre_Audio_Off
/*Audio Part*/
static QUEUE(PRE_BUF_AU) AqPRE_BUF; 
static QUEUE(PRE_BUF_AU) AqFRE_BUF; 
static int Aq_count=0,Aq_free_count=0;
static int Num_Audio_Interval=0;
static int read_num_au=0;   /*Total Number for reading*/
static int AudioType=-1;
static int Apost_cnt=0; /*post alarm  CNT*/
static int Ach_handle=0; /*Change Handle*/
static  int AU_handle; /*handle right*/
static  int  AU_status;  /*memory state*/
static int AQ_LOCK=0;      /*Lock Q*/
#endif

/* Gobal Control */
static int PreTime,PostTime;//Sec.
int WritePre=0;
#ifndef Pre_Audio_Off 	   
static double AV_SYNC_Time=0;
static int AV_SYNC=0;
#endif

/*Alarm Audio */
int Alarm_Audio=0;
int front_time = 0, next_time = 0;

#ifdef FITI_VISION
const int Frame_Rate[10] = {30,25,20,15,10,8,5,3,2,1};
#endif

//#define DEBUG
#ifdef DEBUG
#define DBG(fmt, args...)	printf("ashare_mem: Debug\n" fmt, ##args)
#define ERR(fmt, args...)	printf("ashare_mem: Error\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#define ERR(fmt, args...)	
#endif

int Alarm_state; /*0: pre ; 1: post*/

/*This Part will be modify latter since dynamic pre/post cnt  */
static int QMemInit	(int cnt,int acnt)
{
      int i;
	  
	/* Video Part */ 
	ClearQueue(qPRE_BUF);
	ClearQueue(qFRE_BUF);
	q_count=0;
	 q_free_count=0; 
	
	 for (i = 0; i <cnt  ; i++) 
	 {
             PRE_BUF *tmppk = malloc(sizeof(PRE_BUF));
	       if (tmppk == NULL)
		 	return 0;
		 tmppk->buf = (char *)malloc(V_BUF_SIZE_DEFAULT);	
		 if (tmppk->buf  == NULL)        			
		 	return 0;	      
		 		
		 tmppk->buf_size = V_BUF_SIZE_DEFAULT;	
		 tmppk->frame_type =0; 
	        tmppk->timestamp=0;
		EnQueue(qFRE_BUF, tmppk);		
		q_free_count +=1;
	       
	 }
     handle  = WRITE_HANDLE; 
     status  = NOT_READY;	 
    
#ifndef Pre_Audio_Off 
	 /* Audio Part */ 
       ClearQueue(AqPRE_BUF);
	ClearQueue(AqFRE_BUF);	 
	 Aq_count=0;
	 Aq_free_count=0;
	 for (i = 0; i <acnt  ; i++) 
	 {
             PRE_BUF_AU *tmppk = malloc(sizeof(PRE_BUF_AU));
	       if (tmppk == NULL)
		 	return 0;
		 tmppk->buf = (char *)malloc(A_BUF_SIZE_DEFAULT);	
		 if (tmppk->buf  == NULL)        			
		 	return 0;	      
		 		
		 tmppk->buf_size = A_BUF_SIZE_DEFAULT;		 
	        tmppk->timestamp=0;
		EnQueue(AqFRE_BUF, tmppk);		
		Aq_free_count +=1;
	       
	 } 
	 
	 AU_status  = NOT_READY;	
	AU_handle  = WRITE_HANDLE; 


   /*GOBAL */
   AV_SYNC_Time=0;	
   AV_SYNC=0;
   #endif
	 return 1;
}

static void Reset_Q()
{
   PRE_BUF *pPk;
   #ifndef Pre_Audio_Off 
   PRE_BUF_AU *pPkA;
   #endif
   
   //printf("\n RESER Q Starting...\n");
   
   /*Vodeo Part*/
   while(q_count>0)
   	{
            //PRE To FRE 
	     DeQueue(qPRE_BUF, &pPk);
            EnQueue(qFRE_BUF, pPk); 	        
	     q_free_count +=1;
	     q_count -=1;		    
         
   	}
            /* Initial  the shared memory segment  */       
	status  = NOT_READY;	
	handle  = WRITE_HANDLE; 
	 Q_LOCK=0;      
        ch_handle=0; 
		
#ifndef Pre_Audio_Off 
 /* Audio Part */ 
 while(Aq_count>0)
   	{
            //PRE To FRE 
	     DeQueue(AqPRE_BUF, &pPkA);
            EnQueue(AqFRE_BUF, pPkA); 	        
	     Aq_free_count +=1;
	     Aq_count -=1;	         
   	}
       AU_status  = NOT_READY;	
	AU_handle  = WRITE_HANDLE; 
	AQ_LOCK=0;      
       Ach_handle=0; 


	AV_SYNC_Time=0;  
	AV_SYNC=0;
#endif	 
}

int AShareMemInit(int FRate,int videoBitRate,int Width,int Height,int V_type,int A_type,int upre_cnt,int upost_cnt)
{
       /* Current Case : +1 to meet pre cnt is zero*/
	int i;	
	   
	 frame_rate =  Frame_Rate[FRate];
	Num_Frame_Interval = frame_rate *(upre_cnt+1);//(pre+1)
	BitRate =videoBitRate;
	fprintf( stderr, "\nPre Alarm:  frame rate = %d , interval = %d,BitRate = %d\n",frame_rate,Num_Frame_Interval,BitRate);
       imageWidth=Width;
	imageHeight=Height;
	imageType = V_type;    /*MP4/JPG*/
	/*Keep correct GOP For Mp4, GOP Define ,Need Re-Check*/
	i=((upre_cnt+upost_cnt)*frame_rate);
	//redundant =i%GOP_NUM;	
        //read_num = i-redundant;    /*Discard Redundance*/
        read_num =i; //RS
	//fprintf( stderr, "\n Read: Video = %d\n",read_num);		
	
	
#ifndef Pre_Audio_Off 
	Num_Audio_Interval = GOP_AUDIO*(upre_cnt+1);//(pre+1)	
	fprintf( stderr, "\n  Pre Alarm : Audio = %d\n",Num_Audio_Interval);
	read_num_au=GOP_AUDIO*(upre_cnt+upost_cnt) ; 
	fprintf( stderr, "\n Read: Audio =%d\n",read_num_au);	
	AudioType =  A_type; /*Audio Codec*/	
	fprintf( stderr, "\n Audio Codec : %d\n",AudioType);
#endif

	PreTime= upre_cnt;
	PostTime = upost_cnt;	
if(PreTime!=0)	
{
	//fprintf( stderr, "\n IMAGE = %d X %d, Buffering Type =%d\n",imageWidth,imageHeight,imageType);
       //fprintf( stderr, "\n Duration : Pre Alarm = %d, Post Alarm = %d\n",PreTime,PostTime);	
	
	/*Attach the Dynamic memory segment to Free Queue*/   
#ifndef Pre_Audio_Off 		
	if(!QMemInit(Num_Frame_Interval,Num_Audio_Interval ))
#else
       if(!QMemInit(Num_Frame_Interval,0 ))
#endif
	   fprintf( stderr, "\n QFree MemInit Failed \n");	
	else
	   fprintf( stderr, "\n QFree MemInit Success \n");
	Alarm_state=0;
}else
{
//fprintf( stderr, "\n IMAGE = %d X %d, Buffering Type =%d\n",imageWidth,imageHeight,imageType);
//fprintf( stderr, "\n Duration : Only Post Alarm = %d\n",PostTime);	
Alarm_state=1;
}

	Alarm_Audio=0;
	 WritePre=1; //RS 0429
	return 1;
}

#ifndef Pre_Audio_Off 
static int AV_Sync(double timestamp)	
{
       int i,Swapcnt=1;
	PRE_BUF_AU *pPk;			
	
	while(Swapcnt )
   	 {

		 /*Keep The VIdeo Last timestamp*/	
		pPk = PeekQueueFront(AqPRE_BUF);		
	       if(pPk->timestamp>timestamp)
	       	{
	                    DeQueue(AqPRE_BUF, &pPk);
                           EnQueue(AqFRE_BUF, pPk); 	        
	                    Aq_free_count +=1;
	                    Aq_count -=1;		    
   	              }else
   	                 Swapcnt=0;
	}			  
	AU_status  = FILLED;	
	
}
#endif

static int QSwap(char *buf, int length,int frame_type , double timestamp)	
{
  int i,cnt=0;
	PRE_BUF *pPk;	
	if(imageType==IsJPEG)
		{
	           cnt =MIN(GOP_NUM,frame_rate); /*This will be fine tune */
	           cnt -=1;	 	
	           for(i=0;i<(cnt);i++)
   	                {
           
	                    //PRE To FRE 
	                     DeQueue(qPRE_BUF, &pPk);
                            EnQueue(qFRE_BUF, pPk); 	        
	                     q_free_count +=1;
	                      q_count -=1;	    
                        }  
		}else  //RS 0422
			{
			          /*Keep I Frame in front*/	
		             pPk = PeekQueueFront(qPRE_BUF);
                              if(pPk->frame_type==1)
                              	{
                                      //PRE To FRE 
	                                DeQueue(qPRE_BUF, &pPk);
                                      EnQueue(qFRE_BUF, pPk); 	        
	                               q_free_count +=1;
	                               q_count -=1;
				         
                              	}			
                        while(!cnt)
                        	{
                        	
                               /*Keep I Frame in front*/	
		             pPk = PeekQueueFront(qPRE_BUF);
                           if(pPk->frame_type!=1)  //clear P To I
                           	{
					 //PRE To FRE 
	                                DeQueue(qPRE_BUF, &pPk);
                                      EnQueue(qFRE_BUF, pPk); 	        
	                               q_free_count +=1;
	                               q_count -=1;	   	
                        	}else
                        	   cnt=1;

                        	}

			}
	status  = FILLED;	
	//PRE To PRE
	//DeQueue(qPRE_BUF, &pPk);
	DeQueue(qFRE_BUF, &pPk); //RS 0422
	q_free_count -=1;
	if (pPk) {
	
		if (length > pPk->buf_size)
			{
                           	free(pPk->buf);	
				pPk->buf = (char *)malloc(length);	
				if (pPk->buf   == NULL) 
					{					  
					  Reset_Q();
					  return 1;
					}
									
			  }
                            pPk->buf_size = length;	
				pPk->frame_type =frame_type; 
	                     pPk->timestamp=timestamp;
				memcpy(pPk->buf, buf, length);				
				EnQueue(qPRE_BUF, pPk);	
				q_count +=1;	   
				
#ifndef Pre_Audio_Off 				
		/*Keep The VIdeo Last timestamp*/	
		pPk = PeekQueueFront(qPRE_BUF);	
		
		/*AV Sync */
		AV_Sync(pPk->timestamp);
#endif		
		return 1;
		}
	else
		    {
					  
					  Reset_Q();
					  return 1;
			}
}

static int INSER_Q(char *buf, int length,int frame_type , double timestamp)
{
      PRE_BUF *pPk;
	DeQueue(qFRE_BUF, &pPk);
	q_free_count -=1;	
	if (pPk) {
	
		if (length > pPk->buf_size)
			{
                           	free(pPk->buf);	
				pPk->buf = (char *)malloc(length);	
				if (pPk->buf   == NULL)
					{
					 
					  Reset_Q();
					  return 1;
					}
									
			  }
                            pPk->buf_size = length;	
				pPk->frame_type =frame_type; 
	                     pPk->timestamp=timestamp;				
				memcpy(pPk->buf, buf, length);				
				EnQueue(qPRE_BUF, pPk);
				q_count +=1;
				 
				return 1;
		}	
	return 0;  //error
}

/*Use to Buffering Memory Content about Video*/
static int AShareMemWriteOut(char *buf, int length,int index,int frame_type , double timestamp)  
{
        
	   char filename[25]="";
	   FILE *pfd;	
	   if(imageType==IsJPEG)
	   	{
	          sprintf(filename,"/tmp/%d.jpg",index);   
	          pfd = fopen(filename,"wb");
	       if( pfd == NULL )
	   	  {
                  Reset_Q(); //reset and skip frame
		    return 1;
	   	  }
	       fwrite(buf,length, 1, pfd); 
		//fflush(pfd);
	       fclose(pfd);  
	  }else
	  	{
                 sprintf(filename,"/tmp/%d.m4v",index);   
                   pfd = fopen(filename,"wb");
	           if( pfd == NULL )
	     	    {
                   Reset_Q(); //reset and skip frame
		     return 1;
	   	    }
                  fwrite(&frame_type, sizeof(int), 1, pfd); 
                   fwrite(&length, sizeof(int), 1, pfd); 
		     fwrite(&timestamp,sizeof(double), 1, pfd); 
		     fwrite(buf,length, 1, pfd); 
		    // fflush(pfd);
		     fclose(pfd);
	  	}
	   return 1;

	   
}

/*Adding Video Stream to Video Q, RS*/
int  AShareMemWrite(char*buf, int length,int frame_type , double timestamp,int IMG_TYPE)
{

//Wait Test

if(!Q_LOCK)   //NO ALARM
{
   if(q_free_count>0)
    {
          if(INSER_Q(buf,length, frame_type ,  timestamp))
		  return 1;		
      } 
      else
      	{
             if(QSwap(buf,length, frame_type ,  timestamp))
		  return 1;
      	}

}else //Q_LOCK

{
     if(!ch_handle) //Fist meet QLOCK:change handle and write out post
     	{
            
            post_cnt=q_count;/*post alarm  CNT*/
	     handle  = READ_HANDLE; 			 
	  
	    if(AShareMemWriteOut(buf,length,post_cnt,frame_type ,  timestamp))
	    	{
	         post_cnt +=1;
                ch_handle=1;	
		  return 1;  
	    	}
          
     	}
	 else
	{
                    if(post_cnt<read_num)  //write out post
                    	{
                    	  if(AShareMemWriteOut(buf,length,post_cnt,frame_type ,  timestamp))
	                   {
				  post_cnt +=1;
			         return 1;
                    	  	}			   
			  		
                    	}
			else  //post write out  finish 
			{
			    
                             if(handle  == WRITE_HANDLE )  
                                {
                                   if(frame_type==1)  /*Restarting in I frame*/
					 {
					     Q_LOCK=0; //UN-LOCK
                                        ch_handle=0;                       
                                        if(INSER_Q(buf,length, frame_type ,  timestamp))
					       {
                                #ifndef Pre_Audio_Off          
						AV_SYNC_Time =	timestamp;
						AV_SYNC =1;
				    #endif		
						return 1;
					       }
                             	}else
                             	  	return 1; //  do not thing skip frame  
                                 }else                            	
                             	      return 1; //  do not thing skip frame  
			}
	}

return 1; 
   


}
 	return 0;
}

int AShareMemWritePre(int post,int Cur_Frame_Rate) //Write out Pre Alarm
{
      
if(Cur_Frame_Rate>=0) 
{
	 front_time = time((time_t *) NULL);
	  Alarm_Audio=1; //set Alarm Audio	
	  printf("\n Alarm_Audio =%d\n",Alarm_Audio);
if(!post)  
	{
	  Q_LOCK=1; //Set LOCK
	  printf("\n Q_LOCK =%d\n",Q_LOCK);
         WritePre=0; 		
	 read_num =((PreTime+PostTime)*Frame_Rate[Cur_Frame_Rate]); //RS 0422 , Reset 
	  printf("\n Frame_Rate =%d,read_num =%d\n",Frame_Rate[Cur_Frame_Rate],read_num);
         } 
}else  //only alarming
{
         front_time = time((time_t *) NULL);
	  Alarm_Audio=1; //set Alarm Audio	
}
     return 1;
 
}

int AShareMemWritePre2() //Write out Pre Alarm
{
        PRE_BUF *pPk;
#ifndef Pre_Audio_Off 	   		
	 PRE_BUF_AU *pPkA;	
#endif
	 int i;
	 char filename[25]="";
	 FILE *pfd;	
	 
	
	
if(!WritePre)
{
    printf("\n  AShareMemWritePre2 processing :handle  != WRITE_HANDLE\n");
	
#ifndef Pre_Audio_Off 	   	    
    AQ_LOCK=1; //Set LOCK
#endif    
    printf("\n Writing Info\n");
   //Video Info : NUM--WIDTH--HEIGHT
    sprintf(filename,"/tmp/AVINFO");   //Write Related Info about Video
    pfd = fopen(filename,"wb");
    // i=1;  //pre alarm flag	
    // fwrite(&i, sizeof(int), 1, pfd);  //Total alarm count	
     fwrite(&read_num, sizeof(int), 1, pfd);  //Total alarm count
     fwrite(&imageWidth, sizeof(int), 1, pfd);  //imageWidth
     fwrite(&imageHeight, sizeof(int), 1, pfd);  //imageHeight  
     fwrite(&BitRate, sizeof(int), 1, pfd);  //imageHeight  
#ifndef Pre_Audio_Off 	    
     fwrite(&read_num_au, sizeof(int), 1, pfd);  //Total alarm count
     fwrite(&AudioType, sizeof(int), 1, pfd);  
#endif
     fclose(pfd);
    
  //Video Start

   i=0;
  // discard = PreTime;
  printf("\n q_count =%d \n",q_count);
   while(q_count>0) 
   {	
       
	 DeQueue(qPRE_BUF, &pPk);
	 q_count -=1;

	 if (pPk) 	   	
       {
          if(imageType==IsJPEG)
          	{
	         sprintf(filename,"/tmp/%d.jpg",i);   
	         pfd = fopen(filename,"wb");
	       if( pfd == NULL )
	     	{
                  Reset_Q(); //reset and skip frame
		  return 1;
	   	}
	      fwrite(pPk->buf, pPk->buf_size, 1, pfd); 
	     // fflush(pfd);
	      fclose(pfd);
          	}
	    else  //mp4
	    	{
                  sprintf(filename,"/tmp/%d.m4v",i);   
		   // printf("\n writing file =%s\n",filename);   		  
                   pfd = fopen(filename,"wb");
	           if( pfd == NULL )
	     	    {
                   Reset_Q(); //reset and skip frame
		     return 1;
	   	    }
                   fwrite(&pPk->frame_type, sizeof(int), 1, pfd); 
                   fwrite(&pPk->buf_size, sizeof(int), 1, pfd); 
		     fwrite(&pPk->timestamp,sizeof(double), 1, pfd); 
		     fwrite(pPk->buf, pPk->buf_size, 1, pfd); 
		   // fflush(pfd);
	            fclose(pfd);
			//printf("\n Frame type =%d\n",pPk->frame_type);
	    	}	
		  
	   EnQueue(qFRE_BUF, pPk);
	   q_free_count +=1;	   
       }
   	i++;
   }
   handle  = WRITE_HANDLE  ;//Changing handle to writer
   status  = NOT_READY;
   
#ifndef Pre_Audio_Off 	
   //Audio Start
   while(AU_handle  == WRITE_HANDLE) {} //wait handle change to READ   
   i=0;
   while(Aq_count>0) 
   {	
       
	 DeQueue(AqPRE_BUF, &pPkA);
	 Aq_count -=1;	

	 if (pPkA) 	   	
       {          
                if(imageType!=IsJPEG)
            {
		   sprintf(filename,"/tmp/%d.au",i);   
                   pfd = fopen(filename,"wb");
	           if( pfd == NULL )
	     	    {
                   Reset_Q(); //reset and skip frame
		     return 1;
	   	    }
                   fwrite(&pPkA->buf_size, sizeof(int), 1, pfd); 
		     fwrite(&pPkA->timestamp,sizeof(double), 1, pfd); 
		     fwrite(pPkA->buf, pPkA->buf_size, 1, pfd); 
		     fflush(pfd);
	            fclose(pfd);
              }	
		  
	   EnQueue(AqFRE_BUF, pPkA);
	   Aq_free_count +=1;	   
       }
   	i++;
   }
    AU_handle  = WRITE_HANDLE  ;//Changing handle to writer
   AU_status  = NOT_READY;   
   
 #endif  
    WritePre=1;
     return 1;
}
else
	return 1;
 
}
#ifndef Pre_Audio_Off 	
/*--------------------------------------------------------*/

/*Use to Buffering Memory Content about Audio*/
/* Structure: length--timestamp--data            */
static int AShareMemAuWriteOut(char *buf, int length,int index, double timestamp)  
{
        
	   char filename[25];
	   FILE *pfd;	
	  if(imageType!=IsJPEG)
	  	{
                 sprintf(filename,"/tmp/%d.au",index);   
                   pfd = fopen(filename,"wb");
	           if( pfd == NULL )
	     	    {
                   Reset_Q(); //reset and skip frame
		     return 1;
	   	    }
                   fwrite(&length, sizeof(int), 1, pfd); 
		     fwrite(&timestamp,sizeof(double), 1, pfd); 
		     fwrite(buf,length, 1, pfd); 	
		   //  fflush(pfd);
	            fclose(pfd);
	  	}	
	   return 1;

	   
}


static int INSER_AQ(char *buf, int length, double timestamp)
{
      PRE_BUF_AU *pPk;
	DeQueue(AqFRE_BUF, &pPk);
	Aq_free_count -=1;	
	if (pPk) {
	
		if (length > pPk->buf_size)
			{
                           	free(pPk->buf);	
				pPk->buf = (char *)malloc(length);	
				if (pPk->buf   == NULL)
					{
					  
					  Reset_Q();
					  return 1;
					}
									
			  }
                            pPk->buf_size = length;				
	                     pPk->timestamp=timestamp;				
				memcpy(pPk->buf, buf, length);				
				EnQueue(AqPRE_BUF, pPk);
				Aq_count +=1;
				 
				return 1;
		}	
}


int  AShareMemAuWrite(char*buf, int length, double timestamp,int AU_TYPE)
{


if(!AQ_LOCK)   //NO ALARM
{
   if(Aq_free_count>0)
    {
          if(INSER_AQ(buf,length,  timestamp))
		  return 1;		
      } 
   #if 0
      else
      	{
             if(AV_Sync(buf,length, timestamp))
		  return 1;
      	}
   #endif
}else //Q_LOCK

{
     if(!Ach_handle) //Fist meet QLOCK:change handle and write out post
     	{
            
            Apost_cnt=Aq_count;/*post alarm  CNT*/
	     AU_handle  = READ_HANDLE; 			 
	  
	    if(AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp))
	    	{
	         Apost_cnt +=1;
                Ach_handle=1;	
		  return 1;  
	    	}
          
     	}
	 else
	{
                    if(Apost_cnt<read_num_au)  //write out post
                    	{
                       
			    
			  #if 0
			  if(AV_SYNC)  //Video Finish
                            {
                              if(timestamp>AV_SYNC_Time)
                                 {
                               	   INSER_AQ(buf,length, timestamp);	
					   AQ_LOCK=0; //UN-LOCK
                                       Ach_handle=0;                    
                             	    AV_SYNC=0;
						return 1;					  
                                 }
				else
					{
                                      AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp);
                                      Apost_cnt +=1;
					   return 1;
					}
			   	} 
			   else	
			   	{
			           if(AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp))
	                       {
				   Apost_cnt +=1;
			          return 1;
                    	  	  }	
			   	} 
			   #endif
			   if(AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp))
	                       {
				   Apost_cnt +=1;
			          return 1;
                    	  	  }	
			  		
                    	}
			else  //post write out  finish 
			{
			    
			   // while(q_count>0) { } //check pre, assume pre > post if no error                                     
                             if(AU_handle  == WRITE_HANDLE )  
                             	{
                                      if(AV_SYNC)
                                      	{
                                      	   if(timestamp>AV_SYNC_Time)
                                      	   	{
                               	                  INSER_AQ(buf,length, timestamp);	
					                  AQ_LOCK=0; //UN-LOCK
                                                     Ach_handle=0;                    
                             	                  AV_SYNC=0;
							   return 1;					  
                                      	   	}
							else
								{
							    AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp);
								Apost_cnt +=1;
						            return 1;  //write out and wait video to restart 
								}
                                      	}else
                                      		{
                                      		AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp);
							Apost_cnt +=1;
                                      	       return 1;  //write out and wait video to restart 
                                      		}
				    }
				   else
				   	{
				   	AShareMemAuWriteOut(buf,length,Apost_cnt,  timestamp);
					Apost_cnt +=1;
				       return 1;  //do not thing skip 	
				   	}
							 
                      }
                           	
                           	return 1; //  do not thing skip frame  
			}
	}

return 1;
   


}
#endif

void AShareMemClear(void)
{

PRE_BUF *pPk;
#ifndef Pre_Audio_Off 	   	
   PRE_BUF_AU *pPkA;
#endif

   printf("\n Clear Q Starting...\n");
  if(PreTime!=0)
  	{
   /*Vodeo Part*/
   while(q_count>0)
   	{
            
	     DeQueue(qPRE_BUF, &pPk);
	     free(pPk->buf);
	     free(pPk); 
	     q_count -=1;		    
         
   	}

   while(q_free_count>0)
   	{
            
	     DeQueue(qFRE_BUF, &pPk);
             free(pPk->buf);
	      free(pPk);	
	     q_free_count -=1;	    	    
         
   	}
      ClearQueue(qPRE_BUF);
	ClearQueue(qFRE_BUF);
	
#ifndef Pre_Audio_Off 	
 /* Audio Part */ 
 while(Aq_count>0)
   	{           
	     DeQueue(AqPRE_BUF, &pPkA);
            free(pPkA->buf);
	     free(pPkA);  
	     Aq_count -=1;	         
   	}
  while(Aq_free_count>0)
   	{
             DeQueue(AqFRE_BUF, &pPkA);  
	      free(pPkA->buf);
	     free(pPkA);      
	     Aq_free_count -=1;	     
   	}   
       ClearQueue(AqPRE_BUF);
	ClearQueue(AqFRE_BUF);	 
#endif
  	}
}
/*-------------------------------------------------------*/
/*                Gobal Control                                                             */
/*-------------------------------------------------------*/
int GetStatusQ(void)
{

if(!Alarm_state)  //pre alarm
{
#ifndef Pre_Audio_Off 	
	if((!Q_LOCK)&&(status==FILLED)&&(!AQ_LOCK)&&(AU_status==FILLED))	
#else
      if((!Q_LOCK)&&(status==FILLED))	
#endif
	 return 1 ;
	else if(Q_LOCK) //QLOCK, RS 0417
		{
		  //printf("\n Find Q_LOCK =%d\n",Q_LOCK);
		  if(handle  != WRITE_HANDLE)
		      AShareMemWritePre2();
	     return 0;	
		}else
		  return 0;
}else
   return 1 ;
}

int GetStatusImg(void)
{
	
	
	return imageType;

}


/*Wait For Test*/
int SetImageType(int type)
{
   imageType = type;
   printf("\n imageType = %d\n",imageType );
   return 1;
   
}

/*---------------- Motion Detection Part--------------------------*/

void ClrAlarmAudio(int state) /*Wait For Test*/
{
   Alarm_Audio=state;
}

int GetAlarmAudio(void)
{
     next_time = time((time_t *) NULL);
     if((next_time - front_time) >= 5)
      Alarm_Audio=0;	 
	 //printf("\n Alarm_Audio =%d\n",Alarm_Audio);
	return Alarm_Audio;
}

int GetAlarmBState(void)
{
   	 	
	return Alarm_state;
}
/*-------------------------------------------------------------*/

#if 0
static int AV_Sync(char *buf, int length,double timestamp)	
{
       int i,Swapcnt=1;
	PRE_BUF_AU *pPk;
	double Vtimestamp;
	
	//Swapcnt =MIN(GOP_AUDIO,frame_rate);
	//Swapcnt -=1;	 	
	//for(i=0;i<(GOP_AUDIO-1);i++)
		
		
	for(i=0;i<Swapcnt;i++)
	//while(Swapcnt )
   	 {

		 /*Keep The VIdeo Last timestamp*/	
		//pPk = PeekQueueFront(qPRE_BUF);
		//Vtimestamp =pPk->timestamp;
	     
	     //PRE To FRE 
	     DeQueue(AqPRE_BUF, &pPk);
            EnQueue(AqFRE_BUF, pPk); 	        
	     Aq_free_count +=1;
	     Aq_count -=1;		    
   	  }  
	AU_status  = FILLED;	
	//PRE To PRE
	DeQueue(AqPRE_BUF, &pPk);
	
	if (pPk) {
	
		if (length > pPk->buf_size)
			{
                           	free(pPk->buf);	
				pPk->buf = (char *)malloc(length);	
				if (pPk->buf   == NULL) 
					{					 
					  Reset_Q();
					  return 1;
					}
									
			  }
                            pPk->buf_size = length;				
	                     pPk->timestamp=timestamp;
				memcpy(pPk->buf, buf, length);				
				EnQueue(AqPRE_BUF, pPk);	
				
				return 1;
		}
	else
		    {
					  
					  Reset_Q();
					  return 1;
			}
}


/*---------------------------------------------------------------*/

int AShareMemWritePre2() //Write out the remaining pre
{
        PRE_BUF *pPk;	
	 int i;
	 char filename[25];
	 FILE *pfd;	

	i= Pre_Write_CNT;
	
   while(q_count>0) 
   {	
       
	 DeQueue(qPRE_BUF, &pPk);
	 q_count -=1;	 
	 if (pPk) 	   	
       {
          
	   sprintf(filename,"/tmp/%d.jpg",i);   
	   pfd = fopen(filename,"wb");
	   if( pfd == NULL )
	   	{
                  Reset_Q(); //reset and skip frame
		  return 1;
	   	}
	   fwrite(pPk->buf, pPk->buf_size, 1, pfd); 
	   fclose(pfd);
	   shm_address->size[i]=pPk->buf_size; 
	   shm_address->state[i]=FILLED;    
	   EnQueue(qFRE_BUF, pPk);
	   q_free_count +=1;	 
	   i+=1;
       }
   	
   }
     
	handle  = WRITE_HANDLE  ;//Changing handle to writer
        status  = NOT_READY;    
	 return 1;
 
}
#endif



	
	   

