/*
 *  Boa, an http server, base64.c implements base64 encoding/decoding.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "base64.h"

/*
 * Name: base64decode
 *
 * Description: Decodes BASE-64 encoded string
 */
int base64decode(void *dst,char *src,int maxlen)
{
    int bitval,bits;
    int val;
    int len,x,y;

    len = strlen(src);
    bitval = 0;
    bits = 0;
    y = 0;

    for (x=0;x<len;x++)
    {
#ifdef MODIFY_BASE_DEC
        if (src[x] == '=')
		    return y;
#endif
        if ((src[x] >= 'A') && (src[x] <= 'Z'))
            val = src[x] - 'A';
        else if ((src[x] >= 'a') && (src[x] <= 'z'))
            val = src[x] - 'a' + 26;
        else if ((src[x] >= '0') && (src[x] <= '9'))
            val = src[x] - '0' + 52;
        else
#ifdef MODIFY_BASE_DEC
        if (src[x]=='+')
            val = 62;
        else
            val = 63;
#else
        if (src[x] == '+')
            val = 62;
        else if (src[x] == '-')
            val = 63;
        else
            val = -1;
#endif
        if (val >= 0)
        {
            bitval = bitval << 6;
            bitval += val;
            bits += 6;
            while (bits >= 8)
            {
                if (y < maxlen)
                    ((char *)dst)[y++] = (bitval >> (bits-8)) & 0xFF;
                bits -= 8;
                bitval &= (1 << bits) - 1;
            }
        }
    }
    if (y < maxlen)
        ((char *)dst)[y++] = 0;
    return y;
}

static char base64chars[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Name: base64encode()
 *
 * Description: Encodes a buffer using BASE64.
 */
int base64encode( char *encoded, char *string, int len )
{
	int	i;
	char *p = encoded;
	for ( i=0; i<len-2; i+=3 )
	{
		*p++ = base64chars[(string[i]>>2) & 0x3F];
		*p++ = base64chars[((string[i]&0x3)<<4) | ((int) (string[i+1]&0xF0)>>4)];
		*p++ = base64chars[((string[i+1]&0xF)<<2) | ((int) (string[i+2]&0xC0)>>6)];
		*p++ = base64chars[string[i+2]&0x3F];
	}
	if ( i < len )
	{
		*p++ = base64chars[(string[i]>>2)&0x3F];
		if ( i == (len-1) )
            *p++ = base64chars[((string[i]&0x3)<<4)];
		else
		{
			*p++ = base64chars[((string[i]&0x3)<<4) | ((int) (string[i+1]&0xF0)>>4)];
			*p++ = base64chars[((string[i+1]&0xF)<<2)];
		}
		*p++ = '=';
	}
	*p++ = '\0';
	return p - encoded;
}