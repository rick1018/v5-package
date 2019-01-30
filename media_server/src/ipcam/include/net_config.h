// net_config.h
//
// Last changed:   July 23, 2003
//
// Copyright (c) 2003 Appro Technology, Inc.
// Author: Javier Hsiao <javier@approtech.com>
#if defined (__cplusplus)
extern "C" {
#endif

#include <paths.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <linux/types.h>


#define DHCPC_EXEC			"dhcpcd"
#define DHCPC_EXEC_PATH			"/bin/dhcpcd/"DHCPC_EXEC
//#define DHCPC_EXEC_PATH			"udhcpc"
#define DHCPC_PID_FILE_PATH		""_PATH_VARRUN""DHCPC_EXEC"-%s.pid"
//#define DHCPC_PID_FILE_PATH		""_PATH_VARRUN"dhcpcd-%s.pid"
#define RESOLV_CONF				"/etc/resolv.conf"
//#define RESOLV_CONF				"/mnt/resolv.conf"
//#define RESOLV_CONF				"/var/dhcpc/resolv.conf"
#define PROCNET_ROUTE_PATH		"/proc/net/route"

#define NET_CONFIG_ERR(fmt, args...)     fprintf(stderr, "[ERR] " fmt, ## args)
#define NET_CONFIG_DBG(fmt, args...)     fprintf(stderr, "[DBG] " fmt, ## args)

#define NET_STATUS_MASK 		0x0004
#define ETH_MII_REG 			0x01
#define ETH_LINKUP 				1
#define ETH_LINKDOWN 			0
#define ETH_ERROR 				-1

struct NET_CONFIG {
	unsigned char mac[6];
	in_addr_t	ifaddr;
	in_addr_t	netmask;
	in_addr_t	gateway;
	in_addr_t	dns;
};

#define MAX_AP_NUM  100
typedef struct
{
	int			GetSurveyCnt;
	char		WifiSurveyData[MAX_AP_NUM][256]; 
   char     WifiSSID[MAX_AP_NUM][32];
   char     wifi_chip_vendor;
}WifiGetSurveyData_t;


//alice_modify_2012_3_27_v1

/**
* @brief Davinci emac private control type.
*/
typedef struct {
	unsigned int cmd;
	void *data;
} emac_drv_priv_ioctl;
/**
* @brief Mii data.
*/
struct mii_data
{
	unsigned short phy_id;
	unsigned short reg_num;
	unsigned short val_in;
	unsigned short val_out;
};

/* Basic Mode Control Register */
#define MII_BMCR		0x00
#define  MII_BMCR_RESET	    0x8000
#define  MII_BMCR_LOOPBACK  0x4000
#define  MII_BMCR_100MBIT	0x2000
#define  MII_BMCR_AN_ENA	0x1000
#define  MII_BMCR_ISOLATE	0x0400
#define  MII_BMCR_RESTART	0x0200
#define  MII_BMCR_DUPLEX	0x0100
#define  MII_BMCR_COLTEST	0x0080

/* Basic Mode Status Register */
#define MII_BMSR		0x01
#define  MII_BMSR_CAP_MASK	0xf800
#define  MII_BMSR_100BASET4	0x8000
#define  MII_BMSR_100BASETX_FD	0x4000
#define  MII_BMSR_100BASETX_HD	0x2000
#define  MII_BMSR_10BASET_FD	0x1000
#define  MII_BMSR_10BASET_HD	0x0800
#define  MII_BMSR_NO_PREAMBLE	0x0040
#define  MII_BMSR_AN_COMPLETE	0x0020
#define  MII_BMSR_REMOTE_FAULT	0x0010
#define  MII_BMSR_AN_ABLE	0x0008
#define  MII_BMSR_LINK_VALID	0x0004
#define  MII_BMSR_JABBER	0x0002
#define  MII_BMSR_EXT_CAP	0x0001

#define MII_PHY_ID1		0x02
#define MII_PHY_ID2		0x03

/* Auto-Negotiation Advertisement Register */
#define MII_ANAR		0x04

/* Auto-Negotiation Link Partner Ability Register */
#define MII_ANLPAR		0x05
#define  MII_AN_NEXT_PAGE	0x8000
#define  MII_AN_ACK		0x4000
#define  MII_AN_REMOTE_FAULT	0x2000
#define  MII_AN_ABILITY_MASK	0x07e0
#define  MII_AN_FLOW_CONTROL	0x0400
#define  MII_AN_100BASET4	0x0200
#define  MII_AN_100BASETX_FD	0x0100
#define  MII_AN_100BASETX_HD	0x0080
#define  MII_AN_10BASET_FD	0x0040
#define  MII_AN_10BASET_HD	0x0020
#define  MII_AN_PROT_MASK	0x001f
#define  MII_AN_PROT_802_3	0x0001

/* Auto-Negotiation Expansion Register */
#define MII_ANER		0x06
#define  MII_ANER_MULT_FAULT	0x0010
#define  MII_ANER_LP_NP_ABLE	0x0008
#define  MII_ANER_NP_ABLE	0x0004
#define  MII_ANER_PAGE_RX	0x0002
#define  MII_ANER_LP_AN_ABLE	0x0001

typedef struct
{
	int			RSSI1;
	int			RSSI2;	
	int			RSSI3;	
}WifiRSSI_t;


//int net_set_flag(char *ifname, short flag);
int net_nic_up(char *ifname);
//int net_clr_flag(char *ifname, short flag);
int net_get_flag(char *ifname);
int net_nic_down(char *ifname);
in_addr_t net_get_ifaddr(char *ifname);
int net_set_ifaddr(char *ifname, in_addr_t addr);
in_addr_t net_get_netmask(char *ifname);
int net_set_netmask(char *ifname, in_addr_t addr);
int net_get_hwaddr(char *ifname, unsigned char *mac);
in_addr_t net_get_gateway(void);
int net_set_gateway(in_addr_t addr);
int net_clean_gateway(void);
int net_get_rssi (WifiRSSI_t *WifiRSSI);//alice_modify_2012_5_10_v1 
//int net_add_gateway(in_addr_t addr);
//int net_del_gateway(in_addr_t addr);
in_addr_t net_get_dns(void);
in_addr_t net_get_dns2(void);    //by Larry
int net_set_dns(char *dnsname);
int net_set_dns2(char *dnsname); //by Larry
pid_t net_start_dhcpcd(char *ifname);
int net_renew_dhcpcd(pid_t pid);
int net_enable_dhcpcd(char* ifname, char *tablename, int ipmode);
int net_disable_dhcpcd(char *ifname);
int net_get_info(char *ifname, struct NET_CONFIG *netcfg);
struct in_addr net_get_ip(int skfd, char *ifname);
int Check_Link_status( char *pInterface );
int Check_Link_speed( char *pInterface );
void net_enable_ip_finder_srv(void);
void net_disable_ip_finder_srv(void);
void net_enable_bonjour(void);
void net_disable_bonjour(void);

int net_get_ifaddr_v6(const char *ifname, struct in6_addr *ifaddr);
int net_get_netmask_v6(const char *ifname, unsigned int* plen);
int net_disable_dhcpcd2(char *ifname);
int parse_wifi_infce_by_lshw(char *_wifiInfce, int _wifiInfceLen);

#if WIFI_FEATURE
#define SPACE 					32	   //	 		Space
#define EXCLAMATION_MARK	33	   //	!		Exclamation mark
#define DOUBLE_QUOTES		34	   //	"		&quot;	Double quotes (or speech marks)
#define NUMBER					35	   //	#		Number
#define DOLLAR					36	   //	$		Dollar
#define PROCENTTECHEN		37	   //	%		Procenttecken
#define AMPERSAND				38	   //	&		&amp;	Ampersand
#define SINGLE_QUOTE			39	   //	'		Single quote
#define OPEN_BRACKET			40	   //	(		Open parenthesis (or open bracket)
#define CLOSE_BRACKET		41	   //	)		Close parenthesis (or close bracket)
#define ASTERISK				42	   //	*		Asterisk
#define PLUS					43	   //	+		Plus
#define COMMA					44	   //	,		Comma
#define HYPHEN					45	   //	-		Hyphen
#define DOT						46	   //	.		Period, dot or full stop
#define SLASH					47	   //	/		Slash or divide
#define COLON					58	   //	:		Colon
#define SEMICOLON				59	   //	;		Semicolon
#define LESS_THEN				60	   //	<		&lt;	Less than (or open angled bracket)
#define EQUALS					61	   //	=	 	Equals
#define GREATER_THAN			62	   //	>		&gt;	Greater than (or close angled bracket)
#define QUESTION_MARK		63	   //	?	 	Question mark
#define AT_SYMBOL				64	   //	@	 	At symbol
#define OPENING_BRACKET		91	   //	[	 	Opening bracket
#define BACKSLASH				92	   //	\	 	Backslash
#define CLOSNG_BRACKET		93	   //	]	 	Closing bracket
#define CARET					94	   //	^	 	Caret - circumflex
#define UNDERSCORE			95	   //	_	 	Underscore
#define GRAVE_ACCENT			96	   //	`	 	Grave accent
#define OPENING_BRACE		123	//	{	 	Opening brace
#define VERTICAL_BAR			124	//	|	 	Vertical bar
#define CLOSING_BRACE		125	//	}	 	Closing brace
#define EQUIVALENCY_SIGN	126	//	~	 	Equivalency sign - tilde

#define DELIMETER_A			'('
#define DELIMETER_B			')'
#define SP_CH					"ABC123DEF!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~opq567zxc"
#define DECODED_SSID_LEN   32
#define ENCODED_SSID_LEN   (DECODED_SSID_LEN * 4)
#define SHIFT_ONE_QUOTE    1
#define MINUS_TWO_QUOTES   2
#define SIZEBUF                     1024
#define SIZE                        SIZEBUF
#define SSID_LEN_SIZE               256
#define __TRUE                      1
#define __FALSE                     0
//#define SUCCESS                     1
#define CP_CMD_FAIL                 -1
#define PARSE_CMD_FAIL              -2
#define FIND_CONNECT_CMD_SETS_FAIL  -3
#define WPS_PBC_CONNECT_FAIL        -4
#define DECODE_SSID_FAIL            -5
#define ENCODE_SSID_FAIL            -6
#define WPS_WAIT_SECS               120
#define WIFI_INFCE_LEN              16
#define _UNKNOWN_WIFI_DRV           0
#define _REALTEK_WIFI_DRV           1
#define _RALINK_WIFI_DRV            2
//#endif

//#if WIFI_FEATURE
#define IWLIST_SAMPLE_FROM_RALINK5572  "./iwlist_sample_from_ralink5572.txt"
#define IWLIST_SAMPLE_FROM_RTL8192     "./iwlist_sample_from_rtl8192.txt"
#define IWLIST_SCAN_CMD(IF)      	   "iwlist " IF " scan"
#define WPA_CLI_MAIN_CMD			      "wpa_cli -p/var/run/wpa_supplicant"
#define REMOVE_NETWORK				      "wpa_cli -p/var/run/wpa_supplicant remove_network"
#define REMOVE_NETWORK0				      "wpa_cli -p/var/run/wpa_supplicant remove_network 0"
#define AP_SCAN1					         "wpa_cli -p/var/run/wpa_supplicant ap_scan 1"
#define AP_SCAN2					         "wpa_cli -p/var/run/wpa_supplicant ap_scan 2"
#define ADD_NETWORK0				         "wpa_cli -p/var/run/wpa_supplicant add_network"
#define SET_NETWORK0_SSID			      "wpa_cli -p/var/run/wpa_supplicant set_network 0 ssid"
#define SET_NETWORK0_MODE1			      "wpa_cli -p/var/run/wpa_supplicant set_network 0 mode 1"
#define SET_NETWORK0_KEY_MGMT		      "wpa_cli -p/var/run/wpa_supplicant set_network 0 key_mgmt NONE"
#define SET_NETWORK0_WEP_KEY0		      "wpa_cli -p/var/run/wpa_supplicant set_network 0 wep_key0"
#define SET_NETWORK0_TX_KEYIDX0	      "wpa_cli -p/var/run/wpa_supplicant set_network 0 wep_tx_keyidx 0"
#define SET_NETWORK0_AUTH_ALG		      "wpa_cli -p/var/run/wpa_supplicant set_network 0 auth_alg SHARED"
#define SET_NETWORK0_PSK			      "wpa_cli -p/var/run/wpa_supplicant set_network 0 psk"
#define SELECT_NETWORK0				      "wpa_cli -p/var/run/wpa_supplicant select_network 0"
#define SAVE_CONFIG					      "wpa_cli -p/var/run/wpa_supplicant save_config"
#define DISCONNECT					      "wpa_cli -p/var/run/wpa_supplicant disconnect"
#define RECONNECT					         "wpa_cli -p/var/run/wpa_supplicant reconnect"
#define LIST_NETWORK				         "wpa_cli -p/var/run/wpa_supplicant list"
#define NETWORK_STATUS				      "wpa_cli -p/var/run/wpa_supplicant status"
#define SCAN						         "wpa_cli -p/var/run/wpa_supplicant scan"
#define SCAN_RESULT					      "wpa_cli -p/var/run/wpa_supplicant scan_result"
#define WPS_PBC_CMD					      "wpa_cli -p/var/run/wpa_supplicant wps_pbc any"
#define WPS_PIN_CMD					      "wpa_cli -p/var/run/wpa_supplicant wps_pin any"

#define  _INIT_SECURITY  0x0000
#define  _NONE_SECURITY  0x0001
#define  _CCMP           0x0002
#define  _TKIP           0x0004
#define  _CCMP_TKIP      0x0008
#define  _WEP            0x0010
#define  _WPA1           0x0020
#define  _WPA2           0x0040
#define  _WPA1_WPA2      0x0080

#define  __NONE_SECURITY             _NONE_SECURITY
#define  __WEP                       _WEP
#define  __WPA1_CCMP                 (_WPA1 | _CCMP)
#define  __WPA1_TKIP                 (_WPA1 | _TKIP)
#define  __WPA1_CCMP_TKIP            (_WPA1 | _CCMP_TKIP)
#define  __WPA2_CCMP                 (_WPA2 | _CCMP)
#define  __WPA2_TKIP                 (_WPA2 | _TKIP)
#define  __WPA2_CCMP_TKIP            (_WPA2 | _CCMP_TKIP)
#define  __WPA1_WPA2_CCMP        (_WPA1_WPA2 | _CCMP)
#define  __WPA1_WPA2_TKIP        (_WPA1_WPA2 | _TKIP)
#define  __WPA1_WPA2_CCMP_TKIP   (_WPA1_WPA2 | _CCMP_TKIP)

#define	WEP_OPEN		         1
#define  WEP_SHARED		      2
#define  WEP_ENC_USE_ASCII		3
#define  WEP_ENC_USE_HEX		4


enum {
   ENUM_REMOVE_NETWORK = 0,
   ENUM_AP_SCAN1,
   ENUM_AP_SCAN2,
   ENUM_ADD_NETWORK0,
   ENUM_SET_NETWORK0_SSID,
   ENUM_SET_NETWORK0_MODE1,
   ENUM_SET_NETWORK0_KEY_MGMT,
   ENUM_SET_NETWORK0_WEP_KEY0,
   ENUM_SET_NETWORK0_TX_KEYIDX0,
   ENUM_SET_NETWORK0_ALG,
   ENUM_SET_NETWORK0_PSK,
   ENUM_SELECT_NETWORK0,
   ENUM_SAVE_CONFIG,
   ENUM_DISCONNECT,
   ENUM_RECONNECT,
   ENUM_LIST_NETWORK,
   ENUM_NETWORK_STATUS, 
   ENUM_SCAN,
   ENUM_SCAN_RESULT
};

enum {
   ENUM_NONE = 0,
   ENUM_WEP,
   ENUM_WPA1_CCMP,
   ENUM_WPA1_TKIP,
   ENUM_WPA1_CCMP_TKIP,
   ENUM_WPA2_CCMP,
   ENUM_WPA2_TKIP,
   ENUM_WPA2_CCMP_TKIP,
   ENUM_WPA1_WPA2_CCMP,
   ENUM_WPA1_WPA2_TKIP,
   ENUM_WPA1_WPA2_CCMP_TKIP,
   ENUM_END
};

enum {
	INF_OPEN = 0,
	INF_WEP_WITH_OPEN,
	INF_WEP_WITH_SHARED,
	INF_TKIP_AES,
	ADH_OPEN,
	ADH_WEP_WITH_OPEN,
};

enum _wpa_state{
   ENUM_INTERFACE_DISABLED = -1,
   ENUM_WPA_UNKNOWN = 0,
   ENUM_WPA_INACTIVE,
   ENUM_WPA_SCANNING,
   ENUM_WPA_ASSOCIATED,
   ENUM_WPA_ASSOCIATING,
   ENUM_WPA_4WAY_HANDSHAKE,
   ENUM_WPA_COMPLETED
};

struct _EncryptType{
   int encryptField;
   char securityName[32];
   char encryptName[24];
};

typedef struct _wifi_ap_info{
   char cell[3];
   char protocol[20];
   char ssid[32];
   char mode[12];
   char frequency[12];
   char channel[24];
   char quality[64];
   char signal_level[24];
   char noise_level[24];
   char isEncryption;
   char bit_rate[12];
   char securityType[32];
   char encryptCipher[32];
   // char authentication_suites[12];
   int  securityFlag;
   int  encryptCipherFlag;
   char mac_addr[18];
}WIFI_AP_INFO;

typedef struct {
   int type;
   int cmdCnt;
   char *cmdSets[64];
}WPA_CLI_CMD_SETS;

typedef struct _wifi_connect_cmd{
   int cmdCnt;
   char **connectCmd;
}WIFI_CONNECT_CMD;

typedef struct _wifi_ap_node{
   WIFI_AP_INFO wifi_ap_info;
   struct _wifi_ap_node *next;
}WIFI_AP_NODE;


int findConnectCmdSets(int type, WIFI_CONNECT_CMD *_wifi_connect_cmd);
int cp2cmd(char *_cmd_dest, char *_cmd_src, ...);
int execWpaCliCmd(char *_cmd);
int execInfConnect(int _securityType, int _wepEncryptType, char *_ssid, char *_cipher);
int execAdHocConnect(int _securityType, int _wepEncryptType, char *_ssid, char *_cipher);
int cleanAllNetworkList(void);
void initNode(WIFI_AP_NODE* theNode);
void showWifiAPInfo(WIFI_AP_INFO *theWifiAPInfo, int cnt);
void showDetailWifiAPInfo(WIFI_AP_INFO *theWifiAPInfo);
void insertNode(WIFI_AP_NODE* _HeadNode, WIFI_AP_INFO* _NewWifiAPInfo);
void traverseNode(WIFI_AP_NODE *_node, WifiGetSurveyData_t *WifiGetSurveyData);
WIFI_AP_INFO* findNode(WIFI_AP_NODE *_node, int _num);
WIFI_AP_INFO* findNodebySSID(WIFI_AP_NODE *_node, char* _ssid_name);
int get_wifi_connect_status();
int getRalinkPinCode();
int wps_pin_connect(unsigned int _pincode);
int wps_pbc_connect();
int wifi_ap_site_survey(WIFI_AP_NODE *_headNode, char *_interface, WifiGetSurveyData_t *WifiGetSurveyData);
int wpa_supplicantActivate(char *_interface);
int getNetInterfaceName(char *_NetInfName);
////--------------------------------
static void HWPBC_SignalHandler(int sig);
//[> Wrapper to push some Wireless Parameter in the driver <]
static inline int iw_set_ext_rtl(int	skfd,		/* Socket to the kernel */
	   const char *ifname,		            /* Device name */
	   int request_id,	                  /* WE ID */
	   struct iwreq *	pwrq);		         /* Fixed part of the request */
void *detect_HW_PBC(void *_net_interface);
pid_t g_thread_func_id;
////--------------------------------
int encodedSSID(char* _org_ssid, char* _target_ssid, int _ssid_len);
int decodedSSID(char* _encoded_ssid, char* _target_ssid, int _ssid_len);
char g_encoded_ssid[ENCODED_SSID_LEN];
char g_decoded_ssid[DECODED_SSID_LEN];
void removeDoubleQuotes(char *_target_ssid, int _target_ssid_len);
int ValidateChecksum(unsigned long int PIN);
int ComputeChecksum(unsigned long int PIN);
int genUniqPincodeFromMacAddr();
//#endif
#endif

#if defined (__cplusplus)
}
#endif
