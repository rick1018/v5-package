/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <string.h>

#include "HttpPacket.h"

void httpPacketInitialize(HTTP_PACKET *packet, void (*func)(void *, const char *))
{
	packet->requestLineParser       = func;

	packet->attributes              = NULL;
	
	packet->payload                 = NULL;
	packet->payloadLength           = 0;

	packet->internal.readingHeaders = 1;
	packet->internal.header         = NULL;
	packet->internal.headerLength   = 0;
	packet->internal.setPayloadOnClose = 0;
}

unsigned char httpPacketAttributeAdd(HTTP_PACKET *packet, const char *attributeName, const char *attributeValue, unsigned char overwriteIfExisting)
{
	HTTP_ATTRIBUTE *attribute;
	int x = 0;

	if ((!attributeName) || (!attributeValue))
		return 0;

	if (overwriteIfExisting)
	{
		while ((attribute = httpPacketAttributeEnum(packet, x++)))
		{
#ifndef WIN32
			if (!strcasecmp(attribute->name, attributeName))
#else
			if (!stricmp(attribute->name, attributeName))
#endif
			{
				if (attribute->value)
					free(attribute->value);

				attribute->value = strdup(attributeValue);

				return 1;
			}	
		}
	}

	attribute = (HTTP_ATTRIBUTE *)malloc(sizeof(HTTP_ATTRIBUTE));

	if (!attribute)
		return 0;

	/*
	 * Always tail attributes onto the end so they're in the order with which they came.
	 */

	if (!packet->attributes)
		packet->attributes = attribute;
	else
	{
		HTTP_ATTRIBUTE *curr = packet->attributes;

		while (curr->next)
			curr = curr->next;

		curr->next = attribute;
	}

	attribute->name  = (char *)strdup(attributeName);
	attribute->value = (char *)strdup(attributeValue);
	attribute->next  = NULL;

	if ((!attribute->name) || (!attribute->value))
	{
		free(attribute);
		return 0;
	}

	return 1;
}

const char *httpPacketAttributeGet(HTTP_PACKET *packet, const char *attributeName)
{
	HTTP_ATTRIBUTE *curr = packet->attributes;

	if (!attributeName)
		return NULL;

	while (curr)
	{
#ifndef WIN32
		if (!strcasecmp(curr->name, attributeName))
#else
		if (!stricmp(curr->name, attributeName))
#endif
			return curr->value;

		curr = curr->next;
	}

	return NULL;
}

HTTP_ATTRIBUTE *httpPacketAttributeEnum(HTTP_PACKET *packet, unsigned long index)
{
	unsigned long x = 0;
	HTTP_ATTRIBUTE *curr = packet->attributes;

	for (;
			curr && x < index;
			curr = curr->next, x++);

	return curr;
}

unsigned char httpPacketAttributeRemove(HTTP_PACKET *packet, const char *attributeName)
{
	HTTP_ATTRIBUTE *curr = packet->attributes, *prev = NULL;

	if (!attributeName)
		return 0;

	while (curr)
	{
#ifndef WIN32
		if (!strcasecmp(curr->name, attributeName))
#else
		if (!stricmp(curr->name, attributeName))
#endif
			break;

		prev = curr;
		curr = curr->next;
	}

	if (!curr)
		return 0;

	if (!prev)
		packet->attributes = curr->next;
	else
		prev->next = curr->next;

	free(curr->name);
	free(curr->value);

	free(curr);

	return 1;	
}

unsigned char httpPacketAttributesFlush(HTTP_PACKET *packet)
{
	HTTP_ATTRIBUTE *curr;

	while (packet->attributes)
	{
		curr = packet->attributes->next;

		free(packet->attributes->name);
		free(packet->attributes->value);
		free(packet->attributes);

		packet->attributes = curr;
	}

	return 1;
}

