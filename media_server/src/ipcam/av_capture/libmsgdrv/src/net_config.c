/* 
   This file contains an implementation of the command               
   that either displays or sets the characteristics of               
   one or more of the system's networking interfaces.  
*/
   
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ipnc_feature.h>
#if WIFI_FEATURE
#include <linux/wireless.h>
#else
#include <net/if.h>
#endif
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <unistd.h>
#include <signal.h>
//#include <asm/arch/v4l_ioctl.h>
#include <system_default.h>
//#include "lancam.h"
#include <net_config.h>
//#include <Msg_Def.h>
//#include <linux/autoconf.h>
//#include <ra_ioctl.h>

//#define PROC_IFINET6_PATH	"/proc/net/if_inet6"

//#define NET_CONFIG_DEBUG

#ifdef NET_CONFIG_DEBUG
#define DBG_ERR(x...)		perror(x)
#define DBG_NET(x...)		printf(x)
#define DBG(x...)			printf(x)
#else
#define DBG_ERR(x...)
#define DBG_NET(x...)
#define DBG(x...)
#endif

static struct sockaddr_in sa = {
	sin_family:	PF_INET,
	sin_port:	0
};

#if WIFI_FEATURE
#include <sys/syscall.h>
pid_t gettid(void) {  
   printf("[%s][%d][DBG]\n", __FILE__, __LINE__);
   return syscall(SYS_gettid);  
}

#define FOLLOW_WEBPAGE_PARSER 1

struct _EncryptType encryptType[] = {
#if FOLLOW_WEBPAGE_PARSER
   {__NONE_SECURITY, "NONE", ""},
   {__WEP, "WEP", ""},
   {__WPA1_CCMP, "WPAPSK", "/AES"},
   {__WPA1_TKIP, "WPAPSK", "/TKIP"},
   {__WPA1_CCMP_TKIP, "WPAPSK", "/AES"},
   {__WPA2_CCMP, "WPA2PSK", "/AES"},
   {__WPA2_TKIP, "WPA2PSK", "/TKIP"},
   {__WPA2_CCMP_TKIP, "WPA2PSK", "/AES"},
   {__WPA1_WPA2_CCMP, "WPA2PSK", "/AES"},
   {__WPA1_WPA2_TKIP, "WPA2PSK", "/TKIP"},
   {__WPA1_WPA2_CCMP_TKIP, "WPA2PSK", "/AES"}
#else
   {__NONE_SECURITY, "NONE", "NONE"},
   {__WEP, "WEP", "ENCRYPTED"},
   {__WPA1_CCMP, "WPA1PSK", "AES"},
   {__WPA1_TKIP, "WPA1PSK", "TKIP"},
   {__WPA1_CCMP_TKIP, "WPA1PSK", /*"Automatic (AES/TKIP)"*/ "AES/TKIP"},
   {__WPA2_CCMP, "WPA2PSK", "AES"},
   {__WPA2_TKIP, "WPA2PSK", "TKIP"},
   {__WPA2_CCMP_TKIP, "WPA2PSK", /*"Automatic (AES/TKIP)"*/ "AES/TKIP"},
   {__WPA1_WPA2_CCMP, /*"Automatic (WPA1PSK/WPA2PSK)"*/ "WPA1PSK/WPA2PSK", "AES"},
   {__WPA1_WPA2_TKIP, /*"Automatic (WPA1PSK/WPA2PSK)"*/ "WPA1PSK/WPA2PSK", "TKIP"},
   {__WPA1_WPA2_CCMP_TKIP, /*"Automatic (WPA1PSK/WPA2PSK)"*/ "WPA1PSK/WPA2PSK", /*"Automatic (AES/TKIP)"*/ "AES/TKIP"}
#endif


};

WPA_CLI_CMD_SETS _wpaCliCmdSets[] = {
	//// Infrastructure
	{INF_OPEN, 6,{REMOVE_NETWORK0, 
                 AP_SCAN1, 
                 ADD_NETWORK0, 
                 SET_NETWORK0_SSID, 
                 SET_NETWORK0_KEY_MGMT, 
                 SELECT_NETWORK0}}
	,{INF_WEP_WITH_OPEN, 8, {REMOVE_NETWORK0, 
                           AP_SCAN1, 
                           ADD_NETWORK0, 
                           SET_NETWORK0_SSID, 
                           SET_NETWORK0_KEY_MGMT, 
                           SET_NETWORK0_WEP_KEY0, 
                           SET_NETWORK0_TX_KEYIDX0, 
                           SELECT_NETWORK0}}
	,{INF_WEP_WITH_SHARED, 9, {REMOVE_NETWORK0, 
                              AP_SCAN1, 
                              ADD_NETWORK0, 
                              SET_NETWORK0_SSID, 
                              SET_NETWORK0_KEY_MGMT, 
                              SET_NETWORK0_WEP_KEY0, 
                              SET_NETWORK0_TX_KEYIDX0, 
                              SET_NETWORK0_AUTH_ALG, 
                              SELECT_NETWORK0}}
	,{INF_TKIP_AES, 6, {REMOVE_NETWORK0, 
                     AP_SCAN1, 
                     ADD_NETWORK0, 
                     SET_NETWORK0_SSID, 
                     SET_NETWORK0_PSK, 
                     SELECT_NETWORK0}}
	//// Ad-Hoc
	,{ADH_OPEN, 7, {REMOVE_NETWORK0, 
                 AP_SCAN2, 
                 ADD_NETWORK0, 
                 SET_NETWORK0_SSID, 
                 SET_NETWORK0_MODE1, 
                 SET_NETWORK0_KEY_MGMT, 
                 SELECT_NETWORK0}}
	,{ADH_WEP_WITH_OPEN, 9, {REMOVE_NETWORK0, 
                           AP_SCAN2, 
                           ADD_NETWORK0, 
                           SET_NETWORK0_SSID, 
                           SET_NETWORK0_MODE1, 
                           SET_NETWORK0_KEY_MGMT, 
                           SET_NETWORK0_WEP_KEY0, 
                           SET_NETWORK0_TX_KEYIDX0, 
                           SELECT_NETWORK0}}
};

static pthread_mutex_t ra_mutex;

void net_init_ra()
{
	if (pthread_mutex_init(&ra_mutex, NULL) != 0)
		fprintf(stderr, "%s %s() - error!\n", __FILE__, __func__);
}

