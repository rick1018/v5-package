/*
 * libhttp -- uninformed research
 *
 * skape
 * mmiller@hick.org
 * 09/25/2002
 */
#ifndef _LIBHTTP_HTTPATTRIBUTE_H
#define _LIBHTTP_HTTPATTRIBUTE_H

typedef struct _http_attribute {

	char                   *name;
	char                   *value;

	struct _http_attribute *next;

} HTTP_ATTRIBUTE;

#endif
