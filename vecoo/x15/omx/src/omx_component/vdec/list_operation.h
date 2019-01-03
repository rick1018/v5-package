#ifndef LIST_OPERATION_H
#define LIST_OPERATION_H
#define MAX_QELEMS_NUM 18

typedef struct __TimeStampMgmt TSMgmt,*pTSMgmt;
struct __TimeStampMgmt
{
	OMX_TICKS   timeStamp;
	OMX_U32     picId;
	int    used;
	pTSMgmt     prev;
	pTSMgmt     next;
};

//buffer list for buffer management
typedef struct __BufferMgmt BufMgmt,*pBufMgmt;
struct __BufferMgmt
{
	OMX_U8   *outBuf;
	unsigned int  busAddr;
	int used;
	pBufMgmt  prev;
	pBufMgmt  next;
};

#define IM_OMX_FindStructAddr(list,Node)			\
do{										            \
	int i = 0;                                     \
	for( i = 0; i < 30; i++)						\
	{									            \
		if(pIMVDec->list##Array[i].used == 0)			    \
		{								            \
			pIMVDec->list##Array[i].used = 1;				\
			break;							        \
		}								            \
	}									            \
	Node = (i == 30) ? NULL :  &(pIMVDec->list##Array[i]);	\
}while(0)

//the macro defines how to delete a node from list
#define IM_OMX_ListNodeDelete(del, list) 		    \
	do{									            \
		if(del##Node->next == NULL)					\
		{								            \
			pIMVDec->list##Tail = del##Node->prev;			\
		}								            \
		if(del##Node->prev == NULL)					\
		{								            \
			pIMVDec->list##Head = del##Node->next;		    \
		}								            \
		if(del##Node->prev)						    \
		{								            \
			del##Node->prev->next = del##Node->next;\
			pIMVDec->list##Tail->next = NULL;				\
		}								            \
		if(del##Node->next)						    \
		{								            \
			del##Node->next->prev =  del##Node->prev;\
			pIMVDec->list##Head->prev = NULL;				\
		}								            \
		del##Node->used = 0;						\
	}while(0)

//insert node 
#define IM_OMX_InsertListNodeAtTail(insert,list) 	\
	do{												\
		insert##Node->next = NULL;					\
		insert##Node->prev = pIMVDec->list##Tail;			\
		if(pIMVDec->list##Tail == NULL)						\
		{											\
			pIMVDec->list##Head = insert##Node; \
			pIMVDec->list##Tail = pIMVDec->list##Head;  \
		}											\
		else										\
		{											\
			pIMVDec->list##Tail->next = insert##Node;		\
			pIMVDec->list##Tail = insert##Node;				\
		}											\
	}while(0)

#endif

