#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <linux/tcp.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include "tcp_states.h"
#include "debug.h"

static sigjmp_buf jmpbuf;

static void alarm_func()
{
    siglongjmp(jmpbuf, 1);
}

int open_tcp_client(char *ip_addr, unsigned short int port, unsigned short int local_port)
{
    int sockfd = 0 , flags = 0, ret = 0, error = 0;
    fd_set rset, wset;
    char recvBuff[1024];
    struct sockaddr_in serv_addr,client_addr; 
    struct timeval ts;
    socklen_t len = sizeof(error);

    ts.tv_sec = 2;

    _DEBUG_MSG("Enter [ip : %s port : %d local port : %d]",ip_addr,port,local_port);
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        _DEBUG_MSG("Error : Could not create socket");
        return -1;
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;

    if( (flags = fcntl(sockfd, F_GETFL, 0 )) < 0 )goto error;

    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)goto error;

    if(local_port > 0)
    {
//	if(setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
//	    _DEBUG_MSG("Error : setsockopt");
        memset(&client_addr, '0', sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = INADDR_ANY;
        client_addr.sin_port = htons(local_port); 
        bind(sockfd , (struct sockaddr *) &client_addr , sizeof(client_addr) );
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 

    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr)<=0)
    {
        _DEBUG_MSG("inet_pton error occured [%s]",ip_addr);
        goto error;
    } 
    //fcntl(sockfd, F_SETFL, O_NONBLOCK);

    if( (ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
	if(errno != EINPROGRESS) {
            _DEBUG_MSG("Error : Connect Failed");
            goto error;
        }
    }

    if(ret == 0){
	errno = ETIMEDOUT;
	goto error;
    }

    if ((ret = select(sockfd + 1,&rset, &wset, NULL, &ts)) < 0 )goto error;

    if(ret == 0){
	errno = ETIMEDOUT;
	goto error;
    }

    if(FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd, &wset)){
	if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error,&len) < 0)goto error;
    }else goto error;

    if(error){
	errno = error;
	goto error;
    }

    if(fcntl(sockfd,F_SETFL,flags) < 0) goto error;

    return sockfd;

error:
    close(sockfd);
    return -1;
}

int socket_check(int sock)
{
     if(sock<=0)
         return 0;
     struct tcp_info info;
     int len = sizeof(info); 
     getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
     if((info.tcpi_state == TCP_ESTABLISHED)) {
        return 1; 
     }else{
        return 0; 
     }

    return 1;
}

int close_tcp_client(int clifd) {
    if(clifd < 1) {
        _DEBUG_MSG("Not a volid connection socket:%d\n", clifd);
        return -1;
    }
    close(clifd);
    return 0;
}

struct hostent * timeoutGetIP( const char *domain, int timeout ){
    struct hostent *ipHostent = NULL;

    signal(SIGALRM, alarm_func);
    if(sigsetjmp(jmpbuf, 1) != 0)
    {
        alarm(0);//timout  
        signal(SIGALRM, SIG_IGN);
        return NULL;
    }
    alarm(timeout);//setting alarm  
    ipHostent = gethostbyname(domain);
    signal(SIGALRM, SIG_IGN);
    return ipHostent;
}

char* domainToIP(const char* domain, int timeout){
    struct hostent * result;
    char *cppliveIP = NULL;

    result = timeoutGetIP(domain, timeout);
    if (NULL == result)
    {
        _DEBUG_MSG("ERROR");
        return NULL;
    }
    if (result->h_addr_list[0]){
        cppliveIP = calloc(32, sizeof(char));
        inet_ntop(AF_INET, (result->h_addr_list)[0], cppliveIP, 16);
        _DEBUG_MSG("%s", cppliveIP);
    }else
        return NULL;
    return cppliveIP;
}

int checkIpOrDomain(char *str){
    struct in_addr addr;
    int ret;

    _DEBUG_MSG("Enter");
    //if(checkNetworkStatus() != 1) return 0;
    ret = inet_pton(AF_INET, str, &addr);
    if (ret == 0 )
        return 1;
    else
        return 0;
}
