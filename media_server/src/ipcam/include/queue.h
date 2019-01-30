/*
 *
 *
 *  Implements Queues and CountQueues in MACROs.
 *
 *  The Queue operations are:
 *
 *    QUEUE(t) q;                    declare q as a queue of type t
 *    ClearQueue(q);		     initialize queue to empty
 *    EnQueue(q, pElement);	     enqueue an element
 *    Queue(q, pElement);	     (same as EnQueue())
 *    pElement = PeekQueueFront(q);  peek at front queue element
 *    pElement = PeekQueueBack(q);   peek at back queue element
 *    AddQueueFront(q, pElement);    add an element to front of queue
 *    DiscardQueueFront(q);	     discard front queue element
 *    DeQueue(q, &pElement);	     get (& remove from queue) front
 *				      queue element
 *
 *  The CountQueue operations are:
 *    CQUEUE(t) cq;                    declare cq as a CoutntQueue of type t
 *    ClearCQueue(cq);		       initialize queue to empty
 *    EnCQueue(cq, pElement);	       enqueue an element
 *    CQueue(cq, pElement);	       (same as EnQueue())
 *    pElement = PeekCQueueFront(cq);  peek at front queue element
 *    pElement = PeekCQueueBack(cq);   peek at back queue element
 *    AddCQueueFront(cq, pElement);    add an element to front of queue
 *    DiscardCQueueFront(cq);	       discard front queue element
 *    DeCQueue(cq, &pElement);	       get (& remove from queue) front
 *				        queue element
 *    nCnt = CQueueCnt(cq);	       # elements on CountQueue
 * 
 *  (pElement == NO_ENTRY) after PeekQueueFront() or DeQueue()
 *  implies the queue was empty.
 *
 *  See comments before each MACRO definition for a more complete
 *  description of each one.
 *
 *  In order for a typed structure to be eligible for use in
 *  a queue, it MUST include the pointer pNext defined as
 *  a pointer to the structure it belongs to.
 *
 *
 *  Example:
 *  
 *    typedef struct FOO {
 *      struct FOO *pNext;
 *      DWORD dwFoo;
 *    } FOO;
 *  
 *    FOO aFooArray[20];
 *  
 *    QUEUE(FOO) qFoo;
 *    QUEUE(FOO) qFooBar;
 *  
 *    void FooBar(void)
 *    {
 *      int i;
 *      FOO *pThisFoo;
 *      
 *      ClearQueue(qFoo);
 *      ClearQueue(qFooBar);
 *      for (i = 0; i < 20; i++) {
 *        aFooArray[i].dwFoo = i;
 *        Queue(qFoo, &(aFooArray[i]));
 *      }
 *  
 *      while (1) {
 *        pThisFoo = PeekQueueFront(qFoo);
 *        if (pThisFoo == NO_ENTRY)
 *          break;
 *        DiscardQueueFront(qFoo);
 *        pThisFoo->dwFoo += 100;
 *        Queue(qFooBar, pThisFoo);
 *      }
 *    }
 *
 *
 *
 */

#ifndef QUEUE_H__
#define QUEUE_H__

#ifndef NO_ENTRY
#  define NO_ENTRY ((void *) 0)
#endif


/*---------------------------------------------------------------------------
 *  TYPEDEFS
 */

#define QUEUE(t) 				\
   struct {					\
     t *pFront;		/* first out	*/	\
     t *pBack;		/* last in	*/	\
   }

#define CQUEUE(t) 				\
   struct {					\
     QUEUE(t) q;	/* the queue	*/	\
     int nCnt;		/* # elements	*/	\
   }




/*---------------------------------------------------------------------------
 *  MACROs
 */

/*
 * Clear a Queue, any contents of the queue are lost!
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 */

#define ClearQueue(q) do {					\
			(q).pFront = NO_ENTRY;			\
			(q).pBack = NO_ENTRY;			\
		     } while (0)

#define ClearCQueue(cq) do {					\
			ClearQueue((cq).q);			\
			(cq).nCnt = 0;				\
		     } while (0)
/*
 * add to the back of a Queue
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *		pe	pointer to element to add
 */

#define EnQueue(q, pe) Queue((q), (pe))
#define Queue(q, pe) do {					\
			/*ASSERT(NO_ENTRY != (pe));*/ 		\
			if ((q).pFront == NO_ENTRY) 		\
			  (q).pFront = (pe);			\
			else 					\
			  ((q).pBack)->pNext = (pe);		\
			(q).pBack = (pe);			\
			(pe)->pNext = NO_ENTRY; 		\
		     } while (0)

#define EnCQueue(cq, pe) CQueue((cq), (pe))
#define CQueue(cq, pe) do {					\
			Queue((cq).q, (pe));			\
			(cq).nCnt++;				\
		     } while (0)
/*
 * add to the front of a Queue
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *		pe	pointer to element to add
 */
#define AddQueueFront(q, pe) do					\
		{						\
		  /*ASSERT(NO_ENTRY != (pe));*/			\
		  if ((q).pFront == NO_ENTRY)			\
		    (q).pBack = (pe);				\
		  (pe)->pNext = (q).pFront;			\
		  (q).pFront = (pe);				\
		  QCNT_ADD;					\
		} while (0)

#define AddCQueueFront(cq, pe) do				\
		{						\
		  AddQueueFront((cq).q, (pe));			\
		  (cq).nCnt++;					\
		} while (0)



/*
 * peek at the front element of the queue
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *
 */
#define PeekQueueFront(q) ((q).pFront)

#define PeekCQueueFront(cq) PeekQueueFront((cq).q)


/*
 * peek at the back element of the queue
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *
 */
#define PeekQueueBack(q) ((q).pBack)

#define PeekCQueueBack(cq) PeekQueueBack((cq).q)



/*
 * discard the front element of the queue, the element is
 * LOST unless a PeekQueue() is done 1st to get it.
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *
 */

#define DiscardQueueFront(q)					\
		   do {						\
		     if ((q).pFront != NO_ENTRY) { 		\
		       (q).pFront = ((q).pFront)->pNext;	\
		       if ((q).pFront == NO_ENTRY) 		\
			 (q).pBack = NO_ENTRY;     		\
		     }						\
		   } while (0)

#define DiscardCQueueFront(cq)					\
		   do {						\
		     if ((cq).q.pFront != NO_ENTRY) { 		\
		       (cq).nCnt--;				\
		     }						\
		     DiscardQueueFront((cq).q);			\
		   } while (0)



/*
 * remove and return the front element of the queue
 *
 * parms:	q	a structure of type QUEUE
 *   or 	cq	a structure of type CQUEUE
 *		ppe	pointer to pointer to element
 *
 */
#define DeQueue(q, ppe) 					\
		   do {						\
		     *(ppe) = PeekQueueFront((q));		\
		     DiscardQueueFront((q));			\
		   } while (0)

#define DeCQueue(cq, ppe) 					\
		   do {						\
		     *(ppe) = PeekCQueueFront((cq));		\
		     DiscardCQueueFront((cq));			\
		   } while (0)



/*
 * return # elements in the queue
 */
#define CQueueCnt(cq) ((cq).nCnt)



#endif