void httpPacketSetPayload(HTTP_PACKET *packet, unsigned char *payload, unsigned long payloadLength)
{
	if (packet->payload)
		free(packet->payload);

	if (!(packet->payloadLength = payloadLength))
		packet->payload = NULL;
	else
	{
		if ((packet->payload = (unsigned char *)malloc(payloadLength)))
			memcpy(packet->payload, payload, payloadLength);
	}
}

unsigned char *httpPacketGetPayload(HTTP_PACKET *packet, unsigned long *payloadLength)
{
	if (payloadLength)
		*payloadLength = packet->payloadLength;

	return packet->payload;
}

unsigned char httpPacketRead(HTTP_PACKET *packet, int fd, unsigned long bytesToRead)
{
	unsigned char ret = 0;

	if (!bytesToRead && packet->internal.setPayloadOnClose)
	{
		packet->internal.readingHeaders = 1;
		ret = 1;
	}
	else if (!bytesToRead)
		return (unsigned char)-1;

	while ((!ret) && (bytesToRead > 0))
	{
		if (packet->internal.readingHeaders)
		{
			unsigned char *saveBuf = NULL;

			if (!packet->internal.header)
				packet->internal.header = (unsigned char *)malloc((packet->internal.headerLength = 1));
			else
				packet->internal.header = (unsigned char *)realloc((saveBuf = packet->internal.header), (packet->internal.headerLength += 1));

			if (!packet->internal.header)
			{
				if (saveBuf)
					free(saveBuf);

				return (unsigned char)-1;	
			}

			/* I'd love to make this faster, any suggestions?  I've considered doing peek on the fd. */

			read(fd, &packet->internal.header[packet->internal.headerLength-1], 1);				


			/* Check to see if we have the full header. */
	
			if ((packet->internal.headerLength >= 3) && (packet->internal.header[packet->internal.headerLength-3] == '\n') && (packet->internal.header[packet->internal.headerLength-1] == '\n'))
			{
				const char *contentLength = NULL, *connection = NULL;

				httpPacketReadHeader(packet, packet->internal.header, packet->internal.headerLength);

				contentLength = httpPacketAttributeGet(packet, "Content-Length");
				connection    = httpPacketAttributeGet(packet, "Connection");

				packet->internal.setPayloadOnClose = 0;
				packet->internal.contentLength     = 0;
				packet->internal.contentLengthLeft = 0;

				packet->payloadLength              = 0;
				if (packet->payload)
					free(packet->payload);
				packet->payload                    = NULL;

				if (contentLength)
				{
					packet->internal.contentLengthLeft = packet->internal.contentLength = strtoul(contentLength, NULL, 10);
					packet->internal.readingHeaders    = 0;
				}
				else if (connection)
				{
#ifdef WIN32
					if (!stricmp(connection, "close"))
#else
					if (!strcasecmp(connection, "close"))
#endif
					{
						packet->internal.setPayloadOnClose = 1;
						packet->internal.readingHeaders    = 0;
					}
				}
				else
					ret = 1;

				free(packet->internal.header);
				packet->internal.header = NULL;
			}

			bytesToRead--;
		} 
		else
		{
			unsigned char buf[4096], *origPayload = NULL;
			unsigned long roundOff;
			unsigned long readLength;

			if (packet->internal.setPayloadOnClose)
				roundOff = (bytesToRead > sizeof(buf))?sizeof(buf):bytesToRead;
			else
				roundOff = (packet->internal.contentLengthLeft > sizeof(buf))?sizeof(buf):packet->internal.contentLengthLeft;
 
			readLength = (bytesToRead > roundOff)?roundOff:bytesToRead;

			read(fd, buf, readLength);						

			if (!packet->payload)	
				packet->payload = (unsigned char *)malloc(bytesToRead);
			else
				packet->payload = (unsigned char *)realloc((origPayload = packet->payload), packet->payloadLength + readLength);

			if (!packet->payload)
			{
				if (origPayload)
					free(origPayload);

				return (unsigned char)-1;
			}

			memcpy(packet->payload + packet->payloadLength, buf, readLength);
			packet->payloadLength += readLength;

			packet->internal.contentLengthLeft -= readLength;

			if (!packet->internal.contentLengthLeft)
			{
				packet->internal.readingHeaders    = 1;
				ret = 1;
			}	

			bytesToRead -= readLength;
		}	
	}

	return ret;
}

