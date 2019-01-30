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

#include "HttpRequest.h"

HTTP_REQUEST *httpRequestNew()
{
	HTTP_REQUEST *ret = (HTTP_REQUEST *)malloc(sizeof(HTTP_REQUEST));

	if (!ret)
		return NULL;

	memset(ret, 0, sizeof(ret));

	httpPacketInitialize((HTTP_PACKET *)ret, _httpRequestParseRequestLine);

	return ret;
}

void httpRequestDestroy(HTTP_REQUEST *request)
{
	if (request->method)
		free(request->method);
	if (request->uri)
		free(request->uri);

	httpPacketAttributesFlush(&request->packet);

	if (request->packet.internal.header)
		free(request->packet.internal.header);

	if (request->packet.payload)
		free(request->packet.payload);
}

void httpRequestInitialize(HTTP_REQUEST *request, const char *method, const char *uri, double protocolVersion)
{
	request->method          = strdup(method);
	request->uri             = strdup(uri);
	request->protocolVersion = protocolVersion;
}

unsigned char *httpRequestGetFullBuffer(HTTP_REQUEST *request, unsigned long *requestBufferLength)
{
	static const char httpProtocol[] = "HTTP/";
	HTTP_ATTRIBUTE *curr = NULL;
	unsigned long bufSize = 0, saveLen = 0;
	unsigned char *buf    = (unsigned char *)malloc(1), *saveBuf = NULL;
	char tempBuffer[64];

	if (!buf) return 0;

	*buf = 0;

	/* method */

	if (request->method)
	{
		if (!(buf = (unsigned char *)realloc((saveBuf = buf), 1 + (bufSize += strlen(request->method) + 1))))
		{
			free(saveBuf);
			return 0;
		}
	
		strcpy((char *)buf, request->method);
		strcat((char *)buf, " ");
	}

	/* uri */

	if (request->uri)
	{
		if (!(buf = (unsigned char *)realloc((saveBuf = buf), 1 + (bufSize += strlen(request->uri) + 1))))
		{
			free(saveBuf);
			return 0;
		}
	
		strcat((char *)buf, request->uri);
		strcat((char *)buf, " ");
	}

	/* version HTTP/ + 1.X + \r\n */

	if (!(buf = (unsigned char *)realloc((saveBuf = buf), 1 + (bufSize += strlen(httpProtocol) + 5))))
	{
		free(saveBuf);
		return 0;
	}

#ifdef WIN32
	_snprintf(tempBuffer, sizeof(tempBuffer) - 1, "%s%.1f\r\n", httpProtocol, request->protocolVersion);
#else
	snprintf(tempBuffer, sizeof(tempBuffer) - 1, "%s%.1f\r\n", httpProtocol, request->protocolVersion);
#endif

	strcat((char *)buf, tempBuffer);

	for (curr = request->packet.attributes;
			curr;
			curr = curr->next)
	{
		if (!(buf = (unsigned char *)realloc((saveBuf = buf), 1 + (bufSize += strlen(curr->name) + 2 + strlen(curr->value) + 2))))
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

	if (!(buf = (unsigned char *)realloc((saveBuf = buf), 1 + (bufSize += 2 + request->packet.payloadLength))))
	{
		free(saveBuf);
		return 0;
	}

	strcat((char *)buf, "\r\n");

	if (request->packet.payload)
		memcpy(buf + saveLen + 2, request->packet.payload, request->packet.payloadLength);

	if (requestBufferLength)
		*requestBufferLength = bufSize;

	return buf;
}

void httpRequestFreeFullBuffer(HTTP_REQUEST *request, unsigned char *buffer)
{
	if (buffer)
		free(buffer);
}

unsigned char httpRequestWrite(HTTP_REQUEST *request, int fd)
{
	unsigned long requestBufferLength;
	unsigned char *requestBuffer = httpRequestGetFullBuffer(request, &requestBufferLength);

	if (!requestBuffer)
		return 0;

	write(fd, requestBuffer, requestBufferLength);

	return 1;
}

void _httpRequestParseRequestLine(void *request, const char *requestBuffer)
{
	/* GET / HTTP/1.0 */
	char *firstSpace, *secondSpace, *slash;	
	HTTP_REQUEST *self = (HTTP_REQUEST *)request;

	if (!(firstSpace = strchr(requestBuffer, ' ')))
		return;

	if (!(secondSpace = strrchr(requestBuffer, ' ')))
		return;

	if (!(slash = strrchr(requestBuffer, '/')))
		return;

	*firstSpace = 0;
	*secondSpace = 0;
	*slash = 0;

	firstSpace++;
	slash++;

	self->method          = strdup(requestBuffer);
	self->uri             = strdup(firstSpace);
	self->protocolVersion = atof(slash);
}
