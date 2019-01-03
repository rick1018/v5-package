#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h> 

#include "debug.h"
#include "uart.h"
#include "socket.h"
#include "tcpServer.h"

// ------ UART Setting Start ------
#define BAUDRATE 115200
#define DATA_BIT 8
#define STOP_BIT 1
#define PARITY 0 //none
// ------ UART Setting End ------

// ------ Server Mode Start ------
#define S_SERVER_PORT 6154
// ------ Server Mode End ------

// ------ Client Mode Start ------
#define C_SERVER_IP "192.168.0.101"
#define C_SERVER_PORT 6154 
// ------ Client Mode End ------

typedef enum {
    TCP_SERVER = 0,
    TCP_CLIENT,
    UDP_MODE
}MODE_TYPE;

#define BUF_LEN 1024
#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)
#define SERIAL_TIMEOUT 30


int open_serial(char *device, struct termios info){
    int con = -1;
    int serial_timer = 0;

    do{
        if((con = open_console(device)) > 0){
            tcgetattr(con, &info);
            if(con_setting(con, BAUDRATE, DATA_BIT,
                        STOP_BIT, PARITY) < 0){
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

int socket_init(MODE_TYPE mode){
    int res = 0;
    switch (mode){
        case TCP_SERVER:
            _DEBUG_MSG("Mode : TCP Server !!");
	    res = open_server(S_SERVER_PORT);
            break;
        case TCP_CLIENT:
            _DEBUG_MSG("Mode : TCP Client !!");
	    res = open_tcp_client(C_SERVER_IP, C_SERVER_PORT, 0);
            break;
        default:
            _DEBUG_MSG("Mode : ERROR !!");
    	    res = -1;
            break;
    }
    return res;
}

int read_data(int fd, int socket)
{
    char buf[BUF_LEN];
    int res, i;

    memset(buf,0,sizeof(buf));

    res = read(fd, &buf, sizeof(buf));
    if( res == 0 )  return 1;
    if( res < 0 ) return -2;

    if(socket > 0){
        if((res = write(socket, buf, res)) < 0)return -1;
    }

    if(res == 4 && buf[0] == 87 && buf[1] == 171 && buf[2] == 130 && buf[3] == 163) return 1;
    printf("[%d]",res);
    for(i = 0; i < res;i++){
        printf("%x-",buf[i]);
    }
    printf("\n");
    return 1;
}

int main( int argc, char **argv )
{
    struct timeval selectTime;
    int con, maxfd, socket, mode;
    int client = 0;
    struct termios info;
    fd_set fds;

    _DEBUG_MSG("====== Start ======");

    if(!argv[1]){
	_DEBUG_MSG("Input Error!!");
	exit(0);
    }

    mode = (argv[2]) ? atoi(argv[2]) : -1;
    if(mode < 0 || mode > 1){
	_DEBUG_MSG("Socket mode is invalid !! default : Server Mode");
	mode = 0;
    } 

    _DEBUG_MSG("====== Loop Start ======");

    while(1){
	selectTime.tv_sec = 3;
        selectTime.tv_usec = 0;

        FD_ZERO(&fds);
        maxfd = 0;

        if(con <= 0 && (con = open_serial(argv[1], info)) < 0 ){
	    _DEBUG_MSG("Open Serial Error!!");
        } else {
            FD_SET(con, &fds);
            maxfd = MAXNO(con, maxfd);
	}

	if(socket <= 0 && (socket = socket_init(mode)) < 0){
	    _DEBUG_MSG("Open Socket Error!!");
	}

	if(socket > 0){
            FD_SET(socket, &fds);
            maxfd = MAXNO(socket, maxfd);
	}

 	if(mode == TCP_SERVER && client > 0){
            FD_SET(client, &fds);
            maxfd = MAXNO(client, maxfd);
	}

        if(select(maxfd+1, &fds, NULL, NULL, (struct timeval *)&selectTime) <= 0){
	    continue;
	}

    	if(FD_ISSET(con, &fds)) {
	    if(mode == TCP_SERVER && (read_data(con, client)) < 0) {
		close(client);
		client = -1;
	    }

	    if(mode == TCP_CLIENT && (read_data(con, socket)) < 0) {
		close(socket);
		socket = -1;
	    }
    	}
    
    	if(FD_ISSET(socket, &fds)) {
	    if(mode == TCP_SERVER){ 
	    	if(client > 0){
		    close(client);
	    	}
	        client = user_connect(socket);
	    }

	    if(mode == TCP_CLIENT && (read_data(socket, con)) < 0) {
		close(socket);
		socket = -1;
	    }
    	}

    	if(FD_ISSET(client, &fds) && (read_data(client, con)) < 0){
	    close(client);
	    client = -1;
    	}
    }
    con_close(con,&info);
    _DEBUG_MSG("Close..");
    return 0;
}