void httpPacketReadHeader(HTTP_PACKET *packet, unsigned char *header, const unsigned long headerLength)
{
	unsigned long x = 0, lineStartPosition = 0, lineEndPosition = 0, requestLineParsed = 0;

	for (x = 0; x <= headerLength; x++)
	{
		if ((x >= 3) && (header[x] == '\n') && (header[x-1] == '\r'))
		{
			lineEndPosition = x-2;

			if (lineStartPosition == lineEndPosition)
				break;

			*(header+lineEndPosition+1) = 0;

			if (!requestLineParsed)
			{
				packet->requestLineParser(packet, (char *)header);

				requestLineParsed = 1;
			}
			else
			{
				unsigned char *attrName = header + lineStartPosition, *attrValue;

				if (!(attrValue = (unsigned char *)strchr((const char *)attrName, ':')))
					continue;

				*attrValue = 0;
				attrValue++;

				if (*attrValue == 0)
					continue;

				attrValue++;

				httpPacketAttributeAdd(packet, (char *)attrName, (char *)attrValue, 0);	
			}	

			lineStartPosition = x+1;
		}
	}
}

void httpPacketEncodeString(const char *string, char *encodedString, unsigned long encodedStringLength)
{
	unsigned long x, stringLen = strlen(string), encodedStringPosition = 0;

	memset(encodedString, 0, encodedStringLength);

	for (x = 0; x < stringLen; x++)
	{
		if (!isdigit(string[x]) && !isalpha(string[x]))
		{
			if (encodedStringPosition < encodedStringLength-3)
			{
				char enc[5];

				snprintf(enc, sizeof(enc) - 1, "%%%.2x",string[x]);

				strcat(encodedString, enc);
				encodedStringPosition += 3;
			}
		}
		else if (encodedStringPosition < encodedStringLength-1)
			encodedString[encodedStringPosition++] = string[x];
	}

	return;
}

void httpPacketExtractUriInformation(const char *uri, char *protocol, unsigned long protocolSize, char *hostname, unsigned long hostnameSize, unsigned long *port, char *path, unsigned long pathSize)
{
	char *colon, *currentPosition, *slash;

	if (protocol)
		memset(protocol, 0, protocolSize);
	if (hostname)
		memset(hostname, 0, hostnameSize);
	if (port)
		*port = 80;
	if (path)
		memset(path, 0, pathSize);

	if (!(currentPosition = strchr(uri, ':')))
		return;

	if (protocol)
		strncpy(protocol, uri, ((currentPosition - uri > protocolSize)?protocolSize:currentPosition - uri));
	currentPosition += 2;

	if (!*(currentPosition-1) || (*currentPosition != '/'))
		return;

	currentPosition++;

	colon = strchr(currentPosition, ':');
	slash = strchr(currentPosition, '/');

	if ((colon) && (colon < slash) && (port))
	{
		char portString[16];

		strncpy(portString, colon + 1, slash - colon);

		*port = atoi(portString);
	}
	else if (colon && hostname)
		strncpy(hostname, currentPosition, ((colon - currentPosition > hostnameSize)?hostnameSize:colon - currentPosition));
	else if (slash && hostname)
		strncpy(hostname, currentPosition, ((slash - currentPosition > hostnameSize)?hostnameSize:slash - currentPosition));
	
	if (slash && path)
		strncpy(path, slash, pathSize);
	else if (path)
		strncpy(path, "/", pathSize);
}
