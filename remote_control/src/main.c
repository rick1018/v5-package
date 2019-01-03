#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ezp-lib.h"
#include "tcpServer.h"
#include "thread.h"
#include "serial.h"
#include "configure.h"
#include "uart.h"
#include "debug.h"
#include "queue.h"
#include "sharedMemory.h"
#include "file.h"

int serialPort = -1;

extern pthread_mutex_t mutex;
void saveDataCount(int count){
    static int totalCount = 0;
    static FILE *fp = NULL;

    if(fp == NULL){
	char filePath[512] = {0};
        sprintf(filePath, "/tmp/%s-Rx", get_conf("alias"));
	fp = fopen(filePath, "w");
	if(fp < 0){
	    fp = NULL;
	    return;
	}
    }
    fseek(fp,0,SEEK_SET);
    totalCount += count;
    fprintf(fp,"%d\n", totalCount);
}

void setSessionStatus(int group, SESSION_STATUS status){
    char state[4] = {0};

    _DEBUG_MSG("start");

    sprintf(state, "%d", status);

    _setSessionStatus(group, state, -1);

    _DEBUG_MSG("end");
}

void state_init(){
    int group = 0;

    _DEBUG_MSG("start");

    group = get_conf_toInt("group");

    ezplib_replace_attr("als_status_rule", group, "enable", "1");

    if(get_conf("mode") != NULL){
	ezplib_replace_attr("als_status_rule", group, "mode", get_conf("mode"));
    }

    setSessionStatus(group, LOADING);

    _DEBUG_MSG("end");
}

void system_init(char * configure){

    _DEBUG_MSG("start");

    configure_init(configure);
    
    state_init();
    
    _DEBUG_MSG("end");
}

int open_serial(struct termios info){
    int con = -1;
    int serial_timer = 0;

    do{
        if((con = open_console(get_conf("device"))) > 0){
            tcgetattr(con, &info);
            if(con_setting(con, get_conf_toInt("baudrate"), get_conf_toInt("data_bits"), 
			get_conf_toInt("stop_bit"), get_conf_toInt("parity")) < 0){
        	_DEBUG_MSG("Serial setting error.");
		return -1;
	    }
        }else{
            if(serial_timer > SERIAL_TIMEOUT) return -2;
            sleep(1);
            serial_timer++;
            _DEBUG_MSG("Console Port Locking.. Waitting...");
        }
    }while(con <= 0);

    return con;
}

int mode_init(MODE_TYPE mode){
    int queueNum = 0;
    switch (mode){
        case TCP_SERVER:
            _DEBUG_MSG("Mode : TCP Server !!\n");
            openServerThread();
	    queueNum = 1;
            break;
        case TCP_CLIENT:
            _DEBUG_MSG("Mode : TCP Client !!");
            _DEBUG_MSG("Start Open socket !!");
            tcpClientThread(get_conf_toInt("id_time"));
	    queueNum = getMaxQueue();
            _DEBUG_MSG("Open socket End!!");
            break;
        case UDP_MODE:
            _DEBUG_MSG("Mode : UDP Mode!!");
            //createUdpMode();
            break;
        default:
            _DEBUG_MSG("Mode : ERROR !!");
            break;
    }
    return queueNum;
}

void dataPacking(char *data, char *shmadd, SHARE_TYPE type){
    char *keyWord[] = {get_conf("dp_limit_1"), "$GPRMC"};
    static char saveData[1024] = {0};
    char *tmpData = NULL, *endData = NULL;
    int flag = 0, offset = 0;

    if(strlen(keyWord[type]) <= 0 || strlen(data) <= 0) return;
    if((tmpData = strstr(data, keyWord[type])) == NULL && strlen(saveData) <= 0) return;

    if(tmpData){
        if ((endData = strstr(tmpData, "\n")) != NULL){
            offset = endData - tmpData;
            strncpy(saveData, tmpData, offset);
            flag = 1;
        }else
            strcpy(saveData, tmpData);
    }else if((endData = strstr(data, "\n")) != NULL) {
        offset = endData - data;
        strncat(saveData, data, offset);
        flag = 1;
    }else
        strcat(saveData, data);
    
    if(flag){
        flag = 0;
        _DEBUG_MSG("Save DATA : %s\n",saveData);
        shmSetInfo((char *)&saveData, shmadd, type);
        memset(&saveData, 0, sizeof(saveData));
    }
    return;
}

int main( int argc, char **argv ){
    int selectTimeout = 0;
    struct termios info;

    struct timeval selectTime;
    fd_set fds;

    unsigned char dataBuff[BUF_LEN] = {0};
    int dataLen = 0;

    int queueIndex = 0;
    int maxQueue = 0;
    char *shmadd[2] = {0};

    _DEBUG_MSG("UART Start...");

    if(argv[1] == NULL) {
        _DEBUG_MSG("No such the configure.");
        return 0;
    }

    system_init(argv[1]);

    if(get_conf("device") == NULL){
        _DEBUG_MSG("No such the serial port.");
        return 0;
    }

    if((serialPort = open_serial(info)) < 0) {
        _DEBUG_MSG("Can't open serial port.");
	return 0;
    }

    _DEBUG_MSG("Serial port open success [ %d ].", serialPort);
    thread_init();

    if(get_conf_toInt("from_gps")){
        shmadd[GPS] = init_share_memory(WRITE, GPS);
    }

    if(get_conf_toInt("dp_enable")){
        shmadd[UART] = init_share_memory(WRITE, UART);
    }

    maxQueue = mode_init(get_conf_toInt("mode"));    
    

    setSessionStatus(get_conf_toInt("group"), LISTEN);

    for(;;){
        selectTime.tv_sec = 30;
        selectTime.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(serialPort, &fds);

 	if((selectTimeout = select(serialPort + 1, &fds, NULL, NULL, (struct timeval *)&selectTime)) <= 0){
	    continue;
	}

	if(FD_ISSET(serialPort, &fds)) {
	    memset(dataBuff, 0, sizeof(dataBuff));
            if ((dataLen = read(serialPort, dataBuff, sizeof(dataBuff))) <= 0)
                continue;

	    printf("%s(%d) %s\n", __func__, __LINE__ , dataBuff);
	    pthread_mutex_lock(&mutex);
	    for(queueIndex = 0; queueIndex < maxQueue; queueIndex++){	    
	        queueAdd(queueIndex, dataBuff, dataLen);
 	    }
	    pthread_mutex_unlock(&mutex);

    	    if(shmadd[GPS] != NULL && get_conf_toInt("from_gps")){
		dataPacking(dataBuff, shmadd[GPS], GPS);
    	    }

 	    if(shmadd[UART] != NULL && get_conf_toInt("dp_enable")){
		dataPacking(dataBuff, shmadd[UART], UART);
    	    }

	    saveDataCount(dataLen); 

	    if(get_conf_toInt("file_mode")) saveDataToFile(dataBuff, dataLen);

	    usleep(500);
	}
    }

    con_close(serialPort, &info);
    _DEBUG_MSG("Close..");
    return 1;
}