int encodedSSID(char* _org_ssid, char* _target_ssid, int _ssid_len){
	int i, o = 0;
	char tmp[8];
	char tmpEncodeStr[SIZE];
   int tmpEncodeStrLen = 0;

   memset(tmpEncodeStr, 0, SIZE);
   if(_ssid_len == 0){
      fprintf(stderr, "[%s][%s][%d][%s][len:%d]\n", __FILE__, __func__, __LINE__, _org_ssid, _ssid_len);
      return 0;
   }

	for(i=0;i<_ssid_len;i++){
		switch(_org_ssid[i]){
			case SPACE:
				sprintf(tmp, "%c%d%c", DELIMETER_A, SPACE, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case EXCLAMATION_MARK:
				sprintf(tmp, "%c%d%c", DELIMETER_A, EXCLAMATION_MARK, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case DOUBLE_QUOTES:
				sprintf(tmp, "%c%d%c", DELIMETER_A, DOUBLE_QUOTES, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case NUMBER:
				sprintf(tmp, "%c%d%c", DELIMETER_A, NUMBER, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;			
			case DOLLAR:
				sprintf(tmp, "%c%d%c", DELIMETER_A, DOLLAR, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case PROCENTTECHEN:
				sprintf(tmp, "%c%d%c", DELIMETER_A, PROCENTTECHEN, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case AMPERSAND:
				sprintf(tmp, "%c%d%c", DELIMETER_A, AMPERSAND, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case SINGLE_QUOTE:
				sprintf(tmp, "%c%d%c", DELIMETER_A, SINGLE_QUOTE, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case OPEN_BRACKET:
				sprintf(tmp, "%c%d%c", DELIMETER_A, OPEN_BRACKET, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case CLOSE_BRACKET:
				sprintf(tmp, "%c%d%c", DELIMETER_A, CLOSE_BRACKET, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case ASTERISK:
				sprintf(tmp, "%c%d%c", DELIMETER_A, ASTERISK, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case PLUS:
				sprintf(tmp, "%c%d%c", DELIMETER_A, PLUS, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case COMMA:
				sprintf(tmp, "%c%d%c", DELIMETER_A, COMMA, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case HYPHEN:
				sprintf(tmp, "%c%d%c", DELIMETER_A, HYPHEN, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case DOT:
				sprintf(tmp, "%c%d%c", DELIMETER_A, DOT, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case SLASH:
				sprintf(tmp, "%c%d%c", DELIMETER_A, SLASH, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case COLON:
				sprintf(tmp, "%c%d%c", DELIMETER_A, COLON, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case SEMICOLON:
				sprintf(tmp, "%c%d%c", DELIMETER_A, SEMICOLON, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case LESS_THEN:
				sprintf(tmp, "%c%d%c", DELIMETER_A, LESS_THEN, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case EQUALS:
				sprintf(tmp, "%c%d%c", DELIMETER_A, EQUALS, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case GREATER_THAN:
				sprintf(tmp, "%c%d%c", DELIMETER_A, GREATER_THAN, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case QUESTION_MARK:
				sprintf(tmp, "%c%d%c", DELIMETER_A, QUESTION_MARK, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case AT_SYMBOL:
				sprintf(tmp, "%c%d%c", DELIMETER_A, AT_SYMBOL, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case OPENING_BRACKET:
				sprintf(tmp, "%c%d%c", DELIMETER_A, OPENING_BRACKET, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case BACKSLASH:
				sprintf(tmp, "%c%d%c", DELIMETER_A, BACKSLASH, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case CLOSNG_BRACKET:
				sprintf(tmp, "%c%d%c", DELIMETER_A, CLOSNG_BRACKET, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;		
			case CARET:
				sprintf(tmp, "%c%d%c", DELIMETER_A, CARET, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case UNDERSCORE:
				sprintf(tmp, "%c%d%c", DELIMETER_A, UNDERSCORE, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case GRAVE_ACCENT:
				sprintf(tmp, "%c%d%c", DELIMETER_A, GRAVE_ACCENT, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case OPENING_BRACE:
				sprintf(tmp, "%c%d%c", DELIMETER_A, OPENING_BRACE, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case VERTICAL_BAR:
				sprintf(tmp, "%c%d%c", DELIMETER_A, VERTICAL_BAR, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case CLOSING_BRACE:
				sprintf(tmp, "%c%d%c", DELIMETER_A, CLOSING_BRACE, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			case EQUIVALENCY_SIGN:
				sprintf(tmp, "%c%d%c", DELIMETER_A, EQUIVALENCY_SIGN, DELIMETER_B);
				o += sprintf(&tmpEncodeStr[o], "%s", tmp);
				break;
			default:
				o += sprintf(&tmpEncodeStr[o], "%c", _org_ssid[i]);
				break;
		}
	}

   tmpEncodeStrLen = strlen(tmpEncodeStr);
   if(tmpEncodeStrLen > 0 && tmpEncodeStrLen <= ENCODED_SSID_LEN){
      memset(/*g_encoded_ssid*/ _target_ssid, 0, ENCODED_SSID_LEN);
      strncpy(/*g_encoded_ssid*/ _target_ssid, tmpEncodeStr, tmpEncodeStrLen);
      return SUCCESS;
   }else{
      return ENCODE_SSID_FAIL;
   }
}

int decodedSSID(char* _encoded_ssid, char* _target_ssid, int _ssid_len){
	int i = 0, o = 0;
	char tmp[10];
	char tmpDecodeStr[SIZE];
   int tmpDecodeStrLen = 0;

   memset(tmpDecodeStr, 0, SIZE);
	
	while(i < _ssid_len){
		if(_encoded_ssid[i] == DELIMETER_A && 
            (_encoded_ssid[i+2] == DELIMETER_B || _encoded_ssid[i+2] == '\0')){
			sprintf(tmp, "%c ", _encoded_ssid[i+1]);
			o += sprintf(&tmpDecodeStr[o], "%c", atoi(tmp));
			i+=2;
		}else if(_encoded_ssid[i] == DELIMETER_A && 
            (_encoded_ssid[i+3] == DELIMETER_B || _encoded_ssid[i+3] == '\0')){
			sprintf(tmp, "%c%c ", _encoded_ssid[i+1], _encoded_ssid[i+2]);
			o += sprintf(&tmpDecodeStr[o], "%c", atoi(tmp));
			i+=3;
		}else if(_encoded_ssid[i] == DELIMETER_A && 
            (_encoded_ssid[i+4] == DELIMETER_B || _encoded_ssid[i+4] == '\0')){
			sprintf(tmp, "%c%c%c ", _encoded_ssid[i+1], _encoded_ssid[i+2], _encoded_ssid[i+3]);
			o += sprintf(&tmpDecodeStr[o], "%c", atoi(tmp));
			i+=4;
		}else{
			if(_encoded_ssid[i] != DELIMETER_B){
				o += sprintf(&tmpDecodeStr[o], "%c", _encoded_ssid[i]);
			}
			i++;
		}
	}

   tmpDecodeStrLen = strlen(tmpDecodeStr);
   if(tmpDecodeStrLen > 0 && tmpDecodeStrLen <= DECODED_SSID_LEN){
      memset(/*g_decoded_ssid*/ _target_ssid, 0, DECODED_SSID_LEN);
      strncpy(/*g_decoded_ssid*/ _target_ssid, tmpDecodeStr, tmpDecodeStrLen);
#if WIFI_DRV_VER_RT3070
      /*return tmpDecodeStrLen;*/
      return SUCCESS;
#else
      return SUCCESS;
#endif
   }else{
#if WIFI_DRV_VER_RT3070
      /*return 0;*/
      return DECODE_SSID_FAIL;
#else
      return DECODE_SSID_FAIL;
#endif
   }
}

int findConnectCmdSets(int type, WIFI_CONNECT_CMD *_wifi_connect_cmd){
   int i, limit = sizeof(_wpaCliCmdSets) / sizeof(WPA_CLI_CMD_SETS);
   for(i=0;i<limit;i++){
      if(type == _wpaCliCmdSets[i].type){
         _wifi_connect_cmd->cmdCnt = _wpaCliCmdSets[i].cmdCnt;
         _wifi_connect_cmd->connectCmd = _wpaCliCmdSets[i].cmdSets;
         return 1;
      }
   }
   return FIND_CONNECT_CMD_SETS_FAIL;
}

int cp2cmd(char *_cmd_dest, char *_cmd_src, ...){
   int cnt;
//   va_list argptr;

   memset(_cmd_dest, '\0', 128);
//   va_start(argptr, _cmd_src);
//   cnt = vsprintf(_cmd_dest, _cmd_src, argptr);
//   va_end(argptr);

//   printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
//   if(cnt > 0)
//      return 1;
//   else
      return 0;
}

int execWpaCliCmd(char *_cmd){
   FILE* ptr;
   char buf[SIZEBUF];
   /*char cmd[SIZEBUF];*/
   /*int ret = 0;*/
   
   /*execRtnCode(ret);*/

   ptr = popen(_cmd, "r");
   printf("Executing --->  %s\n\n", _cmd);
   if(ptr){
      while(!feof(ptr)){
         if((fgets(buf, SIZEBUF, ptr)) != NULL){
               printf("==> %s <==\n", buf);
         }
      }
      pclose(ptr);
   }else{
      perror(" POPEN ERROR !!!");
   }
   return 0;
}

int cleanAllNetworkList(void){
   FILE* ptr;
   char buf[SIZEBUF];
   char cmd[SIZEBUF];
	char tmpCmd[128];
   /*int ret = 0;*/
   
   /*execRtnCode(ret);*/

   memset(cmd, 0, SIZEBUF);
   sprintf(cmd, "%s|awk '{print $1}'", LIST_NETWORK);
   ptr = popen(cmd, "r");
   printf("Executing --->  %s\n\n", cmd);
   if(ptr){
      while(!feof(ptr)){
         if((fgets(buf, SIZEBUF, ptr)) != NULL){
            if(!strstr(buf, "network") && !strstr(buf, "Selected")){
               printf("==> %s <==\n", buf);
               cp2cmd(tmpCmd, "%s %s", REMOVE_NETWORK, buf);
               execWpaCliCmd(tmpCmd);
            }
         }
      }
      pclose(ptr);
   }else{
      perror(" POPEN ERROR !!!");
   }
   return 0;
}

///         | 64 | 128 | 152
///   ------+----+-----+-----
///   Hex   | 10 |  26 |  32
///   ASCII |  5 |  13 |  16

int execInfConnect(int _securityType, int _wepEncryptType, char *_ssid, char *_cipher){
	char tmpCmd[128];
	/*int ret, whichCipherType = 0;*/
   int cipherLen = strlen(_cipher);
   printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
   printf("cipher : %s\t len : %d\n", _cipher, strlen(_cipher));
	switch(_securityType){
		case _NONE_SECURITY:
         cleanAllNetworkList();
         cp2cmd(tmpCmd, "%s", AP_SCAN1);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", ADD_NETWORK0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_SSID, _ssid);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_KEY_MGMT);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SELECT_NETWORK0);
         execWpaCliCmd(tmpCmd);
			break;
		case _WEP:
         /*cipherLen = 10;*/
         printf("cipher : %s\t len : %d\n", _cipher, strlen(_cipher));
         cleanAllNetworkList();
         cp2cmd(tmpCmd, "%s", AP_SCAN1);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", ADD_NETWORK0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_SSID, _ssid);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_KEY_MGMT);
         execWpaCliCmd(tmpCmd);
         if(cipherLen == 5
               || cipherLen == 13
               || cipherLen == 16){
            /*printf("[%s][%s][%d]11111111111\n", __FILE__, __func__, __LINE__);*/
            cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_WEP_KEY0, _cipher);
            execWpaCliCmd(tmpCmd);
         }
         if(cipherLen == 10
               || cipherLen == 26
               || cipherLen == 32){
            /*printf("[%s][%s][%d]222222222222\n", __FILE__, __func__, __LINE__);*/
            cp2cmd(tmpCmd, "%s %s", SET_NETWORK0_WEP_KEY0, _cipher);
            execWpaCliCmd(tmpCmd);
         }
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_TX_KEYIDX0);
         execWpaCliCmd(tmpCmd);
         if(_wepEncryptType == WEP_SHARED){
            cp2cmd(tmpCmd, "%s", SET_NETWORK0_AUTH_ALG);
            execWpaCliCmd(tmpCmd);
         }
         cp2cmd(tmpCmd, "%s", SELECT_NETWORK0);
         execWpaCliCmd(tmpCmd);
         printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
			break;
		case _WPA1:
		case _WPA2:
		case _WPA1_WPA2:
         cleanAllNetworkList();
         cp2cmd(tmpCmd, "%s", AP_SCAN1);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", ADD_NETWORK0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_SSID, _ssid);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_PSK, _cipher);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SELECT_NETWORK0);
         execWpaCliCmd(tmpCmd);
			break;
      default:
         printf("Unrecognize cmd\n");
         break;
	};
   printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
   sleep(1);
   return 0;
}

int execAdHocConnect(int _securityType, int _wepEncryptType, char *_ssid, char *_cipher){
	char tmpCmd[128];
	/*int ret, whichCipherType = 0;*/
   int cipherLen = strlen(_cipher);
   printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
   printf("cipher : %s\t len : %d\n", _cipher, strlen(_cipher));
	switch(_securityType){
		case _NONE_SECURITY:
         cleanAllNetworkList();
         cp2cmd(tmpCmd, "%s", AP_SCAN2);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", ADD_NETWORK0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_SSID, _ssid);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_MODE1);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_KEY_MGMT);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SELECT_NETWORK0);
         execWpaCliCmd(tmpCmd);
			break;
		case _WEP:
         /*cipherLen = 10;*/
         printf("cipher : %s\t len : %d\n", _cipher, strlen(_cipher));
         cleanAllNetworkList();
         cp2cmd(tmpCmd, "%s", AP_SCAN2);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", ADD_NETWORK0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_SSID, _ssid);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_MODE1);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_KEY_MGMT);
         execWpaCliCmd(tmpCmd);
         if(cipherLen == 5
               || cipherLen == 13
               || cipherLen == 16){
            /*printf("[%s][%s][%d]11111111111\n", __FILE__, __func__, __LINE__);*/
            cp2cmd(tmpCmd, "%s \'\"%s\"\'", SET_NETWORK0_WEP_KEY0, _cipher);
            execWpaCliCmd(tmpCmd);
         }
         if(cipherLen == 10
               || cipherLen == 26
               || cipherLen == 32){
            /*printf("[%s][%s][%d]222222222222\n", __FILE__, __func__, __LINE__);*/
            cp2cmd(tmpCmd, "%s %s", SET_NETWORK0_WEP_KEY0, _cipher);
            execWpaCliCmd(tmpCmd);
         }
         cp2cmd(tmpCmd, "%s", SET_NETWORK0_TX_KEYIDX0);
         execWpaCliCmd(tmpCmd);
         cp2cmd(tmpCmd, "%s", SELECT_NETWORK0);
         execWpaCliCmd(tmpCmd);
         printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
			break;
      default:
         printf("Unrecognize cmd\n");
         break;
	};
   printf("[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
   sleep(1);
   return 0;
}

void initNode(WIFI_AP_NODE* theNode){
   /*theNode = (WIFI_AP_NODE*) malloc(sizeof(WIFI_AP_NODE));*/
   memset((WIFI_AP_NODE*)theNode, 0, sizeof(WIFI_AP_NODE));
   theNode->next = NULL;
}

WIFI_AP_INFO* initWifiApInfo(){
   WIFI_AP_INFO *theWifiApInfo = (WIFI_AP_INFO*)malloc(sizeof(WIFI_AP_INFO));
   if(!theWifiApInfo)
      return NULL;
   memset((WIFI_AP_INFO*)theWifiApInfo, 0, sizeof(WIFI_AP_INFO));
   return theWifiApInfo;
}

void showWifiAPInfo(WIFI_AP_INFO *theWifiAPInfo, int cnt){
   if(theWifiAPInfo){
      printf(" %-2s |", theWifiAPInfo->cell);
      printf(" %-32s |", theWifiAPInfo->ssid);
      printf(" %-17s |", theWifiAPInfo->mac_addr);
      printf(" %-28s |", theWifiAPInfo->securityType);
      printf(" %-20s |", theWifiAPInfo->encryptCipher);
      printf(" %-7s |", theWifiAPInfo->mode);
      printf(" %-7s |", theWifiAPInfo->channel);
      printf(" %-7s |", theWifiAPInfo->signal_level);
      printf("\n");
   }
}

void showDetailWifiAPInfo(WIFI_AP_INFO *theWifiAPInfo){
   if(theWifiAPInfo){
      printf("\n");
      printf("theWifiAPInfo->cell : %s\n", theWifiAPInfo->cell);
      printf("theWifiAPInfo->mac_addr : %s\n", theWifiAPInfo->mac_addr);
      printf("theWifiAPInfo->protocol : %s\n", theWifiAPInfo->protocol);
      printf("theWifiAPInfo->ssid : %s\n", theWifiAPInfo->ssid);
      printf("theWifiAPInfo->mode : %s\n", theWifiAPInfo->mode);
      printf("theWifiAPInfo->frequency : %s\n", theWifiAPInfo->frequency);
      printf("theWifiAPInfo->channel : %s\n", theWifiAPInfo->channel);
      printf("theWifiAPInfo->quality : %s\n", theWifiAPInfo->quality);
      printf("theWifiAPInfo->signal_level : %s\n", theWifiAPInfo->signal_level);
      printf("theWifiAPInfo->noise_level : %s\n", theWifiAPInfo->noise_level);
      printf("theWifiAPInfo->isEncryption : %d\n", theWifiAPInfo->isEncryption);
      printf("theWifiAPInfo->bit_rate : %s\n", theWifiAPInfo->bit_rate);
      printf("theWifiAPInfo->securityType : %s\n", theWifiAPInfo->securityType);
      printf("theWifiAPInfo->encryptCipher : %s\n", theWifiAPInfo->encryptCipher);
      printf("\n\n");
   }
}

void insertNode(WIFI_AP_NODE* _HeadNode, WIFI_AP_INFO* _NewWifiAPInfo){
   WIFI_AP_NODE *prevNode, *curNode;
   WIFI_AP_NODE *tmpNode = (WIFI_AP_NODE*)malloc(sizeof(WIFI_AP_NODE));
   memset((WIFI_AP_NODE*)tmpNode, 0, sizeof(WIFI_AP_NODE));
   memcpy(&tmpNode->wifi_ap_info, _NewWifiAPInfo, sizeof(WIFI_AP_INFO));
   tmpNode->next = NULL;
   prevNode = _HeadNode;
   curNode = _HeadNode->next;

   while(curNode != NULL){
      prevNode = curNode;
      curNode = curNode->next;
   }
   tmpNode->next = curNode;
   prevNode->next = tmpNode;
}

void traverseNode(WIFI_AP_NODE *_node, WifiGetSurveyData_t *WifiGetSurveyData){
   WIFI_AP_NODE *tmpNode;
   int cnt = 0;
   char _mode[12];
   if(_node->next == NULL){
      printf("the Node is empty\n");
   }else{
      tmpNode = _node->next;
      printf(" %-2s | %-32s | %-17s | %-28s | %-20s | %-7s | %-7s | %-7s |\n", 
            "No", "SSID", "Mac Addr", "Security", "Encryption", "Mode", "Channel", "Signal");
      printf("------------------------------------------------------------------------------------------------------------------------------------------------\n");
      while(tmpNode != NULL){
         if(cnt > 36) break; /// to fit old code limitation because no one wants to modify
         memset(_mode, '\0', sizeof(_mode));
#if FOLLOW_WEBPAGE_PARSER
         if(!strncmp(tmpNode->wifi_ap_info.mode, "Master", 6)){
            strncpy(_mode, "In", 2);
         }else{
            strcpy(_mode, tmpNode->wifi_ap_info.mode);
         }
#endif
         //// SSID,MacAddr,Security,Encryption,Mode,Channel,Signal

         sprintf(WifiGetSurveyData->WifiSurveyData[cnt], 
               "%s|%s|%s%s|%s|%s"
               ,tmpNode->wifi_ap_info.channel
               ,tmpNode->wifi_ap_info.mac_addr
               ,tmpNode->wifi_ap_info.securityType
               ,tmpNode->wifi_ap_info.encryptCipher
               ,tmpNode->wifi_ap_info.signal_level
               ,(FOLLOW_WEBPAGE_PARSER)?_mode:tmpNode->wifi_ap_info.mode);
         sprintf(WifiGetSurveyData->WifiSSID[cnt], "%s", tmpNode->wifi_ap_info.ssid);
         showWifiAPInfo(&tmpNode->wifi_ap_info, ++cnt);
         tmpNode = tmpNode->next;
      }
      WifiGetSurveyData->GetSurveyCnt = cnt;
   }
}

WIFI_AP_INFO* findNode(WIFI_AP_NODE *_node, int _num){
   WIFI_AP_NODE *tmpNode;
   char match[3];
   sprintf(match, "%02d", _num);
   if(_node->next == NULL){
      printf("the Node is empty\n");
   }else{
      tmpNode = _node->next;
      while(tmpNode != NULL){
         if(!strcmp(tmpNode->wifi_ap_info.cell, match))
            return &tmpNode->wifi_ap_info;
         tmpNode = tmpNode->next;
      }
   }
   return NULL;
}

WIFI_AP_INFO* findNodebySSID(WIFI_AP_NODE *_node, char* _ssid_name){
   WIFI_AP_NODE *tmpNode;

   if(_node->next == NULL){
      printf("the Node is empty\n");
   }else{
      tmpNode = _node->next;
      while(tmpNode != NULL){
         if(!strcmp(tmpNode->wifi_ap_info.ssid, _ssid_name))
            return &tmpNode->wifi_ap_info;
         tmpNode = tmpNode->next;
      }
   }
   return NULL;
}

int get_wifi_connect_status(){
   char buf[SIZEBUF], *tmp;
   int wpa_cli_status;

   FILE *fp = popen(NETWORK_STATUS, "r");

   if(fp){
      memset(buf, 0, SIZEBUF);
      fread(buf, SIZEBUF, 1, fp);

      if((tmp = strstr(buf, "wpa_state=INTERFACE_DISABLED")) != NULL){
         printf("INTERFACE_DISABLED...\n");
         wpa_cli_status = ENUM_INTERFACE_DISABLED;
      }else{
         if((tmp = strstr(buf, "wpa_state=INACTIVE")) != NULL){
            printf("INACTIVE...\n");
            wpa_cli_status = ENUM_WPA_INACTIVE;
         }else if((tmp = strstr(buf, "wpa_state=SCANNING")) != NULL){
            printf("SCANNING...\n");
            wpa_cli_status = ENUM_WPA_ASSOCIATED;
         }else if((tmp = strstr(buf, "wpa_state=ASSOCIATED")) != NULL){
            printf("ASSOCIATED...\n");
            wpa_cli_status = ENUM_WPA_ASSOCIATED;
         }else if((tmp = strstr(buf, "wpa_state=ASSOCIATING")) != NULL){
            printf("ASSOCIATING...\n");
            wpa_cli_status = ENUM_WPA_ASSOCIATING;
         }else if((tmp = strstr(buf, "wpa_state=DISCONNECTED")) != NULL){
            printf("DISCONNECTED...\n");
            wpa_cli_status = ENUM_WPA_ASSOCIATED;
         }else if((tmp = strstr(buf, "wpa_state=4WAY_HANDSHAKE")) != NULL){
            printf("4WAY_HANDSHAKE...\n");
            wpa_cli_status = ENUM_WPA_ASSOCIATED;
         }else if((tmp = strstr(buf, "wpa_state=COMPLETED")) != NULL){
            printf("COMPLETED...\n");
            wpa_cli_status = ENUM_WPA_COMPLETED;
         }else{
            printf("WPA_UNKNOWN...\n");
            printf("++++++++++++++++++++++++++++++++\n");
            printf("%s\n", buf);
            printf("++++++++++++++++++++++++++++++++\n");
            wpa_cli_status = ENUM_WPA_UNKNOWN;
         }
      }
      fclose(fp);
   }else{
      perror("open");
      return -2;
   }

   return wpa_cli_status;
}

int parseWpaCliStatusAfterDoingWps(wifi_wps_data_t* _wifi_wps_data){
   /*
    * # wpa_cli -i wlan0 stat
    *
    * bssid=b0:48:7a:a0:2c:16
    * ssid=DQE_TP-LINK_WR940N
    * id=0
    * mode=station
    * pairwise_cipher=CCMP
    * group_cipher=TKIP
    * key_mgmt=WPA2-PSK
    * wpa_state=COMPLETED
    * address=80:1f:02:8f:83:88
    * */
   char buf[SIZEBUF];
   char *tmp, *tmp1;
   FILE *fp = popen(NETWORK_STATUS, "r");

   if(fp){
      memset(buf, 0, SIZEBUF);
      fread(buf, SIZEBUF, 1, fp);

      tmp = strtok(buf, "\n");
      while(tmp != NULL){
         /*printf("len : %2d\tstring fmt:\t%s\n", strlen(tmp), tmp);*/
         if((tmp1 = strstr(tmp, "bssid=")) != NULL){
            printf("BSSID = %s\n", tmp+6);
            /*strncpy(, tmp+6, strcspn(tmp+6, "\n"));*/
         }else if((tmp1 = strstr(tmp, "ssid=")) != NULL){
            printf("SSID = %s\n", tmp+5);
            memset(_wifi_wps_data->SSID, 0, sizeof(_wifi_wps_data->SSID));
            strncpy(_wifi_wps_data->SSID, tmp+5, strcspn(tmp+5, "\n"));
         }else if((tmp1 = strstr(tmp, "id=")) != NULL){
            printf("ID = %s\n", tmp+3);
         }else if((tmp1 = strstr(tmp, "mode=")) != NULL){
            printf("MODE = %s\n", tmp+5);
         }else if((tmp1 = strstr(tmp, "pairwise_cipher=")) != NULL){
            printf("PAIRWISE_CIPHER = %s\n", tmp+16);
         }else if((tmp1 = strstr(tmp, "group_cipher=")) != NULL){
            printf("GROUP_CIPHER = %s\n", tmp+13);
         }else if((tmp1 = strstr(tmp, "key_mgmt=")) != NULL){
            printf("KEY_MGMT = %s\n", tmp+9);
         }else if((tmp1 = strstr(tmp, "wpa_state=")) != NULL){
            printf("WPA_STATE = %s\n", tmp+10);
         }else if((tmp1 = strstr(tmp, "address=")) != NULL){
            printf("ADDRESS = %s\n", tmp+8);
         }

         tmp = strtok(NULL, "\n");
      }

      //// fake data to fool web
      _wifi_wps_data->Auth = 0; //WIFI_WEP_AUTH_NONE
      _wifi_wps_data->Mode = 0; // INFRA
      _wifi_wps_data->Channel = 11;
      fclose(fp);
   }else{
      perror("open");
      return -1;
   }

   return 0;
}

int getRalinkPinCode(){
   ////RT2860 Linux STA PinCode        00258920
   char iwpriv_cmd[SIZE];
   char buf[SIZE], *tmp;
   unsigned int _pinCode;

   memset(iwpriv_cmd, '\0', SIZE);
   memset(buf, '\0', SIZE);

   sprintf(iwpriv_cmd, "iwpriv ra0 stat");

   FILE *fp = popen(iwpriv_cmd, "r");

   if(fp){
      while(!feof(fp)){
         if(!fgets(buf, sizeof(buf), fp)){
            break;
         }
         if((tmp = strstr(buf, "RT2860 Linux STA PinCode")) != NULL){
            /*printf("Pin Code ==> %s<==\n", tmp);*/
            sscanf(buf, "RT2860 Linux STA PinCode\t%08u\n", &_pinCode);
            /*printf("(Your Pin Code : %08u)\n", _pinCode);*/
            break;
         }
      }
      fclose(fp);
   }else{
      perror("open");
      return -1;
   }
   return (int)_pinCode;
}

int wps_pin_connect(unsigned int _pincode){
   int waitCnt = 0, wps_status;
   char wps_pin_cmd_buf[128];
   memset(wps_pin_cmd_buf, 0, sizeof(wps_pin_cmd_buf));
   sprintf(wps_pin_cmd_buf, "%s %08u", WPS_PIN_CMD, _pincode);
   cleanAllNetworkList();
   execWpaCliCmd(wps_pin_cmd_buf);
   while(waitCnt < WPS_WAIT_SECS){
      wps_status = get_wifi_connect_status();
      if(wps_status == ENUM_WPA_COMPLETED){
         break;
      }
      if(wps_status == ENUM_WPA_INACTIVE){
         break;
      }
      usleep(750000);
      waitCnt++;
   }
   return wps_status;
}

int wps_pbc_connect(){
   int waitCnt = 0, wps_status;
   cleanAllNetworkList();
   execWpaCliCmd(WPS_PBC_CMD);
   while(waitCnt < WPS_WAIT_SECS){
      wps_status = get_wifi_connect_status();
      if(wps_status == ENUM_WPA_COMPLETED){
         break;
      }
      if(wps_status == ENUM_WPA_INACTIVE){
         break;
      }
      usleep(750000);
      waitCnt++;
   }
   return wps_status;
}

static void HWPBC_SignalHandler(int sig){
   //if(sig==0x0a)
   //{
   fprintf(stderr, "[%s][%d] get HW_PBC signal from driver\n", __FILE__, __LINE__);
   fprintf(stderr, "[%s][%d] Before wps_pbc_connect() ; will wait 120 secs\n", __FILE__, __LINE__);
   wps_pbc_connect();
   //todo: execute the command below to start WPS PBC Method
   //"./wpa_cli -p/var/run/wpa_supplicant wps_pbc any"
   //pop UI/dialog to show starting WPS PBC - timeout = 120sec

   //}	
}

/* Wrapper to push some Wireless Parameter in the driver */
static inline int iw_set_ext_rtl(
      int	skfd,		         /* Socket to the kernel */
      const char *ifname,		/* Device name */
      int request_id,	      /* WE ID */
      struct iwreq *	pwrq)		/* Fixed part of the request */
{
   /* Set device name */
   fprintf(stderr, "[%s][%d] Before strncpy(pwrq->ifr_name, %s, %d)\n", __FILE__, __LINE__, ifname, strlen(ifname));
   strncpy(pwrq->ifr_name, ifname, /*IFNAMSIZ*/ strlen(ifname));
   //strncpy(pwrq->ifr_ifrn.ifr_name, ifname, IFNAMSIZ);

   /* Do the request */
   fprintf(stderr, "[%s][%d] Before return(ioctl(skfd, request_id, pwrq))\n", __FILE__, __LINE__);
   return(ioctl(skfd, request_id, pwrq));
}

void *detect_HW_PBC(void *_net_interface){
   struct iwreq wrq;
   int devsock;	
   char ifrn_name[IFNAMSIZ];	/* if name, e.g. "wlan0" */
   int cmd =  SIOCIWFIRSTPRIV + 0x05;
   int req[2];
   int _ret;

	pthread_detach(pthread_self());

   memset(ifrn_name, '\0', IFNAMSIZ);
   strncpy(ifrn_name, /*_net_interface*/"wlan0", 5 /*IFNAMSIZ*/ /*strlen(_net_interface)*/);

   devsock = socket(AF_INET, SOCK_STREAM, 0);
   //devsock = socket(AF_INET, SOCK_DGRAM, 0);
   if (devsock == -1) {
      //perror("Failed opening socket");
      fprintf(stderr, "[%s][%d] failed opening socket\n", __FILE__, __LINE__);
      /*exit(1);*/
   }

   /*(1) set signal handler. */
   signal(SIGUSR1, HWPBC_SignalHandler);

   /*(2) Tell wifi driver our pid, so that it can send a signal to us. */
   g_thread_func_id = gettid();
   fprintf(stderr, "[%s][%d] my tid is %d\n", __FILE__, __LINE__, g_thread_func_id);
   req[0]=0; req[1]=g_thread_func_id;

   memcpy(wrq.u.name, req, sizeof(int)*2);

   if((_ret = iw_set_ext_rtl(devsock, ifrn_name, cmd, &wrq)) < 0){
      fprintf(stderr, "[%s][%d] failed iw_set_ext_rtl()!\n", __FILE__, __LINE__);
      close(devsock);	
      /*exit(1);*/
   }

   while(1){
      /*printf("$> ");*/
      /*while( getchar() != '\n'){*/
      /*}*/
      /*if(_ret < 0)   _ret = iw_set_ext_rtl(devsock, ifrn_name, cmd, &wrq);*/
      fprintf(stderr, "[%s][%s][%d]\n", __FILE__, __func__, __LINE__);
      sleep(1);
   }	

   close(devsock);	
}

void removeDoubleQuotes(char *_target_ssid, int _target_ssid_len){
   char *tmpStoredSsid;

   tmpStoredSsid = (char *) malloc(sizeof(char) * _target_ssid_len + 1);
   if(!tmpStoredSsid){
      // Do nothing
      printf("[%s][%d] tmpStoredSsid malloc fail\n", __func__, __LINE__);
   }else{
      memset(tmpStoredSsid, '\0', sizeof(char) * _target_ssid_len + 1);
      strncpy(tmpStoredSsid, _target_ssid, _target_ssid_len);
      memset(_target_ssid, '\0', _target_ssid_len);
      strncpy(_target_ssid, tmpStoredSsid+SHIFT_ONE_QUOTE, _target_ssid_len-MINUS_TWO_QUOTES);
      free(tmpStoredSsid);
   }
}

int wifi_ap_site_survey(WIFI_AP_NODE *_headNode, char *_interface, WifiGetSurveyData_t *WifiGetSurveyData){
   char iwlist_scan_cmd[64];
   WIFI_AP_INFO* tmpWifiAPInfo;
   char buf[SIZE], *tmp;
   int  cellFlag = 0, cellCnt = 0;
   unsigned int encryptFlag = _INIT_SECURITY;

   memset(iwlist_scan_cmd, '\0', sizeof(iwlist_scan_cmd));
   sprintf(iwlist_scan_cmd, "iwlist %s scan", _interface);
	memset(WifiGetSurveyData, 0, sizeof(WifiGetSurveyData));

   FILE *fp = popen(iwlist_scan_cmd, "r");

   if(fp){
      while(!feof(fp)){
         if(!fgets(buf, sizeof(buf), fp)){
            break;
         }

         if((tmp = strstr(buf, "Cell ")) != NULL){
            encryptFlag = _INIT_SECURITY;
            if(cellFlag == 1){
               insertNode(_headNode, tmpWifiAPInfo);
               if(tmpWifiAPInfo){
                  printf("[%s][%s][%d] Before free()\n", __FILE__, __func__, __LINE__);
                  free(tmpWifiAPInfo);
               }
            }
            cellFlag = 1;
            tmpWifiAPInfo = initWifiApInfo();
            if(tmpWifiAPInfo == NULL){
               printf("tmpWifiAPInfo is NULL\n");
               fclose(fp);
               return 1;
            }
            /*printf("Cell ==> %c%c\n", *(tmp+5), *(tmp+6));*/
            sprintf(tmpWifiAPInfo->cell, "%c%c", *(tmp+5), *(tmp+6));
            if((tmp = strstr(buf, "Address: ")) != NULL){
               strncpy(tmpWifiAPInfo->mac_addr, tmp+9, strcspn(tmp+9, "\n"));
            }				
            cellCnt++;
         }else if((tmp = strstr(buf, "Protocol:")) != NULL){
            /*printf("Protocol = %s\n", tmp+9);*/
            strncpy(tmpWifiAPInfo->protocol, tmp+9, strcspn(tmp+9, "\n"));
         }else if((tmp = strstr(buf, "ESSID:")) != NULL){
            strncpy(tmpWifiAPInfo->ssid, tmp+6, strcspn(tmp+6, "\n"));
            removeDoubleQuotes(tmpWifiAPInfo->ssid, strlen(tmpWifiAPInfo->ssid));
         }else if((tmp = strstr(buf, "Mode:")) != NULL){
            strncpy(tmpWifiAPInfo->mode, tmp+5, strcspn(tmp+5, "\n"));
         }else if((tmp = strstr(buf, "Frequency:")) != NULL){
            strncpy(tmpWifiAPInfo->frequency, tmp+10, strcspn(tmp+10, " "));
            if((tmp = strstr(buf, "Channel ")) != NULL){
               strncpy(tmpWifiAPInfo->channel, tmp+8, strcspn(tmp+8, ")"));
            }
         }else if((tmp = strstr(buf, "Quality=")) != NULL){
            strncpy(tmpWifiAPInfo->quality, tmp+8, strcspn(tmp+8, " "));
            if((tmp = strstr(buf, "Signal level=")) != NULL){
               strncpy(tmpWifiAPInfo->signal_level, tmp+13, strcspn(tmp+13, "/"));
               if((tmp = strstr(buf, "Noise level=")) != NULL){
                  strncpy(tmpWifiAPInfo->noise_level, tmp+12, strcspn(tmp+12, " "));
               }
            }
         }else if((tmp = strstr(buf, "Encryption key:on")) != NULL){
            tmpWifiAPInfo->isEncryption = 1;
            encryptFlag |= _WEP;
            tmpWifiAPInfo->encryptCipherFlag = WEP_SHARED;
         }else if((tmp = strstr(buf, "Encryption key:off")) != NULL){
            tmpWifiAPInfo->isEncryption = 0;
            encryptFlag |= _NONE_SECURITY;
            tmpWifiAPInfo->encryptCipherFlag = _NONE_SECURITY;
         }else if((tmp = strstr(buf, "Bit Rates:")) != NULL){
            /*printf("Bit Rates = %s\n", tmp+10);*/
            strncpy(tmpWifiAPInfo->bit_rate, tmp+10, strcspn(tmp+10, "\n"));
         }else if((tmp = strstr(buf, "IE: WPA Version")) != NULL){
            encryptFlag &= _INIT_SECURITY;
            encryptFlag |= _WPA1;
         }else if((tmp = strstr(buf, "Pairwise Ciphers (1) : CCMP")) != NULL){
            encryptFlag |= _CCMP;
            tmpWifiAPInfo->encryptCipherFlag = _CCMP;
         }else if((tmp = strstr(buf, "Pairwise Ciphers (1) : TKIP")) != NULL){
            encryptFlag |= _TKIP;
            tmpWifiAPInfo->encryptCipherFlag = _TKIP;
         }else if((tmp = strstr(buf, "Pairwise Ciphers (2) : CCMP TKIP")) != NULL){
            encryptFlag |= _CCMP_TKIP;
            tmpWifiAPInfo->encryptCipherFlag = _CCMP_TKIP;
         }else if((tmp = strstr(buf, "WPA2 Version")) != NULL){
            if((encryptFlag & _WPA1) == _WPA1){
               encryptFlag &= 0x0f;
               encryptFlag |= _WPA1_WPA2;
            }else{
               encryptFlag &= 0x0f;
               encryptFlag |= _WPA2;
            }
         }

         switch(encryptFlag){
            case __NONE_SECURITY:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_NONE].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_NONE].encryptName);
               tmpWifiAPInfo->securityFlag = _NONE_SECURITY; 
               break;
            case __WEP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WEP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WEP].encryptName);
               tmpWifiAPInfo->securityFlag = _WEP; 
               break;
            case __WPA1_CCMP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_CCMP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_CCMP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1; 
               break;
            case __WPA1_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1; 
               break;
            case __WPA1_CCMP_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_CCMP_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_CCMP_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1; 
               break;
            case __WPA2_CCMP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA2_CCMP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA2_CCMP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA2; 
               break;
            case __WPA2_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA2_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA2_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA2; 
               break;
            case __WPA2_CCMP_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA2_CCMP_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA2_CCMP_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA2; 
               break;
            case __WPA1_WPA2_CCMP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_WPA2_CCMP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_WPA2_CCMP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1_WPA2; 
               break;
            case __WPA1_WPA2_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_WPA2_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_WPA2_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1_WPA2; 
               break;
            case __WPA1_WPA2_CCMP_TKIP:
               strcpy(tmpWifiAPInfo->securityType
                     , encryptType[ENUM_WPA1_WPA2_CCMP_TKIP].securityName);
               strcpy(tmpWifiAPInfo->encryptCipher
                     , encryptType[ENUM_WPA1_WPA2_CCMP_TKIP].encryptName);
               tmpWifiAPInfo->securityFlag = _WPA1_WPA2; 
               break;
         }
      }
      fclose(fp);
      if(tmpWifiAPInfo){
         insertNode(_headNode, tmpWifiAPInfo);
         printf("[%s][%s][%d] Before free()\n", __FILE__, __func__, __LINE__);
         free(tmpWifiAPInfo);
      }
      traverseNode(_headNode, WifiGetSurveyData);
   }else{
      perror("open");
      return -1;
   }

   return 0;
}

int wpa_supplicantActivate(char *_interface){
   FILE* fp;
   char wpa_supplicant_cmd[128], interface_file[64], wpa_supplicant_pid[10];
   int status, doingWpaSupplicantFlag = 0, ret = 0;

   memset(wpa_supplicant_cmd, '\0', sizeof(wpa_supplicant_cmd));
   sprintf(wpa_supplicant_cmd, "wpa_supplicant -B -Dwext -i %s -c /etc/wpa_supplicant.conf", _interface);
   memset(interface_file, '\0', sizeof(interface_file));
   sprintf(interface_file, "/var/run/wpa_supplicant/%s", _interface);
   printf(">>>>> %s\n", interface_file);

   //// check : the process is exist
   if((fp = popen("pidof wpa_supplicant", "r")) != NULL){
      memset(wpa_supplicant_pid, '\0', sizeof(wpa_supplicant_pid));
      fscanf(fp, "%s", wpa_supplicant_pid);
      status = pclose(fp);
      /*printf("\n-------------------------------\n");*/
      /*printf("Process exited? %s\n", WIFEXITED(status) ? "yes" : "no");*/
      /*printf("Process status: %d\n", WEXITSTATUS(status));*/
      /*printf("-------------------------------\n\n");*/
//      if(strlen(wpa_supplicant_pid) > 0 || WEXITSTATUS(status) == 0){
      if(strlen(wpa_supplicant_pid) > 0){
         printf("[%s][%s][%d] the wpa_supplicant already run on background (pid:%s)!!!\n", __FILE__, __func__, __LINE__, wpa_supplicant_pid);
         ret = 1;
      }else{
         doingWpaSupplicantFlag = 1;
         printf("[%s][%s][%d] the wpa_supplicant is not running !!!\n", __FILE__, __func__, __LINE__);
      }
   }

   if(doingWpaSupplicantFlag){
      printf("[%s][%s][%d] exec cmd: %s\n", __FILE__, __func__, __LINE__, wpa_supplicant_cmd);
      if((fp = popen(wpa_supplicant_cmd, "r")) != NULL){
         fclose(fp);
      }else{
         perror("open");
         ret = -1;
      }
   }

   if((fp = popen("pidof wpa_supplicant", "r")) != NULL){
      memset(wpa_supplicant_pid, '\0', sizeof(wpa_supplicant_pid));
      fscanf(fp, "%s", wpa_supplicant_pid);
      status = pclose(fp);
      /*printf("\n-------------------------------\n");*/
      /*printf("Process exited? %s\n", WIFEXITED(status) ? "yes" : "no");*/
      /*printf("Process status: %d\n", WEXITSTATUS(status));*/
      /*printf("-------------------------------\n\n");*/
//      if(strlen(wpa_supplicant_pid) > 0 || WEXITSTATUS(status) == 0){
       if(strlen(wpa_supplicant_pid) > 0){
         printf("[%s][%s][%d] the wpa_supplicant already run on background (pid:%s)!!!\n", __FILE__, __func__, __LINE__, wpa_supplicant_pid);
         ret = 1;
      }else{
         printf("[%s][%s][%d] the wpa_supplicant is not running !!!\n", __FILE__, __func__, __LINE__);
      }
   }
   return ret;
}

int parse_wifi_infce_by_lshw(char *_wifiInfce, int _wifiInfceLen){
   FILE* fp;
   char buf[128];
   int ret = -1, gotWifiInterface = 0;

   memset(buf, '\0', sizeof(buf));
   if((fp = popen("/bin/lshw -C network", "r")) != NULL){
      while(!feof(fp)){
         if(fgets(buf, sizeof(buf), fp) != NULL){
            if(buf[strlen(buf)-1] == '\n')
               buf[strlen(buf)-1] = '\0';
            if(strstr(buf, "description: Wireless interface")){
               gotWifiInterface = 1;
               /*printf("%s\n", buf);*/
            }
            if(gotWifiInterface){
               if(strstr(buf, "logical name:")){
                  if(strstr(buf, "logical name: wlan0")){
                     printf("[%s][%s][%d][%s]\n", __FILE__, __func__, __LINE__, buf);
                     memset(_wifiInfce, '\0', _wifiInfceLen);
                     strncpy(_wifiInfce, "wlan0", 5);
                     ret = _REALTEK_WIFI_DRV;
                  }else if(strstr(buf, "logical name: ra0")){
                     printf("[%s][%s][%d][%s]\n", __FILE__, __func__, __LINE__, buf);
                     memset(_wifiInfce, '\0', _wifiInfceLen);
                     strncpy(_wifiInfce, "ra0", 3);
                     ret = _RALINK_WIFI_DRV;
                  }else{
                     //// for Debug
                     printf("[%s][%s][%d][%s]\n", __FILE__, __func__, __LINE__, buf);
                     ret = _UNKNOWN_WIFI_DRV;
                  }
               }
            }
         }
      }
      pclose(fp);
   }

   return ret;
}

int getNetInterfaceName(char *_NetInfName){
#if 0    //// couldn't use <net/if.h> , mark first then fix it later by mike
   register int i;
   struct if_nameindex *ids;

   ids = if_nameindex();
   for (i=0; ids[i].if_index != 0; i++) {
      if((strstr(ids[i].if_name, "wlan")) || (strstr(ids[i].if_name, "ra"))){
         printf("[%s][%d][Index:%d] = %s\n"
               , __FILE__, __LINE__, ids[i].if_index, ids[i].if_name);
         strcpy(_NetInfName, ids[i].if_name);
         ret = 1;
         break;
      }
   }
   if_freenameindex(ids);
#else    //// use system cmd as a temporary solution
	int ret = -1;
   char cmd[128], buf[128];
   FILE *cmd_fp;

   memset(buf, '\0', sizeof(buf));
   memset(cmd, '\0', sizeof(cmd));
   sprintf(cmd, "ifconfig -a | sed 's/[ \t].*//;/^\(lo\|\)$/d'");

   /*printf("[%s][%d]Executing --->  %s\n", __FILE__, __LINE__, cmd);*/
   if((cmd_fp = popen(cmd, "r")) != NULL){
      fread(buf, sizeof(buf), 1, cmd_fp);
      if(strstr(buf, "wlan0")){
         memset(_NetInfName, '\0', WIFI_INFCE_LEN);
         strncpy(_NetInfName, "wlan0", 5);
         printf("[%s][%s][%d] Network INF Name : %s (len:%d)\n", __FILE__, __func__, __LINE__, _NetInfName, strlen(_NetInfName));
         ret = _REALTEK_WIFI_DRV;
      }else if(strstr(buf, "ra0")){
         memset(_NetInfName, '\0', WIFI_INFCE_LEN);
         strncpy(_NetInfName, "ra0", 3);
         printf("[%s][%s][%d] Network INF Name : %s (len:%d)\n", __FILE__, __func__, __LINE__, _NetInfName, strlen(_NetInfName));
         ret = _RALINK_WIFI_DRV;
      }else{
         printf("[%s][%s][%d] Couldn't find matched Network INF Name  ret:%d (_NetInfName len:%d)\n", __FILE__, __func__, __LINE__, ret, strlen(_NetInfName));
      }
      pclose(cmd_fp);
   }else{
      printf("[%s][%d] POPEN ERROR\n", __FILE__, __LINE__);
   }
#endif
      return ret;
}

int ValidateChecksum(unsigned long int PIN){
   unsigned long int accum = 0;
   accum += 3 * ((PIN / 10000000) % 10);
   accum += 1 * ((PIN / 1000000) % 10);
   accum += 3 * ((PIN / 100000) % 10);
   accum += 1 * ((PIN / 10000) % 10);
   accum += 3 * ((PIN / 1000) % 10);
   accum += 1 * ((PIN / 100) % 10);
   accum += 3 * ((PIN / 10) % 10);
   accum += 1 * ((PIN / 1) % 10);

   return (0 == (accum % 10))?__TRUE:__FALSE;
}

int ComputeChecksum(unsigned long int PIN){
   unsigned long int accum = 0;
   PIN *= 10;
   accum += 3 * ((PIN / 10000000) % 10);
   accum += 1 * ((PIN / 1000000) % 10);
   accum += 3 * ((PIN / 100000) % 10);
   accum += 1 * ((PIN / 10000) % 10);
   accum += 3 * ((PIN / 1000) % 10);
   accum += 1 * ((PIN / 100) % 10);
   accum += 3 * ((PIN / 10) % 10);

   int digit = (accum % 10);

   return (10 - digit) % 10;
}

int genUniqPincodeFromMacAddr(){
#if 0 
   int i = 0, ret = -1, cnt = 0;
   int place1, place2;
   unsigned int _num[3];
   char cmd[] = "ifconfig |grep HWaddr | tail -n 1 | awk '{print $5}'|cut -d ':' -f 4,5,6";
   FILE *cmd_fp;
   char buf[10], *tmp, tmp2[1], *tmpStrHex;

   cmd_fp = popen(cmd, "r");
   if(cmd_fp){
      fscanf(cmd_fp, "%s", buf);
      if(strlen(buf) > 0){
         /*printf("[%s][%d] Network HW addr : %s\n", __FILE__, __LINE__, buf);*/
         tmpStrHex = (char*) malloc(sizeof(char)*strlen(buf) + 1);
         if(!tmpStrHex) return ret;
         memset(tmpStrHex, '\0', strlen(buf) + 1);
         strncpy(tmpStrHex, buf, strlen(buf));

         tmp = strtok(tmpStrHex, ":");
         while(tmp != NULL){
            printf("string fmt: %s\n", tmp);
            i = 0, place1 = place2 = 0;
            while(i<2){
               switch(*(tmp+i)){
                  case '0':
                  case '1':
                  case '2':
                  case '3':
                  case '4':
                  case '5':
                  case '6':
                  case '7':
                  case '8':
                  case '9':
                     sprintf(tmp2, "%c", *(tmp+i));
                     if(i == 0) {place2 = atoi(tmp2);}
                     else {place1 = atoi(tmp2);}
                     break;
                  case 'a':
                  case 'A':
                     if(i == 0) {place2 = 10;}
                     else {place1 = 10;}
                     break;
                  case 'b':
                  case 'B':
                     if(i == 0) {place2 = 11;}
                     else {place1 = 11;}
                     break;
                  case 'c':
                  case 'C':
                     if(i == 0) {place2 = 12;}
                     else {place1 = 12;}
                     break;
                  case 'd':
                  case 'D':
                     if(i == 0) {place2 = 13;}
                     else {place1 = 13;}
                     break;
                  case 'e':
                  case 'E':
                     if(i == 0) {place2 = 14;}
                     else {place1 = 14;}
                     break;
                  case 'f':
                  case 'F':
                     if(i == 0) {place2 = 15;}
                     else {place1 = 15;}
                     break;
               }
               i++;
            }
            _num[cnt] = (place2<<4) + place1;
            if(_num[cnt] == 0) _num[cnt] = 0xff; // avoiding multiply 0 so give a default value 
            /*printf("_num = %d\t|\t%02x\n", _num[cnt], _num[cnt]);*/
            tmp = strtok(NULL, ":");
            cnt++;
         }
         /*printf("cnt = %d\n", cnt);*/

         if(tmpStrHex) free(tmpStrHex);
         return (_num[0] * _num[1] * _num[2]);
      }else{
         printf("[%s][%d]  Couldn't get Network HW addr\n", __FILE__, __LINE__);
      }
      pclose(cmd_fp);
   }else{
      printf("[%s][%d] POPEN ERROR\n", __FILE__, __LINE__);
      /*perror(" POPEN ERROR !!!");*/
   }
   return ret;
#endif
   int ret = -1, rangeA = 1000000, rangeB = 9999999, _7digiNum, pincode, checkSum;

   srand(time(NULL));
   _7digiNum = rand()%(rangeB-rangeA+1)+rangeA;
   printf("[%s][%s][%d] random 7 digital number = %d\n", __FILE__, __func__, __LINE__, _7digiNum);

   checkSum = ComputeChecksum((unsigned long int)_7digiNum);
   printf("[%s][%s][%d] check sum = %d\n", __FILE__, __func__, __LINE__, checkSum);
   pincode = _7digiNum*10+checkSum;

   if(ValidateChecksum((unsigned long int)pincode)){
      printf("[%s][%s][%d] %d is OK\n", __FILE__, __func__, __LINE__, pincode);
      return pincode;
   }else{
      printf("[%s][%s][%d] %d is Not OK\n", __FILE__, __func__, __LINE__, pincode);
   }
   return ret;
}
//#endif

// charles lin wifi
/**
* @brief check ralink wireless card status
*
* @param pInterface [I] ethernt device name. Ex: "ra0" "ra1"
* @return Link status
* @retval 0  Linkdown
* @retval 1  Linkup
* @retvl -1  Error
*/
//charles lin wifi6 start
//int Check_Link_status_wifi_ra(char *pInterface) // charles lin wifi 
int net_get_connStatus_ra()//GET station connection status
//wifi6 end
{
    int skfd = 0;
    struct iwreq wrq;
    char strTemp[16] = {0};
    u_char buffer[0x07ff];//u_char buffer[IW_PRIV_SIZE_MASK];

	pthread_mutex_lock(&ra_mutex);
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    sprintf(wrq.ifr_name, "ra0");
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.flags = 4;//wrq.u.data.flags = SHOW_CONN_STATUS;

    if (ioctl(skfd, 35825, &wrq) < 0)//35825 = 0x8be0+0x11 = RTPRIV_IOCTL_SHOW
    {
    	perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
    }
    //fprintf(stderr, "%s\n", buffer);
    memcpy(strTemp, buffer, 9);//"Connected(AP: smaxPT[00:1F:1F:5E:0C:60])" or "Disconnected"
	close(skfd);
	pthread_mutex_unlock(&ra_mutex);
	
	if (!strcmp(strTemp, "Connected")){
		return 1;
   }else{
      fprintf(stderr, "[%s][%s][%d][%s]\n", __FILE__, __func__, __LINE__, strTemp);
		return 0;
   }
}


//#define SIOCGIWSCAN 0x8B19	//charles lin wifi2
//#define SIOCSIWSCAN 0x8B18  //charles lin wifi2
////alice_modify_2012_3_19_v1	
int net_iwlist_scan(char* buffer, int* length)//chalrles lin wifi2
{
	int socket_id = 0;
    struct iwreq wrq;
    int to_cnt = 0;
    int ret;

	pthread_mutex_lock(&ra_mutex);
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    sprintf(wrq.ifr_name, "ra0");
    wrq.u.data.pointer = buffer;
    wrq.u.data.flags = 0;
    wrq.u.data.length = 0;

    if (ret = ioctl(socket_id, SIOCSIWSCAN, &wrq) < 0)
	{
		close(socket_id);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
    }

    sprintf(wrq.ifr_name, "ra0");
    wrq.u.data.pointer = buffer;
    wrq.u.data.flags = 0;
    wrq.u.data.length = 4096;

    while((ret = ioctl(socket_id, SIOCGIWSCAN, &wrq)) && to_cnt < 128)
    {
        to_cnt++;
        usleep(50000);//sleep 50 ms
    }

	if (ret == 0)
	{
	    *length = wrq.u.data.length;
		fprintf(stderr, "%s %s() - 'iwlist ra0 scanning' success!\n", __FILE__, __func__);
		close(socket_id);
		pthread_mutex_unlock(&ra_mutex);
		return 0;
	}
	else
	{
		fprintf(stderr, "%s %s() - 'iwlist ra0 scanning' error!(ret = %d, to_cnt = %d)\n", __FILE__, __func__, ret, to_cnt);
		close(socket_id);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
}
////alice_modify_2012_3_19_v1

int net_get_stat_ra(char *pInterface, char *data, int size)
{
	int skfd = 0;
	struct iwreq wrq;

	pthread_mutex_lock(&ra_mutex);

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, pInterface);
	wrq.u.data.pointer = data;
	wrq.u.data.length = size;
	wrq.u.data.flags = 0;

	if(ioctl(skfd, (0x8be0 + 0x9), &wrq) < 0)//RTPRIV_IOCTL_STATISTICS = 0x8be0 + 0x9
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
    	perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
    }
	data[wrq.u.data.length] = 0;
	close(skfd);
	pthread_mutex_unlock(&ra_mutex);
	return wrq.u.data.length;
}

/**
* @find key string in ra0 stat
*
* @param    data		
*
* @return    -1:fail, 0:success, but not found, 1:success and found it!
*/
int find_ra_stat_str(char* str)
{
	static char str_buf[1024 + 64] = {0};

	if (net_get_stat_ra("ra0", str_buf, sizeof(str_buf)) < 0)
		return -1;

	if (strstr(str_buf, str))
		return 1;//found it!
	else
		return 0;
}

static int freq2ch(unsigned int freq)
{
	int ch = -1;
	
	switch(freq)
	{
		case 241200000:
			ch = 1;
			break;
		case 241700000:
			ch = 2;
			break;
		case 242200000:
			ch = 3;
			break;
		case 242700000:
			ch = 4;
			break;
		case 243200000:
			ch = 5;
			break;
		case 243700000:
			ch = 6;
			break;
		case 244200000:
			ch = 7;
			break;
		case 244700000:
			ch = 8;
			break;
		case 245200000:
			ch = 9;
			break;
		case 245700000:
			ch = 10;
			break;
		case 246200000:
			ch = 11;
			break;
		case 246700000:
			ch = 12;
			break;
		case 247200000:
			ch = 13;
			break;
		case 248400000:
			ch = 14;
			break;
		default:
			ch = -1;
			break;
	}

	return ch;
}

int net_get_channel_ra(char *pInterface)//, char *data, int size)
{
	int skfd = 0;
	struct iwreq wrq;

	pthread_mutex_lock(&ra_mutex);

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
/*
	wrq->u.freq.m = pAd-> CommonCfg.Channel;
	wrq->u.freq.e = 0;
	wrq->u.freq.i = 0;*/
	strcpy(wrq.ifr_name, pInterface);
	wrq.u.data.pointer = 0;
	wrq.u.data.length = 0;
	wrq.u.data.flags = 0;

	if (ioctl(skfd, (0x8b05), &wrq) < 0)//SIOCGIWFREQ = 0x8B05
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}

	close(skfd);
	fprintf(stderr, "%s %s() - wrq.u.freq : m = %u, e = %u, i = %u\n", __FILE__, __func__, wrq.u.freq.m, wrq.u.freq.e, wrq.u.freq.i);
	pthread_mutex_unlock(&ra_mutex);
	return freq2ch(wrq.u.freq.m);
}

/**
* @find key string in ra0 stat
*
* @param    *value1		Link Quality=value1/value2  in command "iwconifg ra0" result
* @param    *value2
*
* @return    -1:disconnect or "Link Quality" or "Signal level" not found or other error, other value is link quality
*/
int net_get_link_quality_ra(int* value1, int* value2)
{
	static unsigned char buf[1024] = {0};
	unsigned char buf2[16] = {0};
	FILE *fp = NULL;
	long file_begin = 0, file_end = 0;
	char *start = 0, *end = 0;

	if (net_get_connStatus_ra() != 1)//connect
		return -1;

	pthread_mutex_lock(&ra_mutex);
	system("iwconfig ra0 > /tmp/iwconfig_log");
	pthread_mutex_unlock(&ra_mutex);

	fp = fopen("/tmp/iwconfig_log", "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_SET);
		file_begin = ftell(fp);
		fseek(fp, 0, SEEK_END);
		file_end = ftell(fp);

		fseek(fp, 0, SEEK_SET);
		fread(buf, 1, file_end - file_begin, fp);

		//fprintf(stderr, "end(%d) - begin(%d) = %d, buf = %s\n", file_end, file_begin, file_end - file_begin, buf);
		fclose(fp);
	}
	else
		fprintf(stderr, "file not exist!\n");

	start = strstr(buf, "Link Quality");
	if (start == 0)
	{
		fprintf(stderr, "%s %s() - \"Link Quality\" not found!\n", __FILE__, __func__);
		return -1;
	}
	else
		 start += 13;// because strlen("Link Quality=") = 13

	end = strstr(start, "/");
	if(end == 0)
	{
		fprintf(stderr, "%s %s() - \"/\" not found!\n", __FILE__, __func__);
		return -1;
	}

	strncpy(buf2, start, end - start);
	*value1 = atoi(buf2);

	start = end + 1;
	end = strstr(start, "Signal level");
	if (start == 0)
	{
		fprintf(stderr, "%s %s() - \"Signal level\" not found!\n", __FILE__, __func__);
		return -1;
	}

	strncpy(buf2, start, end - start);
	*value2 = atoi(buf2);

	unlink("/tmp/iwconfig_log");
	return 0;
}

//wifi6 end
//alice_modify_2012_3_27_v1
typedef struct
{
	int WifiCnt;	
	char Wifissid[MAX_AP_NUM][64];
	char WifiAddress[MAX_AP_NUM][64];
	int Wifissidlength[MAX_AP_NUM];
} WIFI_SSID_t;

//alice_modify_2012_3_29_v1
//----------------------------------------------------------------
#if WIFI_DRV_VER_RT3070
int net_get_site_survey_ra(WifiGetSurveyData_t *WifiGetSurveyData)//charles lin wifi2//charles lin wifi6
{
	int skfd = 0;
	struct iwreq wrq;
	struct timeval	tv;			
	/* Select timeout */  
	int	timeout = 15000000;		/* 15s */

	char SurveyBuffer[MAX_AP_NUM][256];
	char SurveyAddress[MAX_AP_NUM][64];
	char SurveySSID[MAX_AP_NUM][64];
	char SurveyBufferTmp[4096];
	//
	char Surveytruedata[MAX_AP_NUM][256];
	int  TruedataCnt,surveylength;
	FILE *fp;
	char getnumbuf[2048],getnumbuf2[2048];
	int  i = 0, j = 0, TmpCnt = 0, Surveyflag = 0;
	int ret = 0;
	unsigned char *xxbuffer=NULL;
	int scanlength = IW_SCAN_MAX_DATA; //IW_SCAN_MAX_DATA=4096

	TruedataCnt = 0;
	pthread_mutex_lock(&ra_mutex);
	memset(WifiGetSurveyData, 0, sizeof(WifiGetSurveyData));
	memset(SurveySSID, '\0', sizeof(SurveySSID));
	memset(SurveyBufferTmp, '\0', sizeof(SurveyBufferTmp));

	memset(SurveyBuffer, '\0', sizeof(SurveyBuffer));
	memset(SurveyAddress, '\0', sizeof(SurveyAddress));
	memset(Surveytruedata, '\0', sizeof(Surveytruedata));
	  
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t) SurveyBufferTmp;
	wrq.u.data.length = 4096;
	wrq.u.data.flags = 0;


	if (ioctl(skfd, (0x8be0 + 0xd), &wrq) < 0)  //RTPRIV_IOCTL_GSITESURVEY = 0x8be0 + 0xd
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
	surveylength = wrq.u.data.length ;
//	fprintf(stderr,"\n[%d]RTPRIV_IOCTL_GSITESURVEY ==err(%d,%d)\n",__LINE__,wrq.u.data.length,surveylength);	
//	FILE * pFile;
//       pFile = fopen ("/tmp/survey.txt", "wb");
//       fwrite (SurveyBufferTmp , sizeof(char), wrq.u.data.length, pFile);
//       fclose (pFile);


//	usleep(250000);		
	sprintf(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = NULL;
	wrq.u.data.flags = 0;
	wrq.u.data.length = 0;

 /* Initiate Scanning */	
	if (ret = ioctl(skfd, SIOCSIWSCAN, &wrq) < 0)   //SIOCSIWSCAN (0x8B18).
	{
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
//	fprintf(stderr,"\n[%d]siocsiwscan ==err(%d)\n",__LINE__,ret);
//	usleep(500000);	

 /* Init timeout value -> 1000ms between set and first get */  
	 tv.tv_sec = 0;  
	 tv.tv_usec = 3000000;

#if 1
  /* Forever */  
  while(timeout > 0) 
   {
      fd_set rfds;	/* File descriptors for select */
      int	last_fd;	/* Last fd */
	  
      /* Guess what ? We must re-generate rfds each time */     
       FD_ZERO(&rfds);      
       last_fd = -1;      
      /* In here, add the rtnetlink fd in the list */      
      /* Wait until something happens */     
      ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);     
      /* Check if there was an error */      
     if(ret < 0)	
	 {	 
	     if(errno == EAGAIN || errno == EINTR)	    continue;	  
	     fprintf(stderr, "Unhandled signal - exiting...\n");	 
	     return(-1);	
	  }      

    /* Check if there was a timeout */    
   if(ret == 0)	
   {	  
       unsigned char *	newbuf;		   
	   
realloc:	 
    /* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */	  
	newbuf =realloc(xxbuffer, scanlength);	  
	if(newbuf == NULL)	    
	{	      
		if(xxbuffer)  free(xxbuffer);	     
		fprintf(stderr, "%s: Allocation failed\n", __FUNCTION__);	      
		return(-1);	    
	}	
	xxbuffer = newbuf;
	
	sprintf(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t)xxbuffer;
	wrq.u.data.flags = 0;
	wrq.u.data.length = scanlength;
//	int  to_cnt = 0;
//	usleep(250000);	
	if(ret = ioctl(skfd, SIOCGIWSCAN, &wrq) < 0)  //SIOCGIWSCAN (0x8B19)
//	while((ret = ioctl(skfd, SIOCGIWSCAN, &wrq)) && to_cnt < 50)
	{
//         fprintf(stderr,"\n[%d]siocgiwscan ==err(%d,%d,%d,%d) %s\n",__LINE__,ret,wrq.u.data.length,scanlength,errno,strerror(errno));
   	  if(errno == E2BIG)
  	  {
  	    if(wrq.u.data.length > scanlength)   scanlength = wrq.u.data.length;
	    else 	scanlength *= 2;

           /* Try again */		  
           goto realloc;
	  }
//	  to_cnt++;
//	  usleep(50000);//sleep 50 ms
	}
//      sleep(2);	
      scanlength = wrq.u.data.length;
	timeout = 0;
   	}   
 }
//    fprintf(stderr,"\n[%d]siocgiwscan ==err(%d,%d,%d)\n",__LINE__,ret,wrq.u.data.length,scanlength);

//    fprintf(stderr,"[%d] %d %d\n",__LINE__,scanlength,i);

/* 
====================================
| length:2 bytes | command:2 bytes | data:(length-4) |  
====================================
Examples of data types are the ESSID, BSSID, channel, etc. 
*/	  
//     fprintf(stderr,"\n[%d]siocgiwscan ==err(%d,%d,%d)\n",__LINE__,ret,wrq.u.data.length,scanlength);
//  	  FILE * pFile;
//         pFile = fopen ("/tmp/scan.txt", "wb");
//         fwrite (xxbuffer , sizeof(char), wrq.u.data.length, pFile);
//         fclose (pFile);

      WIFI_SSID_t WIFI_SSID;
	memset(&WIFI_SSID, 0, sizeof(WIFI_SSID_t));

	i = 0;
	while ((i < scanlength) && (WIFI_SSID.WifiCnt < MAX_AP_NUM))
	{			  	
	 	if (xxbuffer[i] == 0x14 && xxbuffer[i+1] == 0x00 && xxbuffer[i+2] == 0x15 && xxbuffer[i+3] == 0x8B &&
		     xxbuffer[i+4] == 0x01 && xxbuffer[i+5] == 0x00 && xxbuffer[i+14] == 0x00 && xxbuffer[i+15] == 0x00 &&
		     xxbuffer[i+16] == 0x00 && xxbuffer[i+17] == 0x00 && xxbuffer[i+18] == 0x00 && xxbuffer[i+19] == 0x00)
	 	{
	 		sprintf(WIFI_SSID.WifiAddress[WIFI_SSID.WifiCnt],"%02X:%02X:%02X:%02X:%02X:%02X",xxbuffer[i+6],xxbuffer[i+7],xxbuffer[i+8],xxbuffer[i+9],xxbuffer[i+10],xxbuffer[i+11]);
			WIFI_SSID.Wifissidlength[WIFI_SSID.WifiCnt] = xxbuffer[i+44];
			memcpy(WIFI_SSID.Wifissid[WIFI_SSID.WifiCnt],xxbuffer+(i+48), WIFI_SSID.Wifissidlength[WIFI_SSID.WifiCnt] ); //SSID					
//			fprintf(stderr,"WIFI_SSID.WifiAddress[%d]=%s,%s,%d\n",WIFI_SSID.WifiCnt,WIFI_SSID.Wifissid[WIFI_SSID.WifiCnt],WIFI_SSID.WifiAddress[WIFI_SSID.WifiCnt],WIFI_SSID.Wifissidlength[WIFI_SSID.WifiCnt]);			
			WIFI_SSID.WifiCnt++;					 
			i = i+80;
	 	}
		i++;
  	}  
      free(xxbuffer);	
//      fprintf(stderr,"[%d] %d\n",__LINE__,	WIFI_SSID.WifiCnt);	
#endif

#if 1	
	SurveyBufferTmp[surveylength] = 0;
	i = 0;
	j = 0;
	while (i < surveylength)
	{
		if (i==0 && SurveyBufferTmp[i] == 0x0a)
		{
			j = 1;
			i++;
			continue;
		}
		if (SurveyBufferTmp[i] == 0x0a)
		{
			if (Surveyflag != 1)
			{
				Surveyflag = 1;
				j = i + 1;
				i++;
				continue;
			}
			strncpy(&SurveyBuffer[TmpCnt], &SurveyBufferTmp[j], i-j);
			strncpy(&SurveySSID[TmpCnt], &SurveyBufferTmp[j+4], 32); //SSID
			strncpy(&SurveyAddress[TmpCnt], &SurveyBufferTmp[j+37], 17); //BSSID(MAC)
			j = i + 1;
			TmpCnt++;
		}
		i++;
	}

	i = 0;
	j = 0;
	int k = 0, m = 0, setsuveyflag = 0;
	
	for (i=0;i<TmpCnt;i++)
	{
		for (j=0;j<WIFI_SSID.WifiCnt;j++)
		{
			if (strncasecmp(SurveyAddress[i], WIFI_SSID.WifiAddress[j], 17) == 0 &&
				WIFI_SSID.Wifissidlength[j] != 0)
			{
				setsuveyflag = 0;
				k = strlen(SurveySSID[i]);
				for (m=0;m<k;m++)
				{
					if (SurveySSID[i][m]==0x22 || SurveySSID[i][m]=='+' || SurveySSID[i][m]=='&' || SurveySSID[i][m]=='=' || SurveySSID[i][m]=='%')
					{
						setsuveyflag = 1;
						break;
					}
				}
				if (setsuveyflag == 0)
				{
					sprintf(Surveytruedata[TruedataCnt],"%s%2d",SurveyBuffer[i],WIFI_SSID.Wifissidlength[j]);
//					fprintf(stderr,"Surveytruedata[%d]=%s\n",TruedataCnt,Surveytruedata[TruedataCnt]);
					TruedataCnt++;
				}	
				break;
			}	
		}
	}	
	
	WifiGetSurveyData->GetSurveyCnt = TruedataCnt;
	memcpy(WifiGetSurveyData->WifiSurveyData, &Surveytruedata, sizeof(Surveytruedata));
#endif	
	
//	for(i=0;i<WifiGetSurveyData->GetSurveyCnt;i++)	fprintf(stderr,"WifiData[%d]=%s %s\n",i,WifiGetSurveyData->WifiSSID[i],WifiGetSurveyData->WifiSurveyData[i]);
	pthread_mutex_unlock(&ra_mutex);
//       fprintf(stderr,"[%d]\n",__LINE__);
	return 0;
}
#else
int net_get_site_survey_ra(WifiGetSurveyData_t *WifiGetSurveyData)//charles lin wifi2//charles lin wifi6
{
	int skfd = 0;
	struct iwreq wrq;

	char SurveyBuffer[40][256];
	char SurveyAddress[40][64];
	char SurveySSID[40][64];
	char SurveyCh[40][64];
	char SurveySecurity[40][64];
	char SurveySiganl[40][64];
	char SurveyWMode[40][64];
	char SurveyExtCH[40][64];
	char SurveyNT[40][64];
	char SurveyBufferTmp[4096];
	//
	char Surveytruedata[40][256];
	char Surveyssiddata[40][32];
	int  TruedataCnt,surveylength;
	FILE *fp;
	char getnumbuf[2048],getnumbuf2[2048];
	int  i = 0, j = 0, TmpCnt = 0, Surveyflag = 0;
   int  headOfCh, Ch_Len;
   int  headOfSSID, SSID_Len;
   int  headOfBSSID, BSSID_Len;
   int  headOfSecurity, Security_Len;
   int  headOfSiganl, Siganl_Len;
   int  headOfWMode, WMode_Len;
   int  headOfExtCH, ExtCH_Len;
   int  headOfNT, NT_Len;
   int  got_ssid_flag = 0;
   int wifiCnt = 0;

	TruedataCnt = 0;
	pthread_mutex_lock(&ra_mutex);

	memset(WifiGetSurveyData, 0, sizeof(WifiGetSurveyData));
	memset(SurveySSID, '\0', sizeof(SurveySSID));
	memset(SurveyBufferTmp, '\0', sizeof(SurveyBufferTmp));
	memset(SurveyBuffer, '\0', sizeof(SurveyBuffer));
	memset(SurveyAddress, '\0', sizeof(SurveyAddress));
	memset(Surveytruedata, '\0', sizeof(Surveytruedata));
	memset(Surveyssiddata, '\0', sizeof(Surveyssiddata));
	memset(SurveyCh, '\0', sizeof(SurveyCh));
	memset(SurveySecurity, '\0', sizeof(SurveySecurity));
	memset(SurveySiganl, '\0', sizeof(SurveySiganl));
	memset(SurveyWMode, '\0', sizeof(SurveyWMode));
	memset(SurveyExtCH, '\0', sizeof(SurveyExtCH));
	memset(SurveyNT, '\0', sizeof(SurveyNT));

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t) SurveyBufferTmp;
	wrq.u.data.length = 4096;
	wrq.u.data.flags = 0;

	if (ioctl(skfd, (0x8be0 + 0xd), &wrq) < 0)//RTPRIV_IOCTL_GSITESURVEY = 0x8be0 + 0xd
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
	surveylength = wrq.u.data.length ;
   ////-----------------------------------------------------------------------------------------------
//----
/*#if 1*/
	/*int ret = 0;*/

	/*char xxbuffer[4096];*/
	/*memset(xxbuffer, 0, sizeof(xxbuffer));*/
	/*int scanlength = 0;*/
	/*sprintf(wrq.ifr_name, "ra0");*/
	/*wrq.u.data.pointer = NULL;*/
	/*wrq.u.data.flags = 0;*/
	/*wrq.u.data.length = 0;*/
	/*if (ret = ioctl(skfd, SIOCSIWSCAN, &wrq) < 0)*/
	/*{*/
		/*close(skfd);*/
		/*pthread_mutex_unlock(&ra_mutex);*/
		/*return -1;*/
	/*}*/
	/*sprintf(wrq.ifr_name, "ra0");*/
	/*wrq.u.data.pointer = &xxbuffer;*/
	/*wrq.u.data.flags = 0;*/
	/*wrq.u.data.length = 4096;*/
	/*int  to_cnt = 0;*/
	/*usleep(25000);	*/
/*#if 1*/
	/*while((ret = ioctl(skfd, SIOCGIWSCAN, &wrq)) && to_cnt < 50)*/
	/*{*/
		/*to_cnt++;*/
		/*usleep(50000);//sleep 50 ms*/
	/*}*/
/*#endif*/

/*//	ret = ioctl(skfd, SIOCGIWSCAN, &wrq);*/
	/*if (ret == 0)*/
	/*{*/
		 /*scanlength = wrq.u.data.length;*/
	/*}*/
	/*else*/
	/*{*/
		/*fprintf(stderr,"\nsiocgiwscan == err\n");*/
		/*close(skfd);*/
		/*pthread_mutex_unlock(&ra_mutex);*/
		/*return -1;*/
	/*}*/
/*#endif*/

	/*//----*/
	/*close(skfd);*/
	/*xxbuffer[scanlength] = 0;*/
	/*WIFI_SSID_t WIFI_SSID;*/
	/*i = 0;*/
	/*j = 0;*/
	/*memset(&WIFI_SSID, 0, sizeof(WIFI_SSID_t));*/

	/*while (i < scanlength)*/
	/*{*/
		 /*if (xxbuffer[i] == 0x14 && xxbuffer[i+1] == 0x00 */
            /*&& xxbuffer[i+2] == 0x15 && xxbuffer[i+3] == 0x8B */
            /*&& xxbuffer[i+4] == 0x01 && xxbuffer[i+5] == 0x00 */
            /*&& xxbuffer[i+14] == 0x00 && xxbuffer[i+15] == 0x00 */
            /*&& xxbuffer[i+16] == 0x00 && xxbuffer[i+17] == 0x00 */
            /*&& xxbuffer[i+18] == 0x00 && xxbuffer[i+19] == 0x00)*/
		 /*{*/
			 /*sprintf(WIFI_SSID.WifiAddress[WIFI_SSID.WifiCnt],"%02X:%02X:%02X:%02X:%02X:%02X",xxbuffer[i+6],xxbuffer[i+7],xxbuffer[i+8],xxbuffer[i+9],xxbuffer[i+10],xxbuffer[i+11]);*/
			/*WIFI_SSID.Wifissidlength[WIFI_SSID.WifiCnt] = xxbuffer[i+44];*/
			/*//fprintf(stderr,"\nWIFI_SSID.WifiAddress[%d]=%s,%d\n",WIFI_SSID.WifiCnt,WIFI_SSID.WifiAddress[WIFI_SSID.WifiCnt],WIFI_SSID.Wifissidlength[WIFI_SSID.WifiCnt]);*/
			/*WIFI_SSID.WifiCnt++;*/
			/*i = i+80;*/
		 /*}*/
		/*i++;*/
     /*}*/
   ////-----------------------------------------------------------------------------------------------

	i = 0;
	j = 0;
	
	SurveyBufferTmp[surveylength] = 0;
   fprintf(stderr, "------------%s--------------\n", SurveyBufferTmp);

	while (i < surveylength)
	{
		if (i==0 && SurveyBufferTmp[i] == 0x0a)
		{
			j = 1;
			i++;
			continue;
		}
      if(!strncmp(SurveyBufferTmp+i, "Ch", 2)){
         headOfCh = i;
         /*printf("headOfCh = %d\n", headOfCh);*/
      }else if(!strncmp(SurveyBufferTmp+i, "SSID", 4) && !got_ssid_flag){
         got_ssid_flag = 1;
         headOfSSID = i;
         /*printf("headOfSSID = %d\n", headOfSSID);*/
      }else if(!strncmp(SurveyBufferTmp+i, "BSSID", 5)){
         headOfBSSID = i;
         /*printf("headOfBSSID = %d\n", headOfBSSID);*/
      }else if(!strncmp(SurveyBufferTmp+i, "Security", 8)){
         headOfSecurity = i;
         /*printf("headOfSecurity = %d\n", headOfSecurity);*/
      }else if(!strncmp(SurveyBufferTmp+i, "Siganl", 6)){
         headOfSiganl = i;
         /*printf("headOfSiganl = %d\n", headOfSiganl);*/
      }else if(!strncmp(SurveyBufferTmp+i, "W-Mode", 6)){
         headOfWMode = i;
         /*printf("headOfWMode = %d\n", headOfWMode);*/
      }else if(!strncmp(SurveyBufferTmp+i, "ExtCH", 5)){
         headOfExtCH = i;
         /*printf("headOfExtCH = %d\n", headOfExtCH);*/
      }else if(!strncmp(SurveyBufferTmp+i, "NT", 2)){
         headOfNT = i;
         /*printf("headOfNT = %d\n", headOfNT);*/
      } 

		if (SurveyBufferTmp[i] == 0x0a)
		{
			if (Surveyflag != 1)
			{
            Ch_Len = headOfSSID - headOfCh;
            SSID_Len = headOfBSSID - headOfSSID;
            BSSID_Len = headOfSecurity - headOfBSSID;
            Security_Len = headOfSiganl - headOfSecurity;
            Siganl_Len = headOfWMode - headOfSiganl;
            WMode_Len = headOfExtCH - headOfWMode;
            ExtCH_Len = headOfNT - headOfExtCH;
            NT_Len = 2;
            /*********DEBUG***********************************
            printf("Ch_Len = %d\n", Ch_Len);
            printf("SSID_Len = %d\n", SSID_Len);
            printf("BSSID_Len = %d\n", BSSID_Len);
            printf("Security_Len = %d\n", Security_Len);
            printf("Siganl_Len = %d\n", Siganl_Len);
            printf("WMode_Len = %d\n", WMode_Len);
            printf("ExtCH_Len = %d\n", ExtCH_Len);
            printf("NT_Len = %d\n", NT_Len);
            ******************************************/
				Surveyflag = 1;
				j = i + 1;
				i++;
				continue;
			}
         /*strncpy(&SurveyBuffer[TmpCnt], &SurveyBufferTmp[j], i-j);*/
         /*strncpy(&SurveyAddress[TmpCnt], &SurveyBufferTmp[j+37], 17);*/
         /*strncpy(&SurveySSID[TmpCnt], &SurveyBufferTmp[j+4], 32);*/
         strncpy(&SurveyCh[TmpCnt], &SurveyBufferTmp[j+headOfCh-1], Ch_Len);
         strncpy(&SurveySSID[TmpCnt], &SurveyBufferTmp[j+headOfSSID-1], SSID_Len);
         strncpy(&SurveyAddress[TmpCnt], &SurveyBufferTmp[j+headOfBSSID-1], BSSID_Len);
         strncpy(&SurveySecurity[TmpCnt], &SurveyBufferTmp[j+headOfSecurity-1], Security_Len);
         strncpy(&SurveySiganl[TmpCnt], &SurveyBufferTmp[j+headOfSiganl-1], Siganl_Len);
         strncpy(&SurveyWMode[TmpCnt], &SurveyBufferTmp[j+headOfWMode-1], WMode_Len);
         strncpy(&SurveyExtCH[TmpCnt], &SurveyBufferTmp[j+headOfExtCH-1], ExtCH_Len);
         strncpy(&SurveyNT[TmpCnt], &SurveyBufferTmp[j+headOfNT-1], NT_Len);
			j = i + 1;
			TmpCnt++;
         if(TmpCnt >= 36)  break;
		}
		i++;
	}

   int h;
   for(i=0;i<TmpCnt /*WIFI_SSID.WifiCnt*/;i++){
      printf("-------------------------------------\n");
      for(h=strlen(SurveyCh+i)-1;h>=0;h--){
         if(SurveyCh[i][h] != ' ' && SurveyCh[i][h] != '\n'){
            SurveyCh[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveyCh = %s\n", SurveyCh[i]);
      for(h=strlen(SurveySSID+i)-1;h>=0;h--){
         if(SurveySSID[i][h] != ' ' && SurveySSID[i][h] != '\n'){
            SurveySSID[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveySSID = %s\tlen = %d\n", SurveySSID[i], strlen(SurveySSID[i]));
      for(h=strlen(SurveyAddress+i)-1;h>=0;h--){
         if(SurveyAddress[i][h] != ' ' && SurveyAddress[i][h] != '\n'){
            SurveyAddress[i][h+1] = '\0';
            break;
         }
      }
      printf("[%s][%d] SurveyAddress = %s\tlen = %d\n", __FILE__, __LINE__, SurveyAddress[i],strlen(SurveyAddress[i]));
      for(h=strlen(SurveySecurity+i)-1;h>=0;h--){
         if(SurveySecurity[i][h] != ' ' && SurveySecurity[i][h] != '\n'){
            SurveySecurity[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveySecurity = %s\n", SurveySecurity[i]);
      for(h=strlen(SurveySiganl+i)-1;h>=0;h--){
         if(SurveySiganl[i][h] != ' ' && SurveySiganl[i][h] != '\n'){
            SurveySiganl[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveySiganl = %s\n", SurveySiganl[i]);
      for(h=strlen(SurveyWMode+i)-1;h>=0;h--){
         if(SurveyWMode[i][h] != ' ' && SurveyWMode[i][h] != '\n'){
            SurveyWMode[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveyWMode = %s\n", SurveyWMode[i]);
      for(h=strlen(SurveyExtCH+i)-1;h>=0;h--){
         if(SurveyExtCH[i][h] != ' ' && SurveyExtCH[i][h] != '\n'){
            SurveyExtCH[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveyExtCH = %s\n", SurveyExtCH[i]);
      for(h=strlen(SurveyNT+i)-1;h>=0;h--){
         if(SurveyNT[i][h] != ' ' && SurveyNT[i][h] != '\n'){
            SurveyNT[i][h+1] = '\0';
            break;
         }
      }
      printf("SurveyNT = %s\n", SurveyNT[i]);
   }

	i = 0;   j = 0;
   int rank = 1;
   int *index = (int *)malloc(TmpCnt*sizeof(int));
   memset(index, 0, sizeof(index));
	for(i=0;i<TmpCnt;i++)   index[i] = -1;
   while(1){
      if(j >= TmpCnt) break;
      switch(rank){
         case 1:
            for(i=0;i<TmpCnt;i++){
               if(atoi(SurveySiganl[i]) >= 90 && atoi(SurveySiganl[i]) <= 100){
                  index[j] = i;
                  j++;
               }
            }
            rank++;
            break;
         case 2:
            for(i=0;i<TmpCnt;i++){
               if(atoi(SurveySiganl[i]) >= 60 && atoi(SurveySiganl[i]) <= 89){
                  index[j] = i;
                  j++;
               }
            }
            rank++;
            break;
         case 3:
            for(i=0;i<TmpCnt;i++){
               if(atoi(SurveySiganl[i]) >= 30 && atoi(SurveySiganl[i]) <= 59){
                  index[j] = i;
                  j++;
               }
            }
            rank++;
            break;
         case 4:
            for(i=0;i<TmpCnt;i++){
               if(atoi(SurveySiganl[i]) >= 0 && atoi(SurveySiganl[i]) <= 29){
                  index[j] = i;
                  j++;
               }
            }
            rank++;
            break;
         default:
            break;
      }
   }

   /*********DEBUG**************************************************
   //for(i=0;i<TmpCnt;i++){
   //   fprintf(stderr, "[%s][%d] index[%d] = %d\n", __FILE__, __LINE__, i, index[i]);
   //}
   //fprintf(stderr, "[%s][%d] j = %d\n", __FILE__, __LINE__, j);
   ************************************************************/


	int k = 0, m = 0, setsuveyflag = 0;
	i = 0;   j = 0;   m = 0;
	
	for (i=0;i<TmpCnt;i++)
	{
      sprintf(Surveytruedata[TruedataCnt],"%s|%s|%s|%s|%s|%s|%s"
            ,SurveyCh[index[i]]
            ,SurveyAddress[index[i]]
            ,SurveySecurity[index[i]]
            ,SurveySiganl[index[i]]
            ,SurveyWMode[index[i]]
            ,SurveyExtCH[index[i]]
            ,SurveyNT[index[i]]
            );
      sprintf(Surveyssiddata[TruedataCnt], "%s", SurveySSID[index[i]]);
      printf("[%s][%d]MIKE Surveytruedata = %s  Surveyssiddata = %s\n", __FILE__, __LINE__, Surveytruedata[TruedataCnt], Surveyssiddata[TruedataCnt]);
      TruedataCnt++;
   }
	
   printf("[%s][%d]MIKE TruedataCnt = %d\n", __FILE__, __LINE__, TruedataCnt);
	WifiGetSurveyData->GetSurveyCnt = TruedataCnt;
	memcpy(WifiGetSurveyData->WifiSurveyData, &Surveytruedata, sizeof(Surveytruedata));
	memcpy(WifiGetSurveyData->WifiSSID, &Surveyssiddata, sizeof(Surveyssiddata));
	pthread_mutex_unlock(&ra_mutex);

	return 0;
}
#endif //// end WIFI_DRV_VER_RT3070

//alice_modify_2012_3_29_v1

//alice_modify_2012_3_27_v1
#define RT_OID_802_11_RSSI      0x0108 
#define RT_OID_802_11_RSSI_1    0x0109
#define RT_OID_802_11_RSSI_2    0x010A
#define RT_PRIV_IOCTL			(SIOCIWFIRSTPRIV + 0x0E)  
//----------------------------------------------------------------
//alice_modify_2012_5_10_v1 
int net_get_rssi (WifiRSSI_t *WifiRSSI)
{
	int skfd = 0,rval;
	struct iwreq wrq;

	//int rssi1,rssi2,rssi3;

	memset(WifiRSSI, 0, sizeof(WifiRSSI_t));
	pthread_mutex_lock(&ra_mutex);
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
//	
	memset(&wrq , 0, sizeof(wrq));
	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t)&WifiRSSI->RSSI1;
	wrq.u.data.length = sizeof(signed int);
	wrq.u.data.flags = RT_OID_802_11_RSSI;
	
	if (ioctl(skfd, (RT_PRIV_IOCTL), &wrq) < 0)//RTPRIV_IOCTL_GSITESURVEY = 0x8be0 + 0xd
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
	fprintf(stderr, "\nrssi1 =%d\n", WifiRSSI->RSSI1);
//
	memset(&wrq , 0, sizeof(wrq));
	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t)& WifiRSSI->RSSI2;
	wrq.u.data.length = sizeof(signed int);
	wrq.u.data.flags = RT_OID_802_11_RSSI_1;

	if (ioctl(skfd, (RT_PRIV_IOCTL), &wrq) < 0)//RTPRIV_IOCTL_GSITESURVEY = 0x8be0 + 0xd
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
	fprintf(stderr,"\nrssi2 =%d\n",WifiRSSI->RSSI2);
//
	memset(&wrq , 0, sizeof(wrq));
	strcpy(wrq.ifr_name, "ra0");
	wrq.u.data.pointer = (caddr_t)& WifiRSSI->RSSI3;
	wrq.u.data.length = sizeof(signed int);
	wrq.u.data.flags = RT_OID_802_11_RSSI_2;

	if(ioctl(skfd, (RT_PRIV_IOCTL), &wrq) < 0)//RTPRIV_IOCTL_GSITESURVEY = 0x8be0 + 0xd
	{
		fprintf(stderr, "%s %s() - error!(ioctl)\n", __FILE__, __func__);
		perror("ioctl");
		close(skfd);
		pthread_mutex_unlock(&ra_mutex);
		return -1;
	}
	fprintf(stderr, "\nrssi3 =%d\n", WifiRSSI->RSSI3);

	close(skfd);
	pthread_mutex_unlock(&ra_mutex);

	return 0;
}
//alice_modify_2012_5_10_v1 

//----------------------------------------------------------------
/**
* @brief check ethernet speed
*
* @param    data			output string by getting sitesurvey of IOCTL DOC said.
* @param    buf			output string by wlist scan.
* @param    buf_length	the length of the output string by wlist scan.
*
* @return    ESSID's length, if it's no finding then value = -1
*/
int find_ssid_length(char* data, char* buf, int buf_length) // charles lin wifi2
{
    char addr[20] = {0};
    char addr2[20] = {0};
    int curr_index = 0;// or 44
    unsigned short event_len = 0;
    unsigned short event_id = 0;//0x8b15 mac, 0x8b1b essid
    int isFind = 0;//if find MAC address or not

    snprintf(addr, 18, "%s", &data[37]);

    while (curr_index < buf_length)
    {
        memcpy(&event_len, &buf[curr_index], 2);
        memcpy(&event_id, &buf[curr_index + 2], 2);

        if (isFind) // for find the ESSID's length
        {
            if (event_id == 0x8b1b)
                return (int)(buf[curr_index + 4]);
        }
        else // for find the MAC address
        {
            if (event_id == 0x8b15)
            {
                memset(addr2, 0, 20);
                sprintf(addr2, "%02x:%02x:%02x:%02x:%02x:%02x", buf[curr_index+6], buf[curr_index+7], buf[curr_index+8], buf[curr_index+9], buf[curr_index+10], buf[curr_index+11]);

                if (!strcmp(addr, addr2))
                    isFind = 1;
            }
        }
        curr_index += event_len;
    }
    return -1;
}
#endif //#if WIFI_FEATURE

static pthread_mutex_t eth_mutex;
void net_init_eth()
{
	if (pthread_mutex_init(&eth_mutex, NULL) != 0)
		fprintf(stderr, "%s %s() - error!\n", __FILE__, __func__);
}
//alice_modify_2012_4_27_v1

/***************************************************************************
 * Set a certain interface flag.                                           *
 ***************************************************************************/
int net_set_flag(char *ifname, short flag)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		DBG_ERR("net_set_flag: ioctl SIOCGIFFLAGS");
		close(skfd);
		return (-1);
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags |= flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		DBG_ERR("net_set_flag: ioctl SIOCSIFFLAGS");
		close(skfd);
		return -1;
	}
	close(skfd);
	return (0);
}

/***************************************************************************
 * Clear a certain interface flag.                                         *
 ***************************************************************************/
int net_clr_flag(char *ifname, short flag)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		DBG_ERR("net_clr_flag: ioctl SIOCGIFFLAGS");
		close(skfd);
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_flags &= ~flag;
	if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
		DBG_ERR("net_clr_flag: ioctl SIOCSIFFLAGS");
		close(skfd);
		return -1;
	}
	close(skfd);
	return (0);
}

/***************************************************************************
 * Get a interface flag.                                                   *
 ***************************************************************************/
int net_get_flag(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		DBG_ERR("net_get_flag: ioctl SIOCGIFFLAGS");
		close(skfd);
		return -1;
	}
	close(skfd);
	return ifr.ifr_flags;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_nic_up(char *ifname)
{
	return net_set_flag(ifname, (IFF_UP | IFF_RUNNING));
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_nic_down(char *ifname)
{
	return net_clr_flag(ifname, IFF_UP);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
in_addr_t net_get_ifaddr(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		DBG_ERR("net_get_ifaddr: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
struct in_addr net_get_ip(int skfd, char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		DBG_ERR("net_get_ip: ioctl SIOCGIFADDR");
		return (struct in_addr){-1};
	}
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_set_ifaddr(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0) {
		DBG_ERR("net_set_ifaddr: ioctl SIOCSIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
in_addr_t net_get_netmask(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		DBG_ERR("net_get_netmask: ioctl SIOCGIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_set_netmask(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}
	sa.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &sa, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) {
		DBG_ERR("net_set_netmask: ioctl SIOCSIFNETMASK");
		close(skfd);
		return -1;
	}
	close(skfd);
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_get_hwaddr(char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		DBG_ERR("net_get_hwaddr: ioctl SIOCGIFHWADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_add_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;

	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		DBG_ERR("socket error");
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCADDRT, &rt) < 0) {
		DBG_ERR("net_add_gateway: ioctl SIOCADDRT");
		close(skfd);
		return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_del_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;

	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct rtentry));

	/* Fill in the other fields. */
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);

	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	sa.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &sa, sizeof(struct sockaddr));

	/* Create a socket to the INET kernel. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		DBG_ERR("socket error");
		return -1;
	}
	/* Tell the kernel to accept this route. */
	if (ioctl(skfd, SIOCDELRT, &rt) < 0) {
		DBG_ERR("net_del_gateway: ioctl SIOCDELRT");
		close(skfd);
	return -1;
	}
	/* Close the socket. */
	close(skfd);
	return (0);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_set_dns(char *dnsname)
{
	FILE *fp;

	fp = fopen(RESOLV_CONF, "w");
	if ( fp ) {
		fprintf(fp, "nameserver %s\n", dnsname);
		fclose(fp);
		// DBG_NET("dns=%s\n", dnsname);
		return 0;
	}
   NET_CONFIG_DBG("[%s][%s][%d] open %s failed!\n", __FILE__, __func__, __LINE__, RESOLV_CONF);
   perror("OPEN");
	DBG("net_set_dns: file \"%s\" opened for writing error!\n", RESOLV_CONF);
	return -1;
}

/***************************************************************************
 *                                                                                    *
 ***************************************************************************/
int net_set_dns2(char *dnsname)
{
	FILE *fp;
	int file_length = 0;
	char *file_buf, *buf_1, *buf_2;
	char dns_1[80] = "";
   NET_CONFIG_DBG("[%s][%s][%d] MIKE_IS_HERE\n", __FILE__, __func__, __LINE__);

	fp = fopen(RESOLV_CONF, "r");
	if (fp == NULL){
      NET_CONFIG_DBG("[%s][%s][%d] open %s failed!\n", __FILE__, __func__, __LINE__, RESOLV_CONF);
      perror("OPEN");
		return -1;
   }

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);

  	file_buf = (char *)malloc(sizeof(char) * file_length);

	rewind(fp);
	fread(file_buf, 1, file_length, fp);
	fclose(fp);

	unlink(RESOLV_CONF);
	fp = fopen(RESOLV_CONF, "w");

	if (fp)
	{
		if (NULL != (buf_1 = strstr(file_buf, "nameserver ")))
		{
			buf_1 += 11;
			if (NULL != (buf_2 = strstr(buf_1, "\n")))
			{
				memcpy(dns_1, buf_1, buf_2 - buf_1);

				fprintf(fp, "nameserver %s\n", dns_1);
				fprintf(fp, "nameserver %s\n", dnsname);

				free(file_buf);
				fclose(fp);
				// DBG_NET("dns2=%s\n", dnsname);
				return 0;
			}
		}
	}

	DBG("net_set_dns2: file \"%s\" opened for writing error!\n", RESOLV_CONF);
	free(file_buf);
	return -1;
}


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
in_addr_t net_get_dns(void)
{
	FILE *fp;
	char dnsname[80];

	fp = fopen(RESOLV_CONF, "r");
	if (fp)
	{
		if (fscanf(fp, "nameserver %s\n", dnsname) != EOF)
		{
			fclose(fp);
//			DBG_NET("dns=%s\n", dnsname);
			return inet_addr(dnsname);
		}
		DBG("net_get_dns: scan fail!\n");
	}
	DBG("net_get_dns: file \"%s\" opened for reading error!\n", RESOLV_CONF);
	return INADDR_ANY;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
in_addr_t net_get_dns2(void)
{
   FILE *fp;
   int file_len = 0;
   char *buf_1, *buf_2, *buf_3, *file_buf;
   char dnsname2[80];
   memset(dnsname2, '\0', sizeof(dnsname2));

   fp = fopen(RESOLV_CONF, "r");
   if (fp == NULL)
      return -1;

   fseek(fp, 0, SEEK_END);
   file_len = ftell(fp);

   file_buf = (char *)malloc(sizeof(char) * file_len);

   rewind(fp);
   fread(file_buf, 1, file_len, fp);

   if (fp)
   {
      if (NULL != (buf_1 = strstr(file_buf, "nameserver ")))
      {
         buf_1 += 11;
         if (NULL != (buf_2 = strstr(buf_1, "nameserver ")))
         {
            buf_2 += 11;
            if (NULL != (buf_3 = strstr( buf_2, "\n" )) )
            {
               memcpy(dnsname2, buf_2, buf_3 - buf_2);

               fclose(fp);
               // DBG_NET("dns2=%s\n", dnsname2);

               free(file_buf);
               return inet_addr(dnsname2);
            }
         }
      }
     fclose(fp);
      //DBG("net_get_dns2: scan fail!\n");
   }

//   NET_CONFIG_DBG("[%s][%d] net_get_dns: file \"%s\" opened for reading error!\n", __FILE__, __LINE__, RESOLV_CONF);
   //DBG("net_get_dns2: file \"%s\" opened for reading error!\n", RESOLV_CONF);
   free(file_buf);
   return INADDR_ANY;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
pid_t net_start_dhcpcd(char *ifname)
{
	pid_t pid;

	if( (pid = vfork()) == 0 ) {
		/* the child */
		execlp(DHCPC_EXEC_PATH, DHCPC_EXEC_PATH, ifname, NULL);
		DBG_ERR("exec dhcpcd failed");
		_exit(0);
	}
	else if(pid < 0) {
		DBG_ERR("fork dhcpcd failed");
		return -1;
	}
	return pid;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_renew_dhcpcd(pid_t pid)
{
	if ( pid > 0 ) {
		if ( kill(pid, SIGALRM) == 0 )
			return 0;
		DBG_ERR("net_disable_dhcpcd, kill error");
	}
	DBG("net_disable_dhcpcd, pid error\n");
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_enable_dhcpcd(char* ifname, char *hostname, int ipmode)
{
	char tmp_str[128];
	if (ifname == NULL || hostname == NULL) return -1;

//	tmp_str = (char*)malloc(128);
//	if (tmp_str == NULL)
//		return -1;

	system("killall -9 dhcpcd"); 
	unlink("/tmp/dhcpc/dhcpcd-eth0.pid");
//	system("killall -9 dhclient");  //  add by kenny chiu 20120604
//	usleep(200000);
//	sprintf(tmp_str, "/opt/ipnc/dhcpcd/dhclient -6 -x %s &", ifname);
//	system(tmp_str);

	sprintf(tmp_str, "%s -t 20 %s -h %s &", DHCPC_EXEC_PATH, ifname, hostname);
	fprintf(stderr,"tmp_str=%s\n",tmp_str);
	system(tmp_str);
	if (ipmode == 1) // DHCP (ipv4 and ipv6)
	{
//		sprintf(tmp_str, "/opt/ipnc/dhcpcd/dhclient -6 -lf /tmp/test6 -sf /opt/ipnc/linux %s &", ifname);
//		system(tmp_str);
	}
//	free(tmp_str);
	return 0;
}

/***************************************************************************
 * move by Joseph  2014-04                                                    *
 ***************************************************************************/
int net_disable_dhcpcd(char *ifname)
{
	char *tmp_str = NULL;
	int  pid;
//	struct in6_addr addr;
	if (ifname == NULL)
		return -1;
	tmp_str = (char*)malloc(128);
	if (tmp_str == NULL)
		return -1;
//	sprintf(tmp_str, "/opt/ipnc/dhcpcd/dhclient -6 -x %s &", ifname);
//	system(tmp_str);

	sprintf(tmp_str, "/tmp/dhcpc/dhcpcd-%s.pid", ifname);
	FILE *fp = fopen(tmp_str, "r");
	if (fp != NULL)
	{
		fscanf(fp, "%d", &pid);
		fclose(fp);

		if (pid > 0)
		{
			unlink(tmp_str);
			free(tmp_str);
			return kill(pid, SIGKILL);
		}
	}
	free(tmp_str);
	return -1;
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_search_gateway(char *buf, in_addr_t *gate_addr)
{
	char iface[16];
	unsigned long dest, gate;
	int iflags;

	sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
	// DBG_NET("net_search_gateway:%s, %lX, %lX, %X\n", iface, dest, gate, iflags);
	if ( (iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY) ) {
		*gate_addr = gate;
		return 0;
	}
	return -1;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_set_gateway(in_addr_t addr)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) {
		DBG_ERR(PROCNET_ROUTE_PATH);
		DBG("INET (IPv4) not configured in this system.\n");
		return -1;
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) {
//	fprintf(stderr,"net_set_gateway 111  buff=%s\n",buff);
		if (net_search_gateway(buff, &gate_addr) == 0) {
			net_del_gateway(gate_addr);
		}
	}
	fclose(fp);

	return net_add_gateway(addr);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_clean_gateway(void)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) {
		DBG_ERR(PROCNET_ROUTE_PATH);
		DBG("INET (IPv4) not configured in this system.\n");
		return -1;
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) {
		if (net_search_gateway(buff, &gate_addr) == 0) {
			net_del_gateway(gate_addr);
		}
	}
	fclose(fp);

	return 0;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
in_addr_t net_get_gateway(void)
{
	in_addr_t gate_addr;
	char buff[132];
	FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");

	if (!fp) {
		DBG_ERR(PROCNET_ROUTE_PATH);
		DBG("INET (IPv4) not configured in this system.\n");
		return (INADDR_ANY);
	}
	fgets(buff, 130, fp);
	while (fgets(buff, 130, fp) != NULL) {
		if (net_search_gateway(buff, &gate_addr) == 0) {
			fclose(fp);
			return gate_addr;
		}
	}
	fclose(fp);
	return (INADDR_ANY);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_get_ifaddr_v6(const char *ifname, struct in6_addr *ifaddr)
{
	FILE *fp = fopen(PROC_IFINET6_PATH, "r");
	char addrstr[INET6_ADDRSTRLEN];
	char seg[8][5];
	int index, plen, scope, flags;
	char ifn[IFNAMSIZ];
	int ret = -1;

	if (fp)
	{
		while (fscanf(fp, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %s\n"
						, seg[0], seg[1], seg[2], seg[3], seg[4], seg[5], seg[6]
						, seg[7], &index, &plen, &scope, &flags, ifn) != EOF)
		{
			if (strcmp(ifn, ifname) == 0 && scope == 0)
			{
                sprintf(addrstr, "%s:%s:%s:%s:%s:%s:%s:%s", seg[0], seg[1],
                        seg[2], seg[3], seg[4], seg[5], seg[6], seg[7]);
                ret = inet_pton(AF_INET6, addrstr, ifaddr);
                goto out;
			}
		}
		errno = ENXIO;
out:
        fclose(fp);
  	}
	return ret;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int net_get_info(char *ifname, struct NET_CONFIG *netcfg)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		DBG_ERR("socket error");
		return -1;
	}

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		DBG_ERR("net_get_info: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	netcfg->ifaddr = saddr->sin_addr.s_addr;
	DBG_NET("ifaddr=0x%x\n", netcfg->ifaddr);

	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		DBG_ERR("net_get_info: ioctl SIOCGIFNETMASK");
		close(skfd);
		return -1;
	}
	netcfg->netmask = saddr->sin_addr.s_addr;
	DBG_NET("netmask=0x%x\n", netcfg->netmask);

	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		DBG_ERR("net_get_info: ioctl SIOCGIFHWADDR");
		close(skfd);
		return -1;
	}
	memcpy(netcfg->mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	DBG_NET("hwaddr=%02x:%02x:%02x:%02x:%02x:%02x\n", netcfg->mac[0],netcfg->mac[1],
					netcfg->mac[2],netcfg->mac[3],netcfg->mac[4],netcfg->mac[5]);

	close(skfd);
	netcfg->gateway = net_get_gateway();
	DBG_NET("gateway=0x%x\n", netcfg->gateway);
	netcfg->dns = net_get_dns();
	return 0;

}
/**
* @brief check ethernet status
*
* @param pInterface [I] ethernt device name. Ex: "eth0" "eth1"
* @return Link status
* @retval 0  Linkdown
* @retval 1  Linkup
* @retvl -1  Error
*/
int Check_Link_status( char *pInterface )
{
	int skfd = 0;
	struct ifreq ifr;	

//	pthread_mutex_lock(&eth_mutex);  
	
#if 1	
 	struct mii_data mii;

    ifr.ifr_data = (char *) &mii;
	mii.reg_num = MII_BMSR;      

    /* Get the vitals from the interface. */	
	strncpy(ifr.ifr_name, pInterface, IFNAMSIZ);

    /* Open a basic socket. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror( "socket" );
		return -1;
	}

#if 0
    ra_mii_ioctl_data mii;
//    int i; 
//	for(i=0;i<5;i++) {
    mii.phy_id = 0x04; //port number 0 to 4 for MTK7620  4:WAN
//    mii.phy_id = 0x00; //port number 0 to 4 for MTK7628 0:WAN  
    if (ioctl(skfd, RAETH_MII_READ, &ifr) < 0)   {  fprintf(stderr, "RAETH_MII_READ on %s failed: %s\n", ifr.ifr_name, strerror(errno)); }	
//	else fprintf(stderr, "[%d]mii.val_out 0x%x\n",__LINE__ ,mii.val_out);	
//    }
#else
	if( ioctl( skfd, SIOCGMIIPHY, &ifr ) < 0 )
	{
	    if (errno != ENODEV) fprintf(stderr, "SIOCGMIIPHY on '%s' failed: %s\n",  pInterface, strerror(errno));
		close( skfd );
		return -1;
	}
	if( ioctl( skfd, SIOCGMIIREG, &ifr ) < 0 )
	{
        fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name, strerror(errno));
		close( skfd );
		return -1;
	}
#endif	
	close( skfd );
//	pthread_mutex_unlock(&eth_mutex); 
	return (mii.val_out & MII_BMSR_LINK_VALID) ? 1 : 0;
	
#else
	emac_drv_priv_ioctl priv_ioctl;
	emac_phy_params phy_params;

	/* DM365 private use */
	phy_params.reg_addr = 1;
	phy_params.phy_num = 1;
	priv_ioctl.cmd = EMAC_PRIV_MII_READ;
	priv_ioctl.data = &phy_params;
	ifr.ifr_data = (char *)&priv_ioctl;
	if (ioctl(skfd, SIOCDEVPRIVATE, &ifr) < 0)
	{
		perror( "ioctl" );
		close( skfd );
		return -1;
	}
	close(skfd);
	pthread_mutex_unlock(&eth_mutex); //alice_modify_2012_4_27_v1
	return (phy_params.data & 4) ? 1 : 0;
#endif
}

//#if WIFI_FEATURE 

/**
* @brief check ethernet speed
*
* @param pInterface [I] ethernt device name. Ex: "eth0" "eth1"
* @return Ethernet speed
* @retval 0  10M
* @reval 1  100M
* @retval -1  Error
*/
int Check_Link_speed( char *pInterface )
{
	int skfd = 0;
	struct ifreq ifr;
	struct mii_data* mii = NULL;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror( "socket" );
		return -1;
	}

	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, pInterface, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	if (ioctl( skfd, SIOCGMIIPHY, &ifr ) < 0)
	{
		perror( "ioctl" );
		return -1;
	}

	mii = (struct mii_data*)&ifr.ifr_data;
	mii->reg_num = 0; //sPHY_register 0
	if (ioctl( skfd, SIOCGMIIREG, &ifr ) < 0)
	{
	  	perror( "ioctl2" );
	  	return -1;
	}
	close( skfd );
	if (mii->val_out & (1<<13)) //scheck bit13 of PHY_REG0
	{
		//printf( "100M\n" );
		return 1;
	}
	else
	{
		//printf( "10M\n" );
		return 0;
	}

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
//#define NET_TEST
#ifdef NET_TEST
int main(int argc, char **argv)
{
	struct NET_CONFIG netcfg;
	int i;

	if (argc < 3) {
		net_get_info("eth0", &netcfg);
		return 0;
	}

	for (i=1; i<argc; i++)
		printf("arg[%d]=%s\n", i, argv[i]);

	if (!strcmp(argv[2], "up"))
		net_set_flag(argv[1], (IFF_UP | IFF_RUNNING));
	else if (!strcmp(argv[2], "down"))
		net_clr_flag(argv[1], IFF_UP);
	else if (!strcmp(argv[2], "ip")) {
		net_set_ifaddr(argv[1], inet_addr(argv[3]));
	}
	else if (!strcmp(argv[2], "netmask")) {
		net_set_netmask(argv[1], inet_addr(argv[3]));
	}
	else if (!strcmp(argv[2], "gateway")) {
		if (!strcmp(argv[1], "add"))
			net_add_gateway(inet_addr(argv[3]));
		else
			net_del_gateway(inet_addr(argv[3]));
	}
	else if (!strcmp(argv[2], "dhcpcd")) {
		if (!strcmp(argv[3], "enable"))
			net_enable_dhcpcd(argv[1]);
		else
			net_disable_dhcpcd(argv[1]);
	}
	else if (!strcmp(argv[1], "dns"))
		net_set_dns(argv[2]);
	else
		printf("unknown argument!\n");
	return 0;
}
#endif
