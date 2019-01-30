/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#ifndef _LIBHTTP_HTTPRESPONSE_H
#define _LIBHTTP_HTTPRESPONSE_H

#include "HttpPacket.h"
#include "HttpAttribute.h"

typedef struct _http_response {

	HTTP_PACKET    packet;

	double         protocolVersion;
	unsigned long  responseCode;
	char           *responseString;

} HTTP_RESPONSE;

HTTP_RESPONSE *httpResponseNew();
void httpResponseDestroy(HTTP_RESPONSE *response);
void httpResponseInitialize(HTTP_RESPONSE *response, double protocolVersion, unsigned long responseCode, char *responseString);

#define httpResponseGetResponseCode(response) response->responseCode
#define httpResponseGetResponseString(response) response->responseString

#define httpResponseAttributeAdd(response, attributeName, attributeValue, overwrite) httpPacketAttributeAdd((HTTP_PACKET *)response, attributeName, attributeValue, overwrite)
#define httpResponseAttributeGet(response, attributeName) httpPacketAttributeGet((HTTP_PACKET *)response, attributeName)
#define httpResponseAttributeEnum(response, index) httpPacketAttributeEnum((HTTP_PACKET *)response, index)
#define httpResponseAttributeRemove(response, attributeName) httpPacketAttributeRemove((HTTP_PACKET *)response, attributeName)
#define httpResponseAttributesFlush(response) httpPacketAttributesFlush((HTTP_PACKET *)response)
#define httpResponseSetPayload(response, payload, payloadLength) httpPacketSetPayload((HTTP_PACKET *)response, payload, payloadLength)
#define httpResponseGetPayload(response, payloadLength) httpPacketGetPayload((HTTP_PACKET *)response, payloadLength)

unsigned char *httpResponseGetFullBuffer(HTTP_RESPONSE *response, unsigned long *responseBufferSize);
void httpResponseFreeFullBuffer(HTTP_RESPONSE *response, unsigned char *buffer);

#define httpResponseRead(request, fd, bytesToRead) httpPacketRead((HTTP_PACKET *)request, fd, bytesToRead)
void _httpResponseParseRequestLine(void *response, const char *requestBuffer);

#endif
