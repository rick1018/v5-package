/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "libhttp.h"

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif
 
void hexdump(void *mem, unsigned int len)
{
        unsigned int i, j;
        
        for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
        {
                /* print offset */
                if(i % HEXDUMP_COLS == 0)
                {
                        printf("0x%06x: ", i);
                }
 
                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }
                
                /* print ASCII dump */
                if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
                {
                        for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isprint(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);        
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
}

int main(int argc, char **argv)
{
	HTTP_REQUEST *request;
	HTTP_RESPONSE *response;
	unsigned char *buffer;
	unsigned long bufferSize;
	struct sockaddr_in s;
	struct timeval tv;
	fd_set fdread;
	int fd,i;
	char *pch,authenc[64],buf[32],user[]="admin",passwd[]="admin";

	if (argc == 1)
	{
		fprintf(stdout,"Usage: %s host\n", argv[0]);
		return 0;
	}	

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		perror("socket");
		return 0;
	}

	s.sin_family      = AF_INET;
	s.sin_port        = htons(8080);
	s.sin_addr.s_addr = inet_addr(argv[1]);

	if (s.sin_addr.s_addr == -1)
	{
		perror("inet_addr");
		return 0;
	}

	if (connect(fd, (struct sockaddr *)&s, sizeof(s)) < 0)
	{
		perror("connect");
		return 0;
	}

	if (!(request = httpRequestNew()))
	{
		fprintf(stdout,"could not allocate request.\n");
		return 0;
	}

	if (!(response = httpResponseNew()))
	{
		fprintf(stdout, "could not allocate response.\n");
		return 0;
	}

	httpRequestInitialize(request, "GET", "/cgi/admin/serverreport.cgi", 1.0);
	httpRequestAttributeAdd(request, "User-Agent", "Test", 0);	
	
 	sprintf(buf,"%s:%s",user,passwd);
  base64encode(authenc, buf, strlen(buf));
  sprintf(buf,"Basic %s",authenc);
  httpRequestAttributeAdd(request, "Authorization",buf, 0);

	buffer = httpRequestGetFullBuffer(request, &bufferSize);

	if (!buffer)
	{
		fprintf(stdout, "could not get full buffer.\n");
		return 0;
	}	
	fprintf(stdout,"Writing request size = %lu.\n", bufferSize);
	
 	write(1, buffer, bufferSize); 

	write(fd, buffer, bufferSize); 	

	httpRequestFreeFullBuffer(request,buffer); 

	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(fd, &fdread);

		tv.tv_sec  = 5;
		tv.tv_usec = 0;		

		if (select(fd + 1, &fdread, NULL, NULL, &tv))
		{
			unsigned char ret;
			int nread = 0;

			ioctl(fd, FIONREAD, &nread);
	
			ret = httpResponseRead(response, fd, nread);			
			
//  		fprintf(stdout,"Ret %d\n", ret); 		
      
			switch (ret)
			{
				case -1:
					fprintf(stdout, "httpResponseRead() returned an error.\n");
					exit(0);
				case 0:
//					fprintf(stdout,"Read %d bytes.\n", nread); 
					break;
				case 1:
					 buffer = httpResponseGetFullBuffer(response, &bufferSize);

					fprintf(stdout, "Read completed. Size = %lu\n", bufferSize);
					
//					if(buffer) write(1, buffer, bufferSize);

//					fprintf(stdout,"\n");
            hexdump(buffer,256); 

//					exit(1);
			}	
		}
	}
}
