#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"
#include "debug.h"
#define MAX_COUNT 4
#define MAX_QUEUE 512

unsigned char circularQueue[MAX_COUNT][MAX_QUEUE][MAX_QUEUE_DATA] = {{{0}}};
unsigned int queueDataLen[MAX_COUNT][MAX_QUEUE] = {{0}};
int frontArray[MAX_COUNT] = {-1};
int rearArray[MAX_COUNT] = {-1};
int itemCountArray[MAX_COUNT] = {0};
int flag = 0;

int getMaxQueue(){
    return MAX_COUNT;
}

unsigned char* queuePeek(int index){
   return (unsigned char*) &circularQueue[index][frontArray[index]];
}

int queueIsFull(int index){
    return rearArray[index] == MAX_QUEUE && frontArray[index] == 0;
}

int queueIsEmpty(int index){
    return frontArray[index] == rearArray[index];
}

int queueSize(int index){
    return itemCountArray[index];
}

void queueGetData(int index, unsigned char *data, int *len) {

    if(queueIsEmpty(index) && flag == 0){
	if(index == 0) _DEBUG_MSG("Queue[%d] is empty.", index);
	return;
    }

    frontArray[index] = (frontArray[index] + 1) % MAX_QUEUE;

    if(data != NULL){
        memset(data, 0, sizeof(char) * MAX_QUEUE_DATA);
        memcpy(data, circularQueue[index][frontArray[index]], MAX_QUEUE);
	if(len) *len = queueDataLen[index][frontArray[index]];
    }

    itemCountArray[index]--;

    memset(circularQueue[index][frontArray[index]], 0, MAX_QUEUE_DATA);

    if (frontArray[index] == rearArray[index]) flag = 0;
}

int queueAdd(int index, unsigned char *data, int len) {
    if(queueIsFull(index) && flag == 1 || rearArray[index] ==  MAX_QUEUE - 1 && frontArray[index] == -1){
	if(index == 0) _DEBUG_MSG("Queue[%d] is full and replace data.", index);
	queueGetData(index, NULL, NULL);
    }

    rearArray[index] = (rearArray[index] + 1 ) % MAX_QUEUE;

    memcpy(circularQueue[index][rearArray[index]], data, MAX_QUEUE);
    queueDataLen[index][rearArray[index]] = len;

    itemCountArray[index]++;

    if(frontArray[index] == rearArray[index]) flag = 1;

    return 1;
}

