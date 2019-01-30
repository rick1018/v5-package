/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <string.h>

#include "HttpResponse.h"

HTTP_RESPONSE *httpResponseNew()
{
	HTTP_RESPONSE *response = (HTTP_RESPONSE *)malloc(sizeof(HTTP_RESPONSE));

	if (!response)
		return NULL;

	memset(response, 0, sizeof(response));

	httpPacketInitialize((HTTP_PACKET *)response, _httpResponseParseRequestLine);

	return response;
}

void httpResponseDestroy(HTTP_RESPONSE *response)
{
	if (response->responseString)
		free(response->responseString);

   httpPacketAttributesFlush(&response->packet);
   
	if (response->packet.internal.header)
		free(response->packet.internal.header);
   
	if (response->packet.payload)
		free(response->packet.payload);	
}

void httpResponseInitialize(HTTP_RESPONSE *response, double protocolVersion, unsigned long responseCode, char *responseString)
{
	response->protocolVersion = protocolVersion;
	response->responseCode    = responseCode;
	response->responseString  = strdup(responseString);
}

unsigned char *httpResponseGetFullBuffer(HTTP_RESPONSE *response, unsigned long *responseBufferLength)
{
	HTTP_ATTRIBUTE *curr = NULL;
	unsigned char *buf = NULL, *saveBuf;
	unsigned long bufSize, saveLen = 0;
	char requestLine[256];

	// Request line
#ifdef WIN32
	bufSize = _snprintf(requestLine, sizeof(requestLine) - 1, "HTTP/%.1f %lu %.200s\r\n", response->protocolVersion, response->responseCode, (response->responseString)?response->responseString:"UNKNOWN");
#else
	bufSize = snprintf(requestLine, sizeof(requestLine) - 1, "HTTP/%.1f %lu %.200s\r\n", response->protocolVersion, response->responseCode, (response->responseString)?response->responseString:"UNKNOWN");
#endif


	if (!(buf = (unsigned char *)malloc(bufSize)))
		return NULL;

	strcpy((char *)buf, requestLine);

//Attributes

	for (curr = response->packet.attributes; curr; curr = curr->next)
	{		
		if (!(buf = (unsigned char *)realloc((saveBuf = buf), (bufSize += strlen(curr->name) + 2 + strlen(curr->value) + 2))))
//		if (!(buf = (unsigned char *)malloc((bufSize += strlen(curr->name) + 2 + strlen(curr->value) + 2))))
		{			  
			free(saveBuf);
			return 0;
		} 

		strcat((char *)buf, curr->name);
		strcat((char *)buf, ": ");
		strcat((char *)buf, curr->value);
		strcat((char *)buf, "\r\n");
	}

	saveLen = bufSize;


	if (!(buf = (unsigned char *)realloc((saveBuf = buf), (bufSize += 2 + response->packet.payloadLength))))
	{
		free(saveBuf);
		return 0;
	} 

	strcat((char *)buf, "\r\n");

	if (response->packet.payload)
		memcpy(buf + saveLen + 2, response->packet.payload, response->packet.payloadLength);

	if (responseBufferLength)
		*responseBufferLength = bufSize;

	return buf;
}

void httpResponseFreeFullBuffer(HTTP_RESPONSE *response, unsigned char *buffer)
{
	if (buffer)
		free(buffer);
}

// HTTP/1.0 200 OK
void _httpResponseParseRequestLine(void *response, const char *responseBuffer)
{
	char *slashPosition, *firstSpace, *secondSpace;
	HTTP_RESPONSE *self = (HTTP_RESPONSE *)response;

	if (!(slashPosition = strchr(responseBuffer, '/')))
		return;

	if (!(firstSpace = strchr(responseBuffer, ' ')))
		return;

	if (!(secondSpace = strrchr(responseBuffer, ' ')))
		return;

	*slashPosition = 0;
	*firstSpace    = 0;
	*secondSpace   = 0;	

	slashPosition++;
	firstSpace++;
	secondSpace++;

	self->protocolVersion = atof(slashPosition);
	self->responseCode    = strtoul(firstSpace, NULL, 10);

	if (*secondSpace != 0)
		self->responseString  = strdup(secondSpace);
}
