/*
    cgitest.c - Testprogram for cgi.o
    Copyright (c) 1998,9 by Martin Schulze <joey@infodrom.north.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111, USA.
 */

/*
 * Compile with: cc -o cgitest cgitest.c -lcgi
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <cgi.h>
#include "libhttp.h"
#include "base64.h"

s_cgi *cgi;

#define URL "http://www.infodrom.north.de/cgilib/"

int httpclient(char **argv);

void print_form()
{
    printf ("<h1>Test-Form</h1>\n");
    printf ("<form action=\"/cgi-bin/cgitest/insertdata\" method=post>\n");
    printf ("Input: <input name=string size=50>\n<br>");
    printf ("<select name=select multiple>\n<option>Nr. 1\n<option>Nr. 2\n<option>Nr. 3\n<option>Nr. 4\n</select>\n");
    printf ("Text: <textarea name=text cols=50>\n</textarea>\n");
    printf ("<center><input type=submit value=Submit> ");
    printf ("<input type=reset value=Reset></center>\n");
    printf ("</form>\n");
    printf ("<hr width=50%%><form action=\"/cgi-bin/cgitest/listall\" method=post>\n");
    printf ("Input: <input name=string size=50>\n<br>");
    printf ("<select name=select multiple>\n<option>Nr. 1\n<option>Nr. 2\n<option>Nr. 3\n<option>Nr. 4\n</select>\n");
    printf ("Text: <textarea name=text cols=50>\n</textarea>\n");
    printf ("<center><input type=submit value=Show> ");
    printf ("<input type=reset value=Reset></center>\n");
    printf ("</form>\n");
    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/redirect\">Redirect</a><p>\n");
    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/listall\">List Everything</a><p>\n");
    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/setcookie\">Set Cookie</a><p>\n");
}

void eval_cgi()
{
    printf ("<h1>Results</h1>\n\n");
    printf ("<b>string</b>: %s<p>\n", cgiGetValue(cgi, "string"));
    printf ("<b>text</b>: %s<p>\n", cgiGetValue(cgi, "text"));
    printf ("<b>select</b>: %s<p>\n", cgiGetValue(cgi, "select"));
}

void listall (char **env)
{
  char **vars;
  char *val;
  s_cookie *cookie;
  int i;
  char * pch;

  printf ("<h3>Environment Variables</h3>\n<pre>\n");
  for (i=0; env[i]; i++) {
  	pch = strstr (env[i],"HTTP_HOST=");    
  	if(pch != NULL)
  	 {
  	 	printf ("%s[%d]\n", env[i],i);
  	 	pch=strtok(env[i],"=");
  	 	pch=strtok(NULL,"=");
  	  printf ("%s\n", pch);
  	 } 
  	else
      printf ("%s[%d]\n", env[i],i);
  }
  
  printf ("</pre>\n<h3>CGI Variables</h3>\n<pre>\n");

  vars = cgiGetVariables (cgi);
  if (vars) {
      for (i=0; vars[i] != NULL; i++) {
	  val = cgiGetValue (cgi, vars[i]);
	  printf ("%s[%d]=%s\n", vars[i],i, val?val:"");
      }
      for (i=0; vars[i] != NULL; i++)
	  free (vars[i]);
  }

  printf ("</pre>\n<h3>Cookies</h3>\n<pre>\n");

  vars = cgiGetCookies (cgi);
  if (vars) {
      for (i=0; vars[i] != NULL; i++) {
	  cookie = cgiGetCookie (cgi, vars[i]);
	  printf ("%s=%s\n", vars[i], cookie?cookie->value:"");
      }
      for (i=0; vars[i] != NULL; i++)
	  free (vars[i]);
  }
  printf ("</pre>\n");
}

int main (int argc, char **argv, char **env)
{
    char *path_info = NULL;
    int i;
    char str[50],* pch;
    
    cgiDebug(0, 0);
    cgi = cgiInit();

    path_info = getenv("PATH_INFO");
    httpclient("127.0.0.1");
    	
/*    
    if (path_info) {
	  if (!strcmp(path_info, "/redirect")) {	  	
	      for(i=0; env[i]; i++) {
  	     pch = strstr (env[i],"HTTP_HOST=");      	    
  	     if(pch != NULL)
  	     {
  	 	    pch=strtok(env[i],"=");
  	 	    pch=strtok(NULL,"=");
  	 	    break;
         }
        } 
//	  	  sprintf(str,"http://%s:1080/cgi/admin/serverreport.cgi",pch);
//	  	  sprintf(str,"http://%s:1080/cgi/jpg/image.cgi",pch);
// 	  cgiRedirect(str);
//  	  sprintf(str,"127.0.0.1");

      cgiHeader();
 //     printf ("<html>\n<head><title>IPCAM</title></title></head>\n\n<body bgcolor=\"#ffffff\">\n");
      httpclient("127.0.0.1");
	    exit (0);
	  } else if (!strcmp(path_info, "/setcookie")) {
	    cgiSetHeader ("Set-Cookie", "Version=1; Library=cgilib; Path=/");
            cgiHeader();
	    printf ("<html>\n<head><title>cgilib</title></title>\n\n<body bgcolor=\"#ffffff\">\n");
	    printf ("<h1><a href=\"%s\">cgilib</a></h1>\n", URL);
	    printf ("<h3>Cookie Library set</h3>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest\">Test</a><p>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/redirect\">Redirect</a><p>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/listall\">List Everything</a><p>\n");
	} else if (!strcmp(path_info, "/listall")) {
            cgiHeader();
	    printf ("<html>\n<head><title>cgilib</title></title>\n\n<body bgcolor=\"#ffffff\">\n");
	    printf ("<h1><a href=\"%s\">cgilib</a></h1>\n", URL);
	    listall (env);
	} else {
	    cgiHeader();
	    printf ("<html>\n<head><title>cgilib</title></title>\n\n<body bgcolor=\"#ffffff\">\n");
	    printf ("<h1><a href=\"%s\">cgilib</a></h1>\n", URL);
	    if (strlen (path_info))
		printf ("path_info: %s<br>\n", path_info);
	    if (!strcmp(path_info, "/insertdata")) {
		eval_cgi();
	    } else
		print_form();
	}
    } else {
	cgiHeader();
	printf ("<html>\n<head><title>cgilib</title></title>\n\n<body bgcolor=\"#ffffff\">\n");
	printf ("<h1><a href=\"%s\">cgilib</a></h1>\n", URL);
	print_form();
    }
*/
    printf ("\n<hr>\n</body>\n</html>\n");
    return 0;
}

