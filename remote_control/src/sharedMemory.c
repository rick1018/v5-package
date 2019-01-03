#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "sharedMemory.h"
#include "debug.h"

int shareSize[] = {SHARE_UART_SIZE, SHARE_GPS_SIZE};

char* init_share_memory(SHARE_MODE mode, SHARE_TYPE type){
    key_t key;
    char *shareFile[] = {SHARE_MEMORY_UART_FILE, SHARE_MEMORY_GPS_FILE};
    int shareKey[] = {IPC_UART_KEY, IPC_GPS_KEY};
    int shareMemoryId = 0;
    char *shareMemoryaddress = NULL;
    struct stat status;

    if(mode == WRITE && stat(shareFile[type], &status) == -1){
        open(shareFile[type], O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    }

    key = ((stat(shareFile[type], &status)) == -1 ) ? shareKey[type] : ftok(shareFile[type], shareKey[type]);
    if(key == -1)
    {
        _DEBUG_MSG("Share memory file not found!!\n");
        return NULL;
    }

    if(mode == WRITE)
        shareMemoryId = shmget(key, shareSize[type], SHM_R|SHM_W|IPC_CREAT);
    else
        shareMemoryId = shmget(key, shareSize[type], SHM_R);

    if(shareMemoryId < 0)
    {
        _DEBUG_MSG("Create share memory error!!\n");
        return NULL;
    }
    shareMemoryaddress = shmat(shareMemoryId, NULL, 0);
    if(shareMemoryaddress < 0)
    {
        _DEBUG_MSG("Get share memory address error!!\n");
        return NULL;
    }
    _DEBUG_MSG("Get share memory Success!!\n");

    if(mode == WRITE)
        memset(shareMemoryaddress, 0, shareSize[type]);
    return shareMemoryaddress;
}

char* shmGetInfo(char *shmadd, SHARE_TYPE type){
    char *info = calloc(sizeof(char), shareSize[type]);
    if(info == NULL) return NULL;
    memcpy(info, shmadd, shareSize[type]);
    info[strlen(info) - 1] = '\0';
    return info;
}

void shmSetInfo(char *data, char *shmadd, SHARE_TYPE type){
    _DEBUG_MSG("Set to Share memory!! \n");
    if(type == GPS){
    	GPRMC_INFO *gps_info = calloc(1, sizeof(GPRMC_INFO));
   	sscanf(data, "$GPRMC,%[^,],%c,%lf,%c,%lf,%c,%lf,%lf,%[^,]",
        	gps_info->UTCTime, &(gps_info->status),&(gps_info->latitude),
        	&(gps_info->latitudeLocate), &(gps_info->longtitude),
        	&(gps_info->longtitudeLocate), &(gps_info->speed),
        	&(gps_info->azimuth_angle), gps_info->UTCDate);
    	memcpy(shmadd, gps_info, shareSize[type]);
    } else memcpy(shmadd, data, shareSize[type]);
}
