/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#ifndef _LIBHTTP_HTTPPACKET_H
#define _LIBHTTP_HTTPPACKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HttpAttribute.h"

typedef struct _http_packet {

	HTTP_ATTRIBUTE *attributes;

	unsigned long  payloadLength;
	unsigned char  *payload;

	void           (*requestLineParser)(void *self, const char *requestBuffer);

	struct {
		
		unsigned char readingHeaders;

		unsigned char *header;
		unsigned long headerLength;

		unsigned long contentLength;
		unsigned long contentLengthLeft;

		unsigned char setPayloadOnClose;
	
	} internal;

} HTTP_PACKET;

void httpPacketInitialize(HTTP_PACKET *packet, void (*)(void *, const char *));

unsigned char httpPacketAttributeAdd(HTTP_PACKET *packet, const char *attributeName, const char *attributeValue, unsigned char overwriteIfExisting);
const char *httpPacketAttributeGet(HTTP_PACKET *packet, const char *attributeName);
HTTP_ATTRIBUTE *httpPacketAttributeEnum(HTTP_PACKET *packet, unsigned long index);
unsigned char httpPacketAttributeRemove(HTTP_PACKET *packet, const char *attributeName);
unsigned char httpPacketAttributesFlush(HTTP_PACKET *packet);

void httpPacketSetPayload(HTTP_PACKET *packet, unsigned char *payload, unsigned long payloadLength);
unsigned char *httpPacketGetPayload(HTTP_PACKET *packet, unsigned long *payloadLength);

unsigned char httpPacketRead(HTTP_PACKET *packet, int fd, unsigned long bytesToRead);
void httpPacketReadHeader(HTTP_PACKET *packet, unsigned char *header, const unsigned long headerLength);

void httpPacketEncodeString(const char *string, char *encodedString, unsigned long encodedStringLength);
void httpPacketExtractUriInformation(const char *uri, char *protocol, unsigned long protocolSize, char *hostname, unsigned long hostnameSize, unsigned long *port, char *path, unsigned long pathSize);

#ifdef __cplusplus
}
#endif

#endif
