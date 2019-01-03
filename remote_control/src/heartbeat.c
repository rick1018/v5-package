#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <termios.h>

#include "debug.h"
#include "configure.h"
#include "ezp-lib.h"
#include "customTime.h"
#include "sharedMemory.h"

#define KILL_ALL_USER "{\"wifi\":\"F\"}"
#define HEARTBEAT_LEN 1024
#define BUF_LEN 1024
#define PORT_COUNT 5
#define LINK_UP 0
#define LINK_DOWN 1

char *portName[] = {"WAN", "LAN1", "LAN2", "LAN3", "LAN4"};
char *linkStatus[] = {"Link-up", "Link-down"};
static char *hbMessage = NULL;
char imei[32] = {0} ,imsi[32] = {0};
char *shmadd[2] = {0};

typedef enum {
    IMEI = 0,
    IMSI
}AT_INFO;

double transferToGoogle(double gpsLocate)
{
    int integer = gpsLocate / 100;
    double points = gpsLocate - (integer * 100);
    double result = integer + (points/60);

    return result;
}

void get_port_status(char *msg)
{
    int count = 0, statusIndex = 0;
    char portInfo[PORT_COUNT][32] = {{0}};
    FILE *fp = NULL;
    char buf[BUF_LEN]={0};

    fp = popen("/usr/sbin/switch phy-port","r");
    if(fp == NULL){
        _DEBUG_MSG("Run \"switch phy-port\" fail!!");
        return;
    }
    fgets(buf, sizeof (buf),fp);

    sscanf(buf, "%[0-9a-f]:%[0-9a-f]:%[0-9a-f]:%[0-9a-f]:%[0-9a-f]",
 	  portInfo[0], portInfo[1], portInfo[2], portInfo[3], portInfo[4]);
    pclose(fp);

    for(count = 0;count < PORT_COUNT;count++)
    {
        if(!strcmp(portInfo[count], "786d") || !strcmp(portInfo[count], "7869"))
            statusIndex = LINK_UP;
        else
            statusIndex = LINK_DOWN;

        sprintf(msg, "%s,\"%s\":\"%s\"", msg, portName[count], linkStatus[statusIndex]);
    }

    return;
}

int checkNumber(char *str){
    int i;

    if(strlen(str) <= 0) return 0;
    for (i = 0;i < strlen(str);i++){
        if (!isdigit(str[i]))return 0;
    }
    return 1;
}

void getATCommand(char *buff, AT_INFO type)
{
    FILE *fp = NULL;
    char buf[BUF_LEN]={0};
    char command[BUF_LEN]={0};

    if(buff == NULL || checkNumber(buff)) return;

    if(type == IMEI)
        sprintf(command,"SCRIPT=\"AT+GSN\" ezpcom -d /dev/ttyACM1 -s /etc/chatscripts/script.gcom");
    else if(type == IMSI)
        sprintf(command,"SCRIPT=\"AT+CIMI\" ezpcom -d /dev/ttyACM1 -s /etc/chatscripts/script.gcom");
   
    fp = popen(command,"r");
    if(fp == NULL){
    	strcpy(buff, "NULL");
	return;
    }

    fgets(buf, sizeof (buf),fp);
    fgets(buf, sizeof (buf),fp);

    if(strlen(buf) <= 0) goto back;
    printf("%s(%d) %s [ %d ]\n", __func__, __LINE__, buf, checkNumber(buf));
    buf[(strlen(buf) - 1)]='\0';
    if(checkNumber(&buf)){
    	strcpy(buff, buf);
    	buff[(strlen(buff) - 1)]='\0';
    }
    printf("%s(%d) %s\n", __func__, __LINE__,buff);
back:
    pclose(fp);
}

