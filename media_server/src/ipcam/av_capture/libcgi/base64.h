#ifndef BASE64_H
#define BASE64_H
		  
int base64decode(void *dst,char *src,int maxlen);
int base64encode( char *encoded, char *string, int len );

#endif /* BASE64_H */
