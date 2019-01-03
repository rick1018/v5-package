#ifndef __SOCKET__
#define __SOCKET__

int open_tcp_client(char *ip_addr, unsigned short int port, unsigned short int local_port);
int socket_check(int sock);
int close_tcp_client(int clifd);
struct hostent * timeoutGetIP( const char *domain, int timeout );
char* domainToIP(const char* domain, int timeOut);
int checkIpOrDomain(char *str);
#endif