int httpclient(char **argv)
{
	HTTP_REQUEST *request;
	HTTP_RESPONSE *response;
	unsigned char *buffer;
	unsigned long bufferSize,i;
	struct sockaddr_in s;
	struct timeval tv;
	fd_set fdread;
	int fd;
	char *pch,authenc[64],buf[32],user[]="admin",passwd[]="admin";

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
    perror("socket");
		return 0;
	}


	s.sin_family      = AF_INET;
	s.sin_port        = htons(1080);
	s.sin_addr.s_addr = inet_addr("127.0.0.1"); //argv[1]);

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

//	httpRequestInitialize(request, "GET", "/", 1.0);
//	httpRequestInitialize(request, "GET", "/cgi/admin/serverreport.cgi", 1.0);
// 	httpRequestInitialize(request, "GET", "/cgi/admin/date.cgi?action=get", 1.0);
	httpRequestInitialize(request, "GET", "/cgi/jpg/image.cgi", 1.0);  
	httpRequestAttributeAdd(request, "User-Agent", "Test", 0);

//Base64 Authorization encode
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

//	fprintf(stdout,"Writing request size = %lu.\n", bufferSize); 

  for(i=0;i<bufferSize;i++) printf ("%c",buffer[i]);	
  
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
			
    //  printf("Ret %d\n", ret);
      
			switch (ret)
			{
				case -1:
					printf("httpResponseRead() returned an error.\n");
					exit(0);
				case 0:
					printf("Read %d bytes.\n", nread);
					break;
				case 1:
					buffer = httpResponseGetFullBuffer(response, &bufferSize);

	//				printf("Read completed. Size = %lu\n", bufferSize);

  	//				if (buffer) write(stdout, buffer, bufferSize);	
//  	        pch = strstr(buffer,"Content-Length:");   
//              pch = httpResponseAttributeGet(buffer,"Content-Length");
//            printf("Content-Length:%d,%d",response->packet.internal.headerLength,response->packet.internal.contentLength);
            //buffer+=response->packet.internal.headerLength;
	          for(i=0;i<response->packet.internal.contentLength;i++) printf ("%c",buffer[i+response->packet.internal.headerLength]);	

 //					  printf("\n");
 //          printf ("\n<hr>\n</body>\n</html>\n");

					exit(0);
			}	
		}
	}
}
