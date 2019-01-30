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
#include <fcntl.h>
#include <errno.h>
#include "libhttp.h"

#define PKG_SIZE  2*1448

int main(int argc, char **argv)
{
	HTTP_REQUEST *request;
	HTTP_RESPONSE *response;
	unsigned char *buffer;
	unsigned long bufferSize;
	struct sockaddr_in s;
	struct timeval tv;
	fd_set fdread;
	int fd,i,fout,nbytes;
	mode_t fmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	char *pch,authenc[64],buf[32],user[]="admin",passwd[]="admin";
  char local_file[256];
  FILE * fp;
//  	char html[] = "<HTML><BODY><B>Hi.</B></BODY></HTML>";
  
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
	s.sin_port        = htons(80);
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
//	httpRequestInitialize(request, "GET", "/cgi/jpg/image.cgi", 1.0);
//  httpRequestInitialize(request, "GET", "/live/audio.cgi", 1.0);
//	httpRequestInitialize(request, "GET", "/live/stream1.cgi", 1.0);
//	httpRequestInitialize(request, "GET", "/live/stream2.cgi", 1.0);
//  httpRequestInitialize(request, "GET", "/cgi/mjpg/mjpeg.cgi", 1.0);	
//	httpRequestInitialize(request, "GET", "/cgi/mpeg/stream1.cgi", 1.0);
//  httpRequestInitialize(request, "GET", "/cgi/mpeg/stream1.cgi", 1.0);
//  httpRequestInitialize(request, "GET", "/cgi/mjpg/mjpeg.cgi", 1.0);	
  
	httpRequestAttributeAdd(request, "User-Agent", "webcam_client", 0);	
//    httpRequestSetPayload(request, (unsigned char *)html, sizeof(html) - 1);
	
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
//	fprintf(stdout,"Writing request size = %lu.\n", bufferSize); write(1, buffer, bufferSize); 
	write(fd, buffer, bufferSize); 	

	httpRequestFreeFullBuffer(request,buffer); 
	
#if 0	
  i=0;   
  buffer = (unsigned char *)malloc(PKG_SIZE);

  fp = fopen("test.txt", "w");
  if(!fp)  {
    printf("create file error! %s\n", strerror(errno));
    return 0;
  }
  
  while((nbytes=read(fd,buffer,PKG_SIZE))>0)
  {
    printf("\nThe following is the response header:%d %d\n",i,nbytes);
    fwrite(buffer, 1, nbytes, fp);
	
    if (i++ > 10) break;
  } 
  fclose(fp);    
  close(fd);
  exit(0);
#else
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
//  	fprintf(stdout,"Ret %d nread %d\n", ret,nread); 		
//		fout = open("test.txt",O_WRONLY | O_CREAT | O_TRUNC, fmode);
//      buffer = httpResponseGetFullBuffer(response, &bufferSize);
//      httpResponseFreeFullBuffer(response,buffer);
//      close(fout);						            
      
		switch (ret)
		{
		  case -1:
				fprintf(stdout, "httpResponseRead() returned an error.\n");
				exit(0);
		  case 0:
	   			fprintf(stdout,"Read %d bytes.\n", nread);        
				break;
		  case 1:
			    buffer = httpResponseGetFullBuffer(response, &bufferSize);

				fprintf(stdout, "Read completed. Size = %lu %lu\n", bufferSize,response->packet.payloadLength);
					
//				if(buffer) write(stderr, buffer, bufferSize); 
				if(buffer) {
					fout = open("test.txt",O_WRONLY | O_CREAT | O_TRUNC, fmode);
				//	write(fout, buffer, bufferSize); 
 					write(fout,response->packet.payload,response->packet.payloadLength);
                    close(fout);						            
				}

				fprintf(stdout,"\n");
				if(i++ > 10) exit(0);
			}	
		}
	}
#endif	
}