void get_AT_number(char *msg){

    printf("%s(%d)\n", __func__, __LINE__);
    if(strlen(imei) != 15 || !checkNumber(imei))getATCommand(imei, IMEI);
    if(strlen(imsi) < 15 || !checkNumber(imsi))getATCommand(imsi, IMSI);
    printf("%s(%d)\n", __func__, __LINE__);
    if(!strcmp(imei, imsi)){
    printf("%s(%d)\n", __func__, __LINE__);
        getATCommand(imei, IMEI);
        getATCommand(imsi, IMSI);
    }
    printf("%s(%d)\n", __func__, __LINE__);

    sprintf(msg, "%s,\"IMEI\":\"%s\",\"IMSI\":\"%s\"", msg, imei, imsi);
}

void getUARTInfo(char *msg){
    char *dpInfo = shmGetInfo(shmadd[UART], UART);
    sprintf(msg, "%s,\"UART\":\"%s\"", msg, (dpInfo == NULL) ? "" : dpInfo);
    if(dpInfo) free(dpInfo);
}

void getGPSInfo(char *msg){
    GPRMC_INFO *gpsInfo = NULL;
    if((gpsInfo = (GPRMC_INFO *)shmGetInfo(shmadd[GPS], GPS)) != NULL){
    	sprintf(msg, "%s,\"LONG\":\"%lf\",\"LAT\":\"%lf\",\"SP\":\"%lf\",\"ANGLE\":\"%lf\"",
		msg, transferToGoogle(gpsInfo->longtitude),
		transferToGoogle(gpsInfo->latitude), gpsInfo->speed,
                gpsInfo->azimuth_angle);
        free(gpsInfo);
    } else {
    	sprintf(msg, "%s,\"LONG\":\"0\",\"LAT\":\"0\",\"SP\":\"0\",\"ANGLE\":\"0\"", msg);
    }
}

void initHeartbeat(){
    char mac[512] = {0}, ver[512] = {0};
    char confVer[512] = {0}, model[512] = {0};

    hbMessage = malloc(sizeof(char) * HEARTBEAT_LEN);
    if(hbMessage == NULL) return;

    memset(hbMessage, 0, sizeof(char) * HEARTBEAT_LEN);

    if(get_conf_toInt("from_gps")){
	shmadd[GPS] = init_share_memory(READ, GPS);
    }

    if(get_conf_toInt("dp_enable")){
       shmadd[UART] = init_share_memory(READ, UART);
    }

    getATCommand(imei, IMEI);

    getATCommand(imsi, IMSI);

    ezplib_get_attr_val("als_id_rule", get_conf_toInt("group"), "mac", mac, sizeof(mac),
                EZPLIB_USE_CLI);

    ezplib_get_attr_val("als_config_rule", 0, "version", confVer, sizeof(confVer),
                EZPLIB_USE_CLI);

    ezplib_get_rule("fw_version", 0, ver, sizeof(ver));

    ezplib_get_rule("model", 0, model, sizeof(model));

    sprintf(hbMessage, "{\"id\":\"%s\",\"Firmware Ver\":\"%s\",\"ConVer\":\"%s\",\"Model\":\"%s\"",
	    mac, ver, confVer, model);
}

int sendHeartbeat(int face_fd){
    char res[128] = {0};
    char nowTime[32] = {0};
    char msg[1024] = {0};

    if(hbMessage == NULL) return -1;
    strcpy(msg, hbMessage);

    get_port_status((char *)&msg);

    get_AT_number((char *)&msg);

    getNowTime((char *)&nowTime);
    
    if(get_conf_toInt("from_gps")){
	getGPSInfo((char *)&msg);
    }

    if(get_conf_toInt("dp_enable")){
	getUARTInfo((char *)&msg);
    }

    sprintf(msg, "%s,\"Time\":\"%s\",\"M\":\"0\",\"F\":\"0\"}"
            , msg, nowTime); 

    _DEBUG_MSG("%s", msg);
    printf("%s(%d) : %s\n\n",__func__, __LINE__, msg);
    if(write(face_fd, msg, strlen(msg)) > 0){
	if(recv(face_fd, res, sizeof(res), 0) > 0){
	    if(!strncmp(res, KILL_ALL_USER, strlen(KILL_ALL_USER))){
                system("/bin/sh /etc/chilli/kickall.sh");
            }
	}
    }
    return 1;  
}
