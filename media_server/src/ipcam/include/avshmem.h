#ifndef	_AVSHMEM_H
#define	_AVSHMEM_H
#if defined (__cplusplus)
extern "C" {
#endif

#define Error(Str)        FatalError(Str)
#define FatalError(Str)   fprintf(stderr, "%s\n", Str)
#define AVStreamNum 4
#define MaxAVFrameQueueSize 15
#define MinAVFrameQueueSize (5)
#define AudioFrameBufferSize  1024
#define VideoFrameBufferSize  (1024*256)

#define KEY_SHM_AV_CAPTURE  0xA000
#define START_VALUE         1000

#if 0
typedef struct VFrame_t {
  unsigned int serial;	/* frame serial number */
  unsigned int size;		/* frame size */
  unsigned int timestamp;	/* get frame time stamp */
//  int     ref_serial[FMT_MAX_NUM];	/* all frame serial record for reference */
  unsigned char ptr[VideoFrameBufferSize];	    /*  pointer for data ouput */	
} VFrame;    

typedef struct AVFrameRecord {
  unsigned int serial;	/* frame serial number */
  unsigned int size;		/* frame size */
  unsigned int timestamp;	/* get frame time stamp */
//    int     ref_serial[FMT_MAX_NUM];	/* all frame serial record for reference */
  unsigned char *ptr;	    /*  pointer for data ouput */	
} *AVFrame;    

typedef int ElementType;

typedef struct AVFrameQueueRecord {
 int Capacity;
 int Front;
 int Rear;	  	
 int Size;
 ElementType *Array;
 unsigned int frameType; /**< either AV_FRAME_TYPE_I_FRAME or AV_FRAME_TYPE_P_FRAME */	    	
 unsigned int width;	 /**< frame width */
 unsigned int height;	 /**< frame height */	
 unsigned int quality;	 /**< frame quality */
 unsigned int flags;	 /**< frame flags */
 AVFrame avframe;
} *AVFrameQueue;

int         AVFrameQueueIsEmpty(AVFrameQueue Q);
int         AVFrameQueueIsFull(AVFrameQueue Q);
AVFrameQueue  CreateAVFrameQueue(int MaxElements);
void        DisposeAVFrameQueue(AVFrameQueue Q);
void        AVFrameQueueMakeEmpty(AVFrameQueue Q);
void        AVFrameEnqueue(ElementType X, AVFrameQueue Q);
ElementType AVFrameQueueFront(AVFrameQueue Q);
ElementType AVFrameDequeue(AVFrameQueue Q);
#endif 

#if 0
typedef struct {
  unsigned int serial;	/* frame serial number */
  unsigned int size;		/* frame size */
  unsigned int timestamp;	/* get frame time stamp */
//    int     ref_serial[FMT_MAX_NUM];	/* all frame serial record for reference */
  unsigned char ptr[VideoFrameBufferSize];	    /*  pointer for data ouput */	
} AVFrame;    

//typedef struct AVFrameData_t {
typedef struct {
    int    flag;
    int    s32Cnt;		
    int    data[4];	
    char busy[4];	
//    AVFrameQueue  avstream[4];

//  unsigned int serial;	/* frame serial number */
//  unsigned int size;		/* frame size */
//  unsigned int timestamp;	/* get frame time stamp */
//    int     ref_serial[FMT_MAX_NUM];	/* all frame serial record for reference */
//    unsigned char ptr[VideoFrameBufferSize];	    /*  pointer for data ouput */	
    AVFrame avframe[4];
} AVFrameData;

extern int   shm_id;   
//extern AVFrameData *avframedata;		 
extern unsigned char *avframedata;		 
//extern AVFrame avframe[];
#endif

#if defined (__cplusplus)
}
#endif
#endif /* _SHMEM_H */

