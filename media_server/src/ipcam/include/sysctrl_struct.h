/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Fitivision Inc.  2009,RS,DM365/DM355 Verify All Pass
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#if defined (__cplusplus)
extern "C" {
#endif

#ifndef __SYS_CONTROL_STRUCT_H__
#define __SYS_CONTROL_STRUCT_H__

/* Date    2008/03/13 */
/* Version 1.00.01    */
#define USR_LEN		33
#define PW_LEN		33
#if 0
#define IP_LEN	20///< IP string length
#else
#define IP_LEN	64///< IP string length
#endif

typedef unsigned short upnp_port_t;
/* 20140225 */
#if 0
typedef struct{
	char id[USR_LEN];
	char password[PW_LEN];
	char remote_ip[IP_LEN];
}login_data_t;
typedef struct{
	char user_id[USR_LEN];
	int	authority;
}get_user_authority_t;
typedef struct{
	char	user_id[USR_LEN];
	char	password[PW_LEN];
	unsigned char	authority;
	char reserve[29]; // add by kenny chiu 20130204
}add_user_t;
#endif
/* 20140225 */
#endif   /* __SYS_CONTROL_STRUCT_H__ */
#if defined (__cplusplus)
}
#endif
