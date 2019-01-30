/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#ifndef _LIBHTTP_HTTPREQUEST_H
#define _LIBHTTP_HTTPREQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HttpPacket.h"
#include "HttpAttribute.h"

typedef struct _http_request {

	HTTP_PACKET		packet;

	char           *method;
	char           *uri;
	double         protocolVersion;

} HTTP_REQUEST;

HTTP_REQUEST *httpRequestNew();
void httpRequestDestroy(HTTP_REQUEST *request);
void httpRequestInitialize(HTTP_REQUEST *request, const char *method, const char *uri, double protocolVersion);

#define httpRequestAttributeAdd(request, attributeName, attributeValue, overwrite) httpPacketAttributeAdd((HTTP_PACKET *)request, attributeName, attributeValue, overwrite)
#define httpRequestAttributeGet(request, attributeName) httpPacketAttributeGet((HTTP_PACKET *)request, attributeName)
#define httpRequestAttributeEnum(request, index) httpPacketAttributeEnum((HTTP_PACKET *)request, index)
#define httpRequestAttributeRemove(request, attributeName) httpPacketAttributeRemove((HTTP_PACKET *)request, attributeName)
#define httpRequestAttributesFlush(request) httpPacketAttributesFlush((HTTP_PACKET *)request)
#define httpRequestSetPayload(request, payload, payloadLength) httpPacketSetPayload((HTTP_PACKET *)request, payload, payloadLength)

unsigned char *httpRequestGetFullBuffer(HTTP_REQUEST *request, unsigned long *requestBUfferLength);
void httpRequestFreeFullBuffer(HTTP_REQUEST *request, unsigned char *buffer);

#define httpRequestRead(request, fd, bytesToRead) httpPacketRead((HTTP_PACKET *)request, fd, bytesToRead)
unsigned char httpRequestWrite(HTTP_REQUEST *request, int fd);

/* Internal functions */

void _httpRequestParseRequestLine(void *request, const char *requestBuffer);

#ifdef __cplusplus
}
#endif

#endif
