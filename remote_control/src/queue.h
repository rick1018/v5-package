#ifndef __QUEUE__
#define __QUEUE__

#define MAX_QUEUE_DATA 1024

int getMaxQueue();
unsigned char* queuePeek(int);
int queueIsFull(int);
int queueIsEmpty(int);
int queueSize(int);
int queueAdd(int, unsigned char *, int);
void queueGetData(int, unsigned char *, int *);
#endif
