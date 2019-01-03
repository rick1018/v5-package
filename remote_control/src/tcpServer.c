#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "debug.h"
#include "customTime.h"

#define SOCKTIMEOUT 5
#define BUF_LEN 1024
#define MAX_SESSION 5

int user_connect (int server)
{
    int addr_len = sizeof(struct sockaddr_in);

    struct sockaddr_in caddr;
    struct timeval tv;

    tv.tv_sec = SOCKTIMEOUT;
    tv.tv_usec = 0;

    int clientPid = 0;

    clientPid = accept( server, (struct sockaddr*)&caddr, (socklen_t *)&addr_len);
    if (!strcmp(inet_ntoa(caddr.sin_addr), "0.0.0.0")) return -1;

    _DEBUG_MSG("User : %d cnnect from : %s : %d \n", clientPid, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
    if(setsockopt(clientPid, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        _DEBUG_MSG("set SO_RCVTIMEO ERROR\n");
    if(setsockopt(clientPid, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        _DEBUG_MSG("set SO_SNDTIMEO ERROR\n");
    return clientPid;
}

int open_server (unsigned short int server_port)
{
    struct sockaddr_in saddr;
    char buf[BUF_LEN] = {0};
    int server_pid = -1;
    int optval = 1;

    _DEBUG_MSG("TCP Server Starting.....\n");
    server_pid = socket( AF_INET, SOCK_STREAM, 0 );

    if(server_pid < 0)
    {
        _DEBUG_MSG("TCP Server start Error!!\n");
        return -1;
    }

    _DEBUG_MSG("TCP Server start Success!! [PID : %d ]\n",server_pid);

    memset( &saddr , 0 , sizeof(saddr));
    memset( buf , 0 , sizeof(buf));

    _DEBUG_MSG("TCP Server setting....\n");

    saddr.sin_family = AF_INET;  
    saddr.sin_port = htons( server_port );  
    saddr.sin_addr.s_addr = htonl( INADDR_ANY );

    if(setsockopt( server_pid, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 )
        _DEBUG_MSG("Error : setsockopt");

    while (bind( server_pid , (struct sockaddr *)&saddr , 16 ) < 0)
    {
        _DEBUG_MSG("TCP Server set fail\n");
        _DEBUG_MSG("Tcp Server Port[ %d ] locking.. Waitting... \n", server_port);
        sleep(1);
    }

    _DEBUG_MSG("TCP Server set Success [Port : %d ]\n", server_port);

    if (listen(server_pid , MAX_SESSION) < 0)
    {
        _DEBUG_MSG("TCP Server listen fail !! [MAX_USER : %d ]\n", MAX_SESSION);
        return -1;
    }

    _DEBUG_MSG("TCP Server listen Success !! [MAX_USER : %d ]\n", MAX_SESSION);

    _DEBUG_MSG( "Accepting connections ... \n" );

    return server_pid;
}
