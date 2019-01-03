#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "debug.h"
#include "customTime.h"
#include "configure.h"
#include "ezp-lib.h"
#include "serial.h"

#define USB_PATH "/tmp/mnt/USB"
#define SD_PATH "/tmp/mnt/SD"
#define MAX_SIZE_PER_HOURE 15 * 1024 * 1024

typedef enum{
    USB = 0,
    SD
}EXTERNAL_TYPE;

int mailFlag[] = {0, 0};
int check_time = 0;
int digital = 0;
char savedPath[BUF_LEN] = {0};

int check_size(const char * file_dir)
{
    struct statvfs buf;
    int total, used;

    _DEBUG_MSG("check_size [ %s ]\n",file_dir);

    if ( statvfs(file_dir,&buf) < 0 )
    {
        _DEBUG_MSG("statvfs fail!!\n");
        return 101;
    }
    used = (buf.f_blocks - buf.f_bfree);
    total = buf.f_blocks ;
    _DEBUG_MSG("check_size use : %d total : %d [ %d ]\n",used,total,( ( used * 100 ) / total ));
    return ( ( used * 100 ) / total );
}


int detect_storage(const char * file_dir)
{
    DIR *dir = NULL;

    if( file_dir == NULL ){ 
        printf("file_dir is NULL!!\n");
        return 0;
    }

    _DEBUG_MSG("detect_storage [ %s ]\n",file_dir);

    if ( ( dir = opendir(file_dir) ) == NULL){
        _DEBUG_MSG("Open dir fail!!\n");
        return 0;
    }

    closedir(dir);

    _DEBUG_MSG("Storage success!!\n");
    return 1;
}

void getExternalPath(char *path, int record){
    char *externalList[2] = {USB_PATH, SD_PATH};
    int enableList[2] = {get_conf_toInt("usb"), get_conf_toInt("sd")};
    int index = 0, size = 0;

    for(index = 0; index < 2; index++){
        if ( enableList[index] && detect_storage(externalList[index])){
            if ((size = check_size(externalList[index])) >= 95){
                if(mailFlag[index] == 0){
		    if(index == USB) system("/bin/sh /etc/init.d/send_mail USB Size_over &");
                    else if(index == SD) system("/bin/sh /etc/init.d/send_mail SD Size_over &");
                    mailFlag[index] = 1;
                }
            }else{
                if(mailFlag[index] == 1)
                {
		    if(index == USB) ezplib_replace_attr("alert_rule", 0, "usb", "0");
		    else if(index == SD) ezplib_replace_attr("alert_rule", 0, "sd", "0");
                    mailFlag[index] = 0;
                }
            }

            if(record || size < 95){
                strcpy(path, externalList[index]);
		break;
            }
        }
    }
}

int find_digital(char *dir, int record){
    struct dirent **namelist = NULL;
    int totalCount = 0, count = 1, finalNum = 0;
    char file_name[BUF_LEN] = {0};
    char num[16] = {0};
    char path[BUF_LEN] = {0};
    struct stat st;
    int deleted = record;

    totalCount = scandir( dir, &namelist, 0, alphasort);
    if (totalCount < 0 ){
        _DEBUG_MSG("Dir is empty!!\n");
        free(namelist);
    }else{
        while(count < totalCount)
        {
            strcpy(file_name, namelist[count]->d_name);
            sscanf(file_name, "%*[^-]-%[0-9]", num);
            if (strlen(num) == 10)
            {
                _DEBUG_MSG("Find File [ %s.txt ] !!\n",num);
                if(deleted)
                {
                    sprintf(path, "%s/%s", dir, namelist[count]->d_name);
                    remove(path);
    		    deleted = 0;
                }
                finalNum = MAXNO(atoi(num), finalNum);
            }
            free(namelist[count]);
            count++;
        }
        free(namelist);
	stat(file_name, &st);	
        _DEBUG_MSG("Last file [ %010d.txt ] (%u)!!\n", finalNum, st.st_mtime);
	if(getTimeBySec() - st.st_mtime > (get_conf_toInt("file_size") * 60)) finalNum++;
    }
    return finalNum;
}

int check_digital(char *path, int record){
    int now_time = time_number();
    char nowFile[128] = {0};
    struct stat st;
    sprintf(nowFile, "%s/%s-%010d.txt", path, get_conf("alias"), digital);
    stat(nowFile, &st);
    unsigned int size = st.st_size;
    
    _DEBUG_MSG("Fiel state : %s[%u]", nowFile, size);
    if ( check_time == 0 || now_time - check_time >= get_conf_toInt("file_size") || strcmp(path, savedPath) || size >= MAX_SIZE_PER_HOURE ){
	_DEBUG_MSG("New File!!\n");
        if (check_time != 0 && get_conf_toInt("ftp_enable")){
           char sendFTP[128] = {0};
           sprintf(sendFTP,"/bin/sh /etc/init.d/sendFTP.sh %s/ %s-%010d.txt &", path, get_conf("alias"), digital);
           system(sendFTP);
        }
	digital = MAXNO(find_digital(path, record), digital);
	strcpy(savedPath, path);
        check_time = now_time;
        return 1;
    } else {
	_DEBUG_MSG("Exist File!!\n");
	return 0;
    }
}

void saveDataToFile(char *data, int len){
    static FILE *fp = NULL;
    char fullPath[BUF_LEN] = {0};
    char *saveDir = get_conf("alias");
    int rewrite = 0;

    if(len <= 0)return;

    getExternalPath((char *)&fullPath, get_conf_toInt("record"));
   
    if(strlen(fullPath) <= 0) return;

    rewrite = (get_conf_toInt("record") && check_size(fullPath) >= 95) ? 1 : 0;

    sprintf(fullPath, "%s/%s", fullPath, saveDir);

    _DEBUG_MSG("PATH : %s", fullPath);
   
    if(fp == NULL || check_digital(fullPath, rewrite)){

        if(!detect_storage(fullPath)) mkdir(fullPath, 0777);
    
        sprintf(fullPath, "%s/%s-%010d.txt", fullPath, saveDir, digital);

        _DEBUG_MSG("File path : %s !!", fullPath);

 	if(fp != NULL) fclose(fp);

    	if((fp = fopen(fullPath, "a")) == NULL){
	    _DEBUG_MSG("Write file fail!!\n");
	    return;
    	}
    }
    fwrite(data, 1, len, fp);
}  
