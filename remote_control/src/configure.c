#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

#include "serial.h"
#include "debug.h"
#include "configure.h"
#include "ezp-lib.h"

static struct conf *list = NULL;

int get_conf_toInt(char *str){
    return (get_conf(str) == NULL ) ? 0 : atoi(get_conf(str));
}

char *get_conf(char *str){
    struct conf *now = NULL;

    if (list == NULL)
        return NULL;

    now = list;

    while(now != NULL)
    {
        if(!strcmp(now->name, str)){
            return now->data;
        }
        now = now->next;
    }
    return NULL;
}

void configure_init(char *file){
    FILE *fp = NULL;
    struct conf *new = NULL;
    char buff[BUF_LEN] = {0}, name[128] = {0}, data[128] = {0};

    _DEBUG_MSG("configure init()");

    fp = fopen( file, "r");

    if( fp <= 0 )
    {
        _DEBUG_MSG("Can't load configure[ %s ]",file);
    }

    
    while (fgets(buff, sizeof(buff), fp) != NULL){

        if(!strncmp(buff, "#", 1) || strlen(buff) == strcspn(buff, "="))
            continue;

        memset(name, 0 , sizeof(name));
        memset(data, 0 , sizeof(data));
        new = malloc(sizeof(struct conf)); 
        memset(new, 0 , sizeof(new));
        
        sscanf(buff, "%[^=]=%s", name, data);

        _DEBUG_MSG("Get name:%s value:%s\n", name, data);
        strcpy(new->name, name);
        strcpy(new->data, data);

        if(list == NULL){
            new->next = NULL;            
        }else{
            new->next = list;
        }
        list = new;
    }
    fclose(fp);
}

void _setSessionStatus(int group, char *status, int user){
    int sessionIndex = 0;

    _DEBUG_MSG("start");

    for(sessionIndex = 0; sessionIndex <= MAX_SESSION; sessionIndex++){
	if(user < 0 || user == sessionIndex){
            char attr[ BUF_LEN / 2] = {0};
            sprintf(attr, "isp%d", sessionIndex);
            ezplib_replace_attr("als_status_rule", group, attr, status);
            _DEBUG_MSG("set %s to %s", attr, status);
	}
    }
    _DEBUG_MSG("end");
}

