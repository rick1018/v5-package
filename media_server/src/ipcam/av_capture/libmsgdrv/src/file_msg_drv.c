/* ===========================================================================
* @file file_msg_drv.c
*
* @path $(IPNCPATH)\util
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <share_mem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/vfs.h>		
#include <sys/time.h>		
#include <dm355_gio_util.h>	
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <signal.h>			
#include <sys/mount.h>		
#include <sys/wait.h>		
#include <fcntl.h>           
#include <linux/sockios.h>
#include <sem_util.h>
#include <sys/stat.h>        
#include <system_default.h>
#include <Msg_Def.h>
#include <file_msg_drv.h>
#include <net_config.h>

#include <sem_util.h> 
//#include <system_call_msg_drv.h>				

#ifdef DEBUG
#define DBG(fmt, args...)	printf("FileMsgDrv: Debug\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("FileMsgDrv: Error\n" fmt, ##args)

/* Get IFNAMSIZ */
#include <net/if.h>
#ifndef IFNAMSIZ
#define IFNAMSIZ	16
#endif

/* Maximum length of network interface name */
#define MAX_IFACE_LEN	IFNAMSIZ

#define SWAP(a, b)  {a ^= b; b ^= a; a ^= b;}
#define ONVIF_SCOPE "www.onvif.org"

static int extro_acount_index = 0;
static int md_index = 0;
static int event_sch_index = 0;
static int qid, mid, gProcId, eCount = 0, gKey;
static void *pShareMem;
static SemHandl_t hndlDrvSem = NULL;

DBListInfo DBList[DB_MEDIA_TOTAL];
char DBDir[DB_MEDIA_TOTAL][DB_MEDIA_DIR_LEN] = DB_MEDIA_DIR_LIST;
int DBOpShift[DB_INFO_LIST_OP_TOTAL] = DB_INFO_LIST_OP_SHIFT_LIST;
char DBCount[DB_COUNT_CHECK_TOTAL] = {0};		
char DBSearchCount[DB_MEDIA_TOTAL] = {0};
int FileCheckCount = 0;						
#define KERNEL_2_0_0

#define MSG_NOWAIT 1 

static pthread_mutex_t file_mag_mutex;

typedef union __NET_IPV4 {
	__u32	int32;
	__u8	str[4];
} NET_IPV4;

int FileMagMutexInit(void)
{
	return pthread_mutex_init(&file_mag_mutex, NULL);
}

int FileMagMutexDestroy(void)
{
	return pthread_mutex_destroy(&file_mag_mutex);
}
/* 20140225 */

/**
* @brief Save message ID in file message driver

* Save a message ID to file message driver. This ID will be used to identify
*  who is communicating with file manager. This function is old method, please
*  use InitFileMsgDrv(int key,int iProcId) instead.
* @param iqid [I ] message ID
*/
void SaveQid(int iqid)
{
	qid = iqid;
}
/**
* @brief Send quit command to file manager.

* This command will make file manager stop running. After you called this, all
*  the other process can't get system information because file manager is down.
*/
void SendFileQuitCmd()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.cmd = FILE_MSG_QUIT;
#if MSG_NOWAIT
  	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
#endif
}

#if 0
static int GetShareMemoryId()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_GET_MID;
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	return msgbuf.shmid;
}
#endif

#if 0
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
#endif

int net_get_hwaddr_util(char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
	{
		close(skfd);
		return -1;
	}
	close(skfd);
	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);

	return 0;
}

/**
* @brief Initialize file message driver.

* Initialize file message driver. Please see \ref FILE_MSG_DRV_HOW to learn more.
* @note This API must be used before use any other file message driver API.
* @param key [I ] Key number for message queue and share memory.
* @param iProcId [I ] Message ID(Which was define in \ref File_Msg_Def.h) to initialize file message driver.
* @retval 0 Success.
* @retval -1 Fail.
*/
int InitFileMsgDrv(int key,int iProcId)
{
	if(hndlDrvSem == NULL)
		hndlDrvSem = MakeSem();
	if(hndlDrvSem == NULL){
		return -1;
	}
	DBG("Semaphore Addr: %p\n", hndlDrvSem);
	if((qid = Msg_Init(key)) < 0){
		ERR("Message init fail\n");
		DestroySem(hndlDrvSem);
		return -1;
	}
	DBG("Qid: %d\n",qid);
	gKey = key;
	mid = pShareMemInit(key);
	if(mid < 0){
		ERR("Share memory id error\n");
		DestroySem(hndlDrvSem);
		return -1;
	}
	DBG("Share Memory id : %d\n", mid);
	pShareMem = shmat(mid,0,0);
	DBG("Share Memory Address:%p\n", pShareMem);
	gProcId = iProcId;
	return 0;
}
/**
* @brief Cleanup file message driver.

* This API shoud be called at the end of process.
*/
void CleanupFileMsgDrv()
{
	shmdt(pShareMem);
	DestroySem(hndlDrvSem);
}

Motion_Str_Data *GetMDData(char *md_data)
{
	int enable = 0, left = 0, top = 0, right = 0, bottom = 0, sen = 0;
	char *ptr;

	ptr = md_data;

	memset(&MD_Str_Data, 0, sizeof(Motion_Str_Data));

	sscanf(ptr, "%d", &enable);
	if (NULL != (ptr = strstr(ptr, ",")))
	 	ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &left);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

	sscanf(ptr, "%d", &top);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &right);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

	sscanf(ptr, "%d", &bottom);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &sen);

    MD_Str_Data.enable = enable;
    MD_Str_Data.left   = left;
    MD_Str_Data.top    = top;
    MD_Str_Data.right  = right;
    MD_Str_Data.bottom = bottom;
    MD_Str_Data.sen    = sen;
    return (Motion_Str_Data *)&MD_Str_Data;
}

/**
* @brief Get system information.

* Get system information from file manager. In order to make every change can
*  update quickly, this function return the pointer to share memory.
* @warning Do NOT modify where this pointer point to.
* @return Pointer to where system information was stored.
* @retval NULL Can't get system information.
*/
SysInfo *GetSysInfo()
{
#if 0	
	if (eCount >= 3)
	{
		eCount = 0;
		qid = Msg_Init(gKey);
	}

	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if (msgbuf.length != sizeof(SysInfo))
	{
		//ERR("Data read error\n");
		eCount++;
		return NULL;
	}

	return (SysInfo *) pShareMem;
#else
	return (SysInfo *) pShareMem;
#endif
}

/**
* @brief Get system log information.

* @param nPageNum [I ] Page number
* @param nItemNum [I ] Item number
* @param pBuf [O ] pointer to where log information stored.
* @retval 0 Success.
* @retval -1 Fail
*/
int GetSysLog(int nPageNum, int nItemNum, LogEntry_t * pBuf)
{
	FILE_MSG_BUF msgbuf;
	SysInfo *pSysInfo = (SysInfo *) pShareMem;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_READ;
	msgbuf.shmid = mid;
	msgbuf.shmidx = 1;
	msgbuf.nPageNum = nPageNum;
	msgbuf.nItemNum = nItemNum;
	if (pthread_mutex_trylock(&file_mag_mutex) < 0)
	{//SemWait(hndlDrvSem);
		fprintf(stderr, "pthread_mutex_trylock GetSysLog file_mag_mutex fail...\n");
		pthread_mutex_unlock(&file_mag_mutex);
		return -1;
  	}
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	pthread_mutex_unlock(&file_mag_mutex);//SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	memcpy(pBuf, &pSysInfo->tCurLog, sizeof(LogEntry_t));
	return 0;
}
/**
* @brief Save system information in file manager.
* @param nShmIndex [I ] Share memory index (0 or 1). 0 is used for system settings;
 and 1 is used for system log.
* @retval 0 Success.
* @retval -1 Fail
*/
int SetSysInfo(int nShmIndex)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE;
	msgbuf.shmid = mid;
	msgbuf.shmidx = nShmIndex;
	if (pthread_mutex_trylock(&file_mag_mutex) < 0)
	{//SemWait(hndlDrvSem);
		fprintf(stderr, "pthread_mutex_trylock SetSysInfo file_mag_mutex fail...\n");
		pthread_mutex_unlock(&file_mag_mutex);
		return -1;
  	}
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	pthread_mutex_unlock(&file_mag_mutex);//SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	if (nShmIndex != 0)
		fprintf(stderr, "SetSysInfo(%d)!!\n", nShmIndex);
	return 0;
}
/**
* @brief Save system information in file manager immediately
* @retval 0 Success.
* @retval -1 Fail
*/
static int SetSysInfoImmediately()
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_WRITE_I;
	msgbuf.shmid = mid;
	if (pthread_mutex_trylock(&file_mag_mutex) < 0)
	{//SemWait(hndlDrvSem);
		fprintf(stderr, "pthread_mutex_trylock SetSysInfoImmediately file_mag_mutex fail...\n");
		pthread_mutex_unlock(&file_mag_mutex);
		return -1;
  	}
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	pthread_mutex_unlock(&file_mag_mutex);//SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data write error\n");
		eCount++;
		return -1;
	}
	return 0;
}
/**
* @brief Save netmask in file manager

* The netmask here will be used in static IP mode.
* @param netmask [I ] New netmask in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetNetMask(struct in_addr netmask)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.netmask, &netmask, sizeof(netmask));
	return SetSysInfo(0);
}
/**
* @brief Save IP in file manager

* The IP here will be used in static IP mode.
* @param ip [I ] New IP in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetIp(struct in_addr ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.ip, &ip, sizeof(ip));
	return SetSysInfoImmediately();
}
/**
* @brief Set http port in file manager

* Http port of web site will be change when boa restart.
* @param port [I ] New http port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetHttpPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.http_port, &port, sizeof(port));
	return SetSysInfo(0);
}
/**
* @brief Save IPCAM title in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTitle(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if (strncmp(buf, "Empty", 5))
	{
		if(sizeof(pSysInfo->lan_config.title) < len+1)
			return -1;
		memset(pSysInfo->lan_config.title, '\0', sizeof(pSysInfo->lan_config.title));
		memcpy(pSysInfo->lan_config.title, buf, len);
	}
	else
	{
  		memset(pSysInfo->lan_config.title, '\0', MAX_LANCAM_TITLE_LEN+1);
		pSysInfo->lan_config.title[MAX_LANCAM_TITLE_LEN] = '\0';
	}
	return SetSysInfo(0);
}
/**
* @brief Save IPCAM title in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetRegUsr(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->lan_config.regusr) < len+1)
		return -1;
	memcpy(pSysInfo->lan_config.regusr, buf, len);
	pSysInfo->lan_config.regusr[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save gateway in file manager
* @param gateway [I ] New gateway in static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGateway(struct in_addr gateway)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.gateway, &gateway, sizeof(gateway));
	return SetSysInfo(0);
}
/**
* @brief Save DHCP status in file manager
* @param enable [I ] 0 meets DHCP mode, otherwise static IP mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDhcpEnable(int enable)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.dhcp_enable, &enable, sizeof(enable));
	return SetSysInfo(0);
}
/**
* @brief Save DHCP setup status in file manager
* @param setup 1 meets DHCP status fail , otherwise setup DHCP status success.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDhcpStatus(int status)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.dhcp_config, &status, sizeof(status));
	return SetSysInfo(0);
}
/**
* @brief Save FTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpFqdn(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->ftp_config.servier_ip) < len + 1)
		return -1;
	memcpy(&pSysInfo->ftp_config.servier_ip, buf, len);
	pSysInfo->ftp_config.servier_ip[len] = '\0';
	return SetSysInfo(0);
}
/**
* @brief Save FTP user name in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpUsername(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[33] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->ftp_config.username) < len + 1)
		return -1;
	memcpy(pSysInfo->ftp_config.username, buf, len);
	pSysInfo->ftp_config.username[len] = '\0';
  	}
	else
	{
		memset(pSysInfo->ftp_config.username, 0, USER_LEN);
		pSysInfo->ftp_config.username[USER_LEN] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save FTP user password in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPassword(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[33] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->ftp_config.password) < len + 1)
		return -1;
	memcpy(pSysInfo->ftp_config.password, buf, len);
	pSysInfo->ftp_config.password[len] = '\0';
	}
	else
	{
		memset(pSysInfo->ftp_config.password, 0, PASSWORD_LEN);
		pSysInfo->ftp_config.password[PASSWORD_LEN] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save FTP folder in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpFoldname(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[256] = "";

	if(len !=0 )
	memcpy(tmp, buf, len );
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->ftp_config.foldername) < len + 1)
		return -1;
	memcpy(pSysInfo->ftp_config.foldername, buf, len);
	pSysInfo->ftp_config.foldername[len] = '\0';
	}
	else
	{
		memset(pSysInfo->ftp_config.foldername, 0, MAX_STRING_LENGTH);
		pSysInfo->ftp_config.foldername[MAX_STRING_LENGTH] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save FTP image acount in file manager

* This value controls that how many JPEG image will be sent in one FTP
*  connection.
* @param imageacount [I ] JPEG image count in one FTP connection.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpImageacount(int imageacount)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ftp_config.image_acount, &imageacount, sizeof(imageacount));
	return SetSysInfo(0);
}
/**
* @brief Save FTP port number in file manager
* @param port [I ] New FTP port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ftp_config.port, &port, sizeof(port));
	return SetSysInfo(0);
}
/**
* @brief Save FTP client process ID in file manager
* @param pid [I ] FTP client process ID.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFtpPid(int pid)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ftp_config.pid, &pid, sizeof(pid));
	return SetSysInfo(0);
}
/**
* @brief Save SMTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpServerIp(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->smtp_config.servier_ip) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.servier_ip, buf, len);
	pSysInfo->smtp_config.servier_ip[len] = '\0';
	return SetSysInfo(0);
}

/**
* @brief Save SMTP port number in file manager
* @param port [I ] New SMTP port.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpServerPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.server_port, &port, sizeof(port));
	return SetSysInfo(0);
}

/**
* @brief Save SMTP Authentication in file manager.

* Determind whether use SMTP authentication to send e-mail or not.
* @param authentication [I ] SMTP authentication.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpAuthentication(unsigned char authentication)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.authentication, &authentication,
		sizeof(authentication));
	return SetSysInfo(0);
}
/**
* @brief Save SMTP user name in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpUsername(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[33] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->smtp_config.username) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.username, buf, len);
	pSysInfo->smtp_config.username[len] = '\0';
	}
	else
	{
		memset(pSysInfo->smtp_config.username, 0, USER_LEN);
		pSysInfo->smtp_config.username[USER_LEN-1] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save SMTP user password in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpPassword(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[33] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->smtp_config.password) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.password, buf, len);
	pSysInfo->smtp_config.password[len] = '\0';
  	}
  	else
	{
		memset(pSysInfo->smtp_config.password, 0, PASSWORD_LEN);
		pSysInfo->smtp_config.password[PASSWORD_LEN-1] = '\0';
  	}

	return SetSysInfo(0);
}
/**
* @brief Save SMTP sender e-mail address in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpSenderEmail(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[256] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
	if(sizeof(pSysInfo->smtp_config.sender_email) < len + 1)
		return -1;
	memcpy(pSysInfo->smtp_config.sender_email, buf, len);
	pSysInfo->smtp_config.sender_email[len] = '\0';
		fprintf(stderr, "2 pSysInfo->smtp_config.sender_email = %s\n", pSysInfo->smtp_config.sender_email);
  	}
  	else
	{
		memset(pSysInfo->smtp_config.sender_email, 0, MAX_STRING_LENGTH);
		pSysInfo->smtp_config.sender_email[MAX_STRING_LENGTH] = '\0';
		fprintf(stderr, "2 pSysInfo->smtp_config.sender_email = %s\n", pSysInfo->smtp_config.sender_email);
  	}

	return SetSysInfo(0);
}
/**
* @brief Save SMTP reveiver e-mail address in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpReceiverEmail(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[256] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
		if (sizeof(pSysInfo->smtp_config.receiver_email) < len + 1)
			return -1;
		memcpy(pSysInfo->smtp_config.receiver_email, buf, len);
		pSysInfo->smtp_config.receiver_email[len] = '\0';
		fprintf(stderr, "1 pSysInfo->smtp_config.receiver_email = %s  %d\n", pSysInfo->smtp_config.receiver_email, len);
	}
	else
	{
		memset(pSysInfo->smtp_config.receiver_email, 0, MAX_STRING_LENGTH);
		pSysInfo->smtp_config.receiver_email[MAX_STRING_LENGTH] = '\0';
		fprintf(stderr, "2 pSysInfo->smtp_config.receiver_email = %s\n", pSysInfo->smtp_config.receiver_email);
  	}

	return SetSysInfo(0);
}
/**
* @brief Save e-mail CC in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpCC(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[256] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
		if (sizeof(pSysInfo->smtp_config.CC) < len + 1)
			return -1;
		memcpy(pSysInfo->smtp_config.CC, buf, len);
		pSysInfo->smtp_config.CC[len] = '\0';
	}
	else
	{
		memset(pSysInfo->smtp_config.CC, 0, MAX_STRING_LENGTH);
		pSysInfo->smtp_config.CC[MAX_STRING_LENGTH-1] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save e-mail subject in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpSubject(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
  	char tmp[256] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
		if (sizeof(pSysInfo->smtp_config.subject) < len + 1)
			return -1;
		memcpy(pSysInfo->smtp_config.subject, buf, len);
		pSysInfo->smtp_config.subject[len] = '\0';
	}
	else
	{
		memset(pSysInfo->smtp_config.subject, 0, MAX_STRING_LENGTH);
		pSysInfo->smtp_config.subject[MAX_STRING_LENGTH-1] = '\0';
	}

	return SetSysInfo(0);
}
/**
* @brief Save e-mail contant text in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpText(void * buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	char tmp[256] = "";

	memcpy(tmp, buf, sizeof(tmp));
	if(pSysInfo == NULL)
		return -1;
	if (strstr(tmp, "Empty") == NULL)
	{
		if (sizeof(pSysInfo->smtp_config.text) < len + 1)
			return -1;
		memcpy(pSysInfo->smtp_config.text, buf, len);
		pSysInfo->smtp_config.text[len] = '\0';
 	}
  	else
	{
		memset(pSysInfo->smtp_config.text, 0, MAX_STRING_LENGTH);
		pSysInfo->smtp_config.text[MAX_STRING_LENGTH-1] = '\0';
  	}

	return SetSysInfo(0);
}
/**
* @brief Save e-mail attachments in file manager.
* @param attachments [I ] Count of JPEG images in one e-mail.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpAttachments(unsigned char attachments)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.attachments, &attachments,
		sizeof(attachments));
	return SetSysInfo(0);
}
/**
* @brief Save e-mail view in file manager.
* @param view [I ] 0 meets text style, otherwise html style.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSmtpView(unsigned char view)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.view, &view, sizeof(view));
	return SetSysInfo(0);
}
/**
* @brief Save Primary DNS ip in file manager.

* DNS IP here will be used under static IP mode.
* @param ip [I ] DNS ip.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDns(struct in_addr ip)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.dns, &ip, sizeof(ip));
	return SetSysInfo(0);
}
/**
* @brief Save Secondary DNS ip in file manager.

* DNS IP here will be used under static IP mode.
* @param ip [I ] DNS ip.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDns2(struct in_addr ip)
{
   SysInfo *pSysInfo = (SysInfo *)pShareMem;
   if(pSysInfo == NULL)
      return -1;
//   memcpy(&pSysInfo->lan_config.net.dns2, &ip, sizeof(ip));
   return SetSysInfo(0);
}
/**
* @brief Save SNTP FQDN in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSntpServer(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->lan_config.net.ntp_server) < len + 1)
		return -1;
	memcpy(pSysInfo->lan_config.net.ntp_server, buf, len);
	pSysInfo->lan_config.net.ntp_server[len] = '\0';
	return SetSysInfo(0);
}
#if DAY_NIGHT_FEATURE
/**
* @brief Save camera day/night mode in file manager.
* @param value [I ] Day/night mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamDayNight(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nDayNight, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save camera white balance mode in file manager.
* @param value [I ] White balance mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamWhiteBalance(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nWhiteBalance, &value, sizeof(value));
	return SetSysInfo(0);
}
#if BACKLIGHT_FEATURE
/**
* @brief Save camera backlight mode in file manager.
* @param value [I ] Backlight mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamBacklight(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBackLight, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save camera brightness in file manager.
* @param value [I ] Brightness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamBrightness(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBrightness, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera contrast in file manager.
* @param value [I ] Contrast.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamContrast(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nContrast, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera saturation in file manager.
* @param value [I ] Saturation.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamSaturation(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nSaturation, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save camera sharpness in file manager.
* @param value [I ] Sharpness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamSharpness(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nSharpness, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetCamExposureMode(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.exposuremode, &value, sizeof(value));

//	fprintf(stderr, "fSetCamExposureMod value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetMaxExposureValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.maxexposure, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetMaxGainValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.maxgain, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetCamExposureValue(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.exposurevalue, &value, sizeof(value));

//	fprintf(stderr, "fSetCamExposureValue value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamExposureValue1(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.exposurevalue1, &value, sizeof(value));

//	fprintf(stderr, "fSetCamExposureValue1 value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}

/**
* @brief Save camera sharpness in file manager.
* @param value [I ] Sharpness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamAGC(unsigned char value)
{
#if AGC_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.agc, &value, sizeof(value));

//	fprintf(stderr, "fSetCamAGC value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamAGCMin(unsigned char value)
{
#if AGC_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.agc_min, &value, sizeof(value));

//	fprintf(stderr, "fSetCamAGC value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamAGC1(unsigned char value)
{
#if AGC_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.agc1, &value, sizeof(value));

//	fprintf(stderr, "fSetCamAGC value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamAGCMin1(unsigned char value)
{
#if AGC_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.agc_min1, &value, sizeof(value));

//	fprintf(stderr, "fSetCamAGC value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}

/**
* @brief Save camera sharpness in file manager.
* @param value [I ] Sharpness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamShutterTime(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.shuttertime, &value, sizeof(value));

	fprintf(stderr, "fSetCamShutterTime value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamShutterTimeMin(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.shuttertime_min, &value, sizeof(value));

	fprintf(stderr, "fSetCamShutterTimeMin value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamShutterTime1(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.shuttertime1, &value, sizeof(value));

	fprintf(stderr, "fSetCamShutterTime1 value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}
int fSetCamShutterTimeMin1(unsigned char value)
{
#if MANUAL_SHUTTER_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.shuttertime_min1, &value, sizeof(value));

	fprintf(stderr, "fSetCamShutterTimeMin1 value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}

/**
* @brief Save camera sharpness in file manager.
* @param value [I ] Sharpness.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetCamFlickerlessMode(unsigned char value)
{
#if FLICKERLESS_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.flickerlessmode, &value, sizeof(value));

//	fprintf(stderr, "fSetCamFlickerlessMode value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}

int fSetCamColorBar(unsigned char value)
{
#if COLORBAR_FEATURE
	SysInfo *pSysInfo = (SysInfo *)pShareMem;

	if (pSysInfo == NULL) return -1;
	memcpy(&pSysInfo->lan_config.colorbar, &value, sizeof(value));

	fprintf(stderr, "fSetCamColorBar value = %d!!\n", value);
	return SetSysInfo(0);
#else
	return 1;
#endif
}

/* TAMPER_FEATURE - Dean - 20131007 - Begin */
int fSetTamper(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	return SetSysInfo(0);
}
/* TAMPER_FEATURE - Dean - 20131007 - End */

/**
* @brief Save JPEG encode quality in file manager.
* @param value [I ] JPEG quality.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetJpegQuality(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.njpegquality, &value, sizeof(value));
	return SetSysInfo(0);
}

#if  ROTATION_FEATURE

int fSetRotation(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.rotation, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
int fSetMirror(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.mirror, &value, sizeof(value));
	return SetSysInfo(0);
}
#if FDETECT_FEATURE
int fSetFaceDetect(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fdetect, &value, sizeof(value));
	return SetSysInfo(0);
}
#else
int fSetOvDetect(unsigned char value) //RS,20091120
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.fdetect, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
int fSetFDX(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.startX, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDY(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.startY, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDW(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDH(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDConfLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fdconflevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFDDirection(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.fddirection, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetFRecognition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frecog, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFRConfLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frconflevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetFRDatabase(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->face_config.frdatabase, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetPrivacyMask(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	//memcpy(&pSysInfo->face_config.pmask, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMaskOptions(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	//memcpy(&pSysInfo->face_config.maskoption, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAdvMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.AdvanceMode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetM41SFeature(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.M41SFeature, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetM42SFeature(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.M42SFeature, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetJPGSFeature(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.JPGSFeature, &value, sizeof(value));
	return SetSysInfo(0);
}
#if DISCARD_FEATURE
int fSetDemoCfg(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.democfg, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
int fSetOSDWin(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.osdwin, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetHistogram(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.histogram, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDynRange(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.dynRange, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetOSDStream(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.osdstream, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetOSDWinNum(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.osdwinnum, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save IPCAM title in file manager
* @param buf [I ] Pointer to string.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetOSDText(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	if (sizeof(pSysInfo->lan_config.osdtext) < len+1)
		return -1;
	memcpy(pSysInfo->lan_config.osdtext, buf, len);
	pSysInfo->lan_config.osdtext[len] = '\0';

	return SetSysInfo(0);
}

#if 0
int fSetVideoCodec(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nVideocodec, &value, sizeof(value));

	fprintf(stderr, "fSetVideoCodec !!\n");
	return SetSysInfo(0);
}
#endif

/**
* @brief Save video codec mode in file manager.

* Video codec mode can be single codec, dual codec, triple codec.
* @param value [I ] Video codec mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetVideoMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	/* 20140225 */
#if 1
	memcpy(&pSysInfo->lan_config.nStreamType, &value, sizeof(value));
#else
	memcpy(&pSysInfo->lan_config.nVideomode, &value, sizeof(value));
#endif
	/* 20140225 */
	return SetSysInfo(0);
}
int fSetVideoCodecMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nVideocodecmode, &value, sizeof(value));

	return SetSysInfo(0);
}
int fSetVideoCodecCombo(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nVideocombo, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetVideoCodecRes(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nVideocodecres, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save time display format in file manager.
* @param value [I ] Time display format.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTimeFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.time_format, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save daylight time setting in file manager.
* @param value [I ] Use daylight or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDaylightTime(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.daylight_time, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save time zone in file manager.
* @param value [I ] Time zone.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTimeZone(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.ntp_timezone, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save image size in file manager.
* @param stream [I ] 0: JPEG stream. 1: MPEG1 stream. 2: MPEG2 stream.
* @param Xvalue [I ] Image width.
* @param Yvalue [I ] Image height.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetVideoSize(unsigned char stream, int Xvalue, int Yvalue)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	if (stream == 1 || stream == 4) {
		memcpy(&pSysInfo->lan_config.JpegXsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->lan_config.JpegYsize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 2) {
		memcpy(&pSysInfo->lan_config.Mpeg41Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->lan_config.Mpeg41Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 3) {
		memcpy(&pSysInfo->lan_config.Mpeg42Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->lan_config.Mpeg42Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 5) {
		memcpy(&pSysInfo->lan_config.Avc1Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->lan_config.Avc1Ysize, &Yvalue, sizeof(Yvalue));
	} else if (stream == 6) {
		memcpy(&pSysInfo->lan_config.Avc2Xsize, &Xvalue, sizeof(Xvalue));
		memcpy(&pSysInfo->lan_config.Avc2Ysize, &Yvalue, sizeof(Yvalue));
	}

	return SetSysInfo(0);
}

int fSetStreamConfig(unsigned char stream, int width, int height, char *name, int portnum)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	fResetFDROIParam(stream, width, height);
	fResetCodecROIParam(stream, width, height);

	pSysInfo->stream_config[stream].width	= width;
	pSysInfo->stream_config[stream].height  = height;

	strcpy((char *)pSysInfo->stream_config[stream].name, name);

	pSysInfo->stream_config[stream].portnum  = portnum;

	if((portnum==H264_1_PORTNUM) || (portnum==H264_2_PORTNUM))
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/2?videoCodecType=H.264");  //h264   PSIA/Streaming/channels/2?videoCodecType=H.264
	else if((portnum==MPEG4_1_PORTNUM) || (portnum==MPEG4_2_PORTNUM))
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/1?videoCodecType=MPEG4");  //mpeg4   PSIA/Streaming/channels/1?videoCodecType=MPEG4
	else
		strcpy((char *)pSysInfo->stream_config[stream].portname, "PSIA/Streaming/channels/0?videoCodecType=MJPEG");  //mjpeg   PSIA/Streaming/channels/0?videoCodecType=MJPEG

	return SetSysInfo(0);
}

void fResetCodecROIParam(int stream, int width, int height)
{
	int i=0;

    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	for(i=0;i<pSysInfo->codec_roiconfig[stream].numROI;i++) {
		pSysInfo->codec_roiconfig[stream].roi[i].startx = (width*pSysInfo->codec_roiconfig[stream].roi[i].startx)/pSysInfo->codec_roiconfig[stream].prevwidth;
		pSysInfo->codec_roiconfig[stream].roi[i].starty = (height*pSysInfo->codec_roiconfig[stream].roi[i].starty)/pSysInfo->codec_roiconfig[stream].prevheight;
		pSysInfo->codec_roiconfig[stream].roi[i].width 	= (width*pSysInfo->codec_roiconfig[stream].roi[i].width)/pSysInfo->codec_roiconfig[stream].prevwidth;
		pSysInfo->codec_roiconfig[stream].roi[i].height = (height*pSysInfo->codec_roiconfig[stream].roi[i].height)/pSysInfo->codec_roiconfig[stream].prevheight;
	}

	pSysInfo->codec_roiconfig[stream].prevwidth 	= (width<=0)?1:width;
	pSysInfo->codec_roiconfig[stream].prevheight 	= (height<=0)?1:height;

	return;
}

void fResetFDROIParam(int stream, int width, int height)
{
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	if(stream == 0) {
		pSysInfo->face_config.startX 		= (width*pSysInfo->face_config.startX)/pSysInfo->face_config.prevwidth;
		pSysInfo->face_config.startY 		= (height*pSysInfo->face_config.startY)/pSysInfo->face_config.prevheight;
		pSysInfo->face_config.width  		= (width*pSysInfo->face_config.width)/pSysInfo->face_config.prevwidth;
		pSysInfo->face_config.height 		= (height*pSysInfo->face_config.height)/pSysInfo->face_config.prevheight;
		pSysInfo->face_config.prevwidth  	= (width<=0)?1:width;
		pSysInfo->face_config.prevheight 	= (height<=0)?1:height;
	}

	return;
}

/**
* @brief Save stream active status in file manager.
* @param stream1 [I ] Active or not.
* @param stream2 [I ] Active or not.
* @param stream3 [I ] Active or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetStreamActive(unsigned char stream1, unsigned char stream2, unsigned char stream3,
                     unsigned char stream4, unsigned char stream5, unsigned char stream6)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.Supportstream1, &stream1, sizeof(stream1));
	memcpy(&pSysInfo->lan_config.Supportstream2, &stream2, sizeof(stream2));
	memcpy(&pSysInfo->lan_config.Supportstream3, &stream3, sizeof(stream3));
	memcpy(&pSysInfo->lan_config.Supportstream4, &stream4, sizeof(stream4));
	memcpy(&pSysInfo->lan_config.Supportstream5, &stream5, sizeof(stream5));
	memcpy(&pSysInfo->lan_config.Supportstream6, &stream6, sizeof(stream6));
	return SetSysInfo(0);
}
/**
* @brief Save TV out clock config in file manager
* @param value [I] NTSC or PAL mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageSource(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.imagesource, &value, sizeof(value));
	fprintf(stderr, "fSetImageSource %d\n", pSysInfo->lan_config.net.imagesource);
	return SetSysInfo(0);
}
/**
* @brief Save MPEG1 bitrate in file manager.
* @param value [I ] Bitrate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMP41bitrate(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nMpeg41bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save MPEG2 bitrate in file manager.
* @param value [I ] Bitrate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMP42bitrate(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nMpeg42bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetMP43bitrate(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nMpeg43bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save encode frame rate in file manager.
* @param stream [I ] Which stream.
* @param value [I ] Frame rate.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFramerate1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	pSysInfo->lan_config.nMpeg41frmrate = value;
	//memcpy(&pSysInfo->lan_config.nMpeg41frmrate, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerate2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	pSysInfo->lan_config.nMpeg42frmrate = value;
	//memcpy(&pSysInfo->lan_config.nMpeg42frmrate, &value, sizeof(value));
	//printf("pSysInfo->lan_config.nMpeg42frmrate = %d\n",pSysInfo->lan_config.nMpeg42frmrate);
	return SetSysInfo(0);
}

int fSetFramerate3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	pSysInfo->lan_config.nMpeg43frmrate = value;
	//memcpy(&pSysInfo->lan_config.nMpeg43frmrate, &value, sizeof(value));
	//printf("pSysInfo->lan_config.nMpeg43frmrate = %d\n",pSysInfo->lan_config.nJpegfrmrate);
	return SetSysInfo(0);
}
int fSetFramerate4(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	pSysInfo->lan_config.nJpegfrmrate = value;
	//memcpy(&pSysInfo->lan_config.nJpegfrmrate, &value, sizeof(value));
	printf("pSysInfo->lan_config.nJpegfrmrate = %d\n",pSysInfo->lan_config.nJpegfrmrate);
	return SetSysInfo(0);
}

int fSetFramerateVal1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal1, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerateVal2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal2, &value, sizeof(value));

	return SetSysInfo(0);
}

int fSetFramerateVal3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.frameRateVal3, &value, sizeof(value));

	return SetSysInfo(0);
}
int fSetEncFramerate(unsigned char stream, unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	if (stream == 0)
		memcpy(&pSysInfo->lan_config.nMpeg41frmrate, &value, sizeof(value));
	else if (stream == 1)
		memcpy(&pSysInfo->lan_config.nMpeg42frmrate, &value, sizeof(value));
	else
		memcpy(&pSysInfo->lan_config.nJpegfrmrate, &value, sizeof(value));

	fprintf(stderr, "fSetEncFramerate(%d,%d) !!\n",stream,value);
	return SetSysInfo(0);
}
//Add by Joseph 20131119
#if GOP_FEATURE
int fSetMP41Gop(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nMpeg41GOP, &value, sizeof(value));

	fprintf(stderr, "fSetMP41Gop %d !!\n",value);
	return SetSysInfo(0);
}
#endif
/**
* @brief Save rate control in file manager.
*
* @param value [I ] 0: OFF, 1: VBR, 2: CBR
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetRateControl(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nRateControl, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetImageFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.imageformat, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save JPEG resolution in file manager.
* @param value [I ] JPEG resolution.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetResolution(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.liveresolution, &value, sizeof(value));
	fprintf(stderr, "fSetResolution 11111\n");
	return SetSysInfo(0);
}
int fSetMaindewarpmode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.ndewarp, &value, sizeof(value));
	return SetSysInfo(0);

}
int fSet3rddewarpmode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.ndewarp3, &value, sizeof(value));
	return SetSysInfo(0);

}

/**
* @brief Save MPEG1 resolution in file manager.
* @param value [I ] MPEG1 resolution.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMPEG4Res(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.mpeg4resolution, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save MPEG2 resolution in file manager.
* @param value [I ] MPEG2 resolution.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMPEG42Res(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.mpeg42resolution, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetMPEG43Res(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.mpeg43resolution, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save MPEG2 resolution in file manager.
* @param value [I ] MPEG2 resolution.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMPEG4Quality(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.mpeg4quality, &value, sizeof(value));

	fprintf(stderr, "fSetMPEG4Quality(%d) !!\n",value);
	return SetSysInfo(0);
}
/**
* @brief Save user schedule in file manager.

* Number of schedules was define as \ref SCHDULE_NUM . User can modify the
	front 7 schedules by web site.
* @param index [I ] User schedule index.
* @param pSchedule [I ] User schedule.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSchedule(int index,Schedule_t* pSchedule, int day, int year)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->lan_config.aSchedules[index], pSchedule, sizeof(Schedule_t));
	memcpy(&pSysInfo->lan_config.schedCurDay, &day, sizeof(day));
	memcpy(&pSysInfo->lan_config.schedCurYear, &year, sizeof(year));

	return SetSysInfo(0);
}
#if DISCARD_FEATURE
/**
* @brief Save Ethernet lost status in file manager.
* @param value [I ] Ethernet lost or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetLostAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.lostalarm, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save SD card alarm enable status in file manager.
* @param value [I ] Enable alarm save to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bSdaEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif

#if SD_FEATURE
/**
* @brief Save SD card alarm enable status in file manager.
* @param value [I ] Enable alarm save to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bSdaEnable, &value, sizeof(value));

	fprintf(stderr, "fSetSDAlarmEnable !!\n");
	return SetSysInfo(0);
}
#endif

/**
* @brief Save FTP alarm enable status in file manager.
* @param value [I ] Enable alarm save to FTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFTPAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bAFtpEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save SMTP alarm enable status in file manager.
* @param value [I ] Enable alarm save to SMTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSMTPAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.bASmtpEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save alarm duration in file manager.
* @param value [I ] New alarm duration.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAlarmDuration(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmDuration, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save AVI duration in file manager.
* @param value [I ] New AVI duration.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAVIDuration(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.aviduration, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save AVI format in file manager.
* @param value [I ] New AVI format.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAVIFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.aviformat, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Set SNTP FQDN.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSntpFqdn(unsigned char value)  //by Larry
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.ntp_frequency, &value, sizeof(value));

	fprintf(stderr, "fSetSntpFqdn !!\n");
	return SetSysInfo(0);
}
/**
* @brief Save FTP file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetFTPFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ftp_config.ftpfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}
#if SD_FEATURE
/**
* @brief Save SD file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save record to SD card status in file manager.
* @param value [I ] Enable record to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSdReEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdrenable, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save SD insert status in file manager.
* @param value [I ] SD insert status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDInsert(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdinsert, &value, sizeof(value));

	fprintf(stderr, "fSetSDInsert !!\n");
	return SetSysInfo(0);
}

#endif
/**
* @brief SaveSMTP file format in file manager.
* @param value [I ] 0:MPEG4 , 1:JPEG.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAttFileFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.attfileformat, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save web site audio enable status in file manager.
* @param value [I ] Enable audio on web site or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAudioON(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioON, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioenable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAudioMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiomode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioEncode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.codectype, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioSampleRate(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.samplerate, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioBitrateValue(int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.bitrateValue, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioBitrate(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.bitrate, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioAlarmLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.alarmlevel, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetSpeakerEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.speakerenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAudioOutVolume(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiooutvolume, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetASmtpAttach(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->smtp_config.asmtpattach, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save record to FTP status in file manager.
* @param value [I ] Enable record to FTP or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetRftpenable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ftp_config.rftpenable, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0
/**
* @brief Save record to SD card status in file manager.
* @param value [I ] Enable record to SD card or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSdReEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdrenable, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save 2A mode in file manager.
* @param value [I ] New 2A mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageAEW(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAEWswitch, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save 2A mode in file manager.
* @param value [I ] New 2A mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetImageAEWType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAEWtype, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save TV cable config in file manager.
* @param value [I ] TV cable connected or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTVcable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nTVcable, &value, sizeof(value));

	fprintf(stderr, "fSetTVcable !!\n");
	return SetSysInfo(0);
}
#if BINNING_FEATURE
/**
* @brief Save binning mode in file manager.
* @param value [I ] New binning mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetBinning(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(pSysInfo->lan_config.nBinning == value)
		return 0;
	memcpy(&pSysInfo->lan_config.nBinning, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save backlight control status in file manager.
* @param value [I ] New backlight control mode.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetBlc(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nBacklightControl, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect enable status in file manager.
* @param value [I ] Enable motion detection or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motionenable, &value, sizeof(value));
	return SetSysInfo(0);
}
#if 0 //RS,DC
/**
* @brief Save motion detect choice status in file manager.
* @param value [I ] New motion detect choice status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionCEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motioncenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect level status in file manager.
* @param value [I ] New motion detect level status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionLevel(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motionlevel, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save motion detect user define value in file manager.
* @param value [I ] New motion detect user define value.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionCValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->motion_config.motioncvalue, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save motion block in file manager.
* @param pValue [I ] New motion block value.
* @param len [I ] String length.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetMotionBlock(char *pValue,int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(MOTION_BLK_LEN < len)
		return -1;
	memset(&pSysInfo->motion_config.motionblock, 0, 10);
	memcpy(&pSysInfo->motion_config.motionblock, pValue, len);
	return SetSysInfo(0);
}
#if 0
/**
* @brief Save SD insert status in file manager.
* @param value [I ] SD insert status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSDInsert(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->sdcard_config.sdinsert, &value, sizeof(value));
	return SetSysInfo(0);
}
#endif
/**
* @brief Save user acount in file manager.
* @param index [I ] User acount index.
* @param acout [I ] User acount data.
* @retval 0 Success.
* @retval -1 Fail
*/
int fDelAcount(int index,Acount_t *acout)
{
        SysInfo *pSysInfo = (SysInfo *)pShareMem;
        if(pSysInfo == NULL)
                return -1;
        memcpy(&pSysInfo->acounts[index], acout, sizeof(Acount_t));
        return SetSysInfo(0);
}

int fSetAcount(int index, Acount_t *acout)
{
        SysInfo *pSysInfo = (SysInfo *)pShareMem;
        if(pSysInfo == NULL)
                return -1;
        memcpy(&pSysInfo->acounts[index], acout, sizeof(Acount_t));
        return SetSysInfo(0);
}

#if 0
#if IPNCPTZ_FEATURE	//RS Disable,(NOT Implemented)
/**
* @brief Save IP netcam pan and zoom status in file manager.
* @param value [I ] Pan and zoom status.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetIpncPtz(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->ptz_config, &value, sizeof(value));

	return SetSysInfo(0);
}

/**
* @brief Set whether to support PTZ or not.
* @param value [I ] support PTZ or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetPtzSupport(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	if (value == pSysInfo->lan_config.net.supportptzpage)
		return 0;
	memcpy(&pSysInfo->lan_config.net.supportptzpage, &value, sizeof(value));

	fprintf(stderr, "fSetPtzSupport !!\n");
	return SetSysInfo(0);
}
#endif
#endif

/**
* @brief Save GIO input alarm status in file manager.
* @param value [I ] Enable GIO input alarm or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOInEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.gioinenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO input triger status in file manager.
* @param value [I ] High or low triger.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOInType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.giointype, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO output alarm status in file manager.
* @param value [I ] Enable alarm GIO output or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOOutEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.giooutenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save GIO outputput status in file manager.
* @param value [I ] output high or low.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetGIOOutType(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.gioouttype, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save time stamp status in file manager.
* @param value [I ] Enable time stamp or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetTStampEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Save time stamp format in file manager.
* @param value [I ] Time stamp format.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetDateFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.dateformat, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetTStampFormat(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.tstampformat, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetDatePosition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.dateposition, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetTimePosition(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.timeposition, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save alarm audio volume in file manager.
* @param value [I ] Alarm audio volume.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetAudioInVolume(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audioinvolume, &value, sizeof(value));
//    pSysInfo->audio_config.audioinvolume = value;
	fprintf(stderr, "fSetAudioinVolume %d!!\n", pSysInfo->audio_config.audioinvolume);
	return SetSysInfo(0);
}

/*
int fSetTAudioinVolume(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.audioinvolume, &value, sizeof(value));

	fprintf(stderr, "fSetTAudioinVolume %d!!\n", pSysInfo->lan_config.audioinvolume);
	return SetSysInfo(0);
}
*/

/**
* @brief Reset system to default.
* @retval 0 Success.
* @retval -1 Fail
*/
int fResetSysInfo()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_RESET;
	msgbuf.shmid = mid;
	if (pthread_mutex_trylock(&file_mag_mutex) < 0)
	{//SemWait(hndlDrvSem);
		fprintf(stderr, "pthread_mutex_trylock GetSysLog file_mag_mutex fail...\n");
		pthread_mutex_unlock(&file_mag_mutex);
		return -1;
  	}
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	pthread_mutex_unlock(&file_mag_mutex);//SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("FILE_MSG_RESET fail\n");
		return -1;
	}
	return 0;
}
/**
* @brief Reset log file to default.
* @retval 0 Success.
* @retval -1 Fail
* by Larry
*/
int fResetLogInfo()
{
	FILE_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_LOG_RESET;
	msgbuf.shmid = mid;
	if (pthread_mutex_trylock(&file_mag_mutex) < 0)
	{//SemWait(hndlDrvSem);
		fprintf(stderr, "pthread_mutex_trylock GetSysLog file_mag_mutex fail...\n");
		pthread_mutex_unlock(&file_mag_mutex);
		return -1;
  	}
#if MSG_NOWAIT
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#else
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
#endif
	pthread_mutex_unlock(&file_mag_mutex);//SemRelease(hndlDrvSem);
	if (msgbuf.length != sizeof(SysInfo))
	{
		ERR("FILE_LOG_RESET fail\n");
		return -1;
	}
	return 0;
}
/**
* @brief Save a system log in file manager.
* @param value [I ] System log.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSysLog(LogEntry_t* value)
{
#if 1
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL || value == NULL)
		return -1;
	memcpy(&pSysInfo->tCurLog, value, sizeof(LogEntry_t));
	DBG("LOG DEBUG MSG: event: %s\n", pSysInfo->tCurLog.event);
	DBG("LOG DEBUG MSG: time: %s\n", asctime(&pSysInfo->tCurLog.time));
	return SetSysInfo(1);
#else
	return 0;
#endif
}
int fContinueSetSysLog(LogEntry_t* value)//by Larry
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL || value == NULL)
		return -1;
	memcpy(&pSysInfo->tCurLog, value, sizeof(LogEntry_t));
	//DBG("event:%s\n", pSysInfo->tCurLog.event);
	//DBG("time:%s\n", asctime(&pSysInfo->tCurLog.time));
	return SetSysInfo(5);
}
/**
* @brief Update alarm status in file manager.

* This function will change alarm status in file manager.
* @param value [I ] New alarm status
* @retval -1 Fail to update.
* @retval 0 Update successfully.
*/
int fSetAlarmStatus(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.alarmstatus, &value, sizeof(value));
	return SetSysInfo(0);
}
/**
* @brief Set whether to support PTZ or not.
* @param value [I ] support PTZ or not.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetPtzSupport(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if (value == pSysInfo->lan_config.net.supportptzpage)
		return 0;
	memcpy(&pSysInfo->lan_config.net.supportptzpage, &value, sizeof(value));
	return SetSysInfo(0);
}

/**
* @brief Save .
* @param value [I ] .
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetNetMulticast(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if (value == pSysInfo->lan_config.net.multicast_enable)
		return 0;
	memcpy(&pSysInfo->lan_config.net.multicast_enable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetHttpsPort(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.https_port, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetRS485Port(unsigned short value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.rs485config, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetRateControl1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nRateControl1, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetRateControl2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nRateControl2, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDateStampEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDateStampEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDateStampEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].dstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTimeStampEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].tstampenable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nLogoEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLogoPosition3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nLogoPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextPosition1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetTextPosition2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextPosition3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nTextPosition, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetTextEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nTextEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetOverlayText1(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[0].overlaytext, buf, len);
	pSysInfo->osd_config[0].overlaytext[len] = '\0';

	return SetSysInfo(0);
}

int fSetOverlayText2(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[1].overlaytext, buf, len);
	pSysInfo->osd_config[1].overlaytext[len] = '\0';

	return SetSysInfo(0);
}

int fSetOverlayText3(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->osd_config[2].overlaytext, buf, len);
	pSysInfo->osd_config[2].overlaytext[len] = '\0';

	return SetSysInfo(0);
}
int fSetDetailInfo1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[0].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDetailInfo2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[1].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetDetailInfo3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->osd_config[2].nDetailInfo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetEncryptVideo(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nEncryptVideo, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetLocalDisplay(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nLocalDisplay, &value, sizeof(value));
	return SetSysInfo(0);
}

 int fSetIpratio1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetIpratio2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetIpratio3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].ipRatio, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetForceIframe3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].fIframe, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPInit3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].qpInit, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetQPMin3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].qpMin, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetQPMax3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].qpMax, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetMEConfig3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].meConfig, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[0].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[1].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}

int  fSetPacketSize3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_advconfig[2].packetSize, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetROIEnable1(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetROIEnable2(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetROIEnable3(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].numROI, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetStr1X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr1H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[0].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr2H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[1].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H1(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[0].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H2(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[1].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3X3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].startx, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3Y3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].starty, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3W3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].width, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetStr3H3(unsigned int value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->codec_roiconfig[2].roi[2].height, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetExpPriority(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.expPriority, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetClickSnapFilename(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(pSysInfo->lan_config.nClickSnapFilename, buf, len);
	pSysInfo->lan_config.nClickSnapFilename[len] = '\0';

	return SetSysInfo(0);
}

int fSetClickSnapStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nClickSnapStorage, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAlarmEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetExtAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nExtAlarm, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAudioAlarm(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.alarmON, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAlarmAudioPlay(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmAudioPlay, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAlarmAudioFile(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nAlarmAudioFile, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleRepeatEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleRepeatEnable, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleNumWeeks(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleNumWeeks, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetScheduleInfiniteEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.nScheduleInfiniteEnable, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetAlarmStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.alarmlocal, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetRecordStorage(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.recordlocal, &value, sizeof(value));
	return SetSysInfo(0);
}
int fSetChipConfig(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.chipConfig, &value, sizeof(value));
	return SetSysInfo(0);
}
/* System and Access log clear */
int fSetClearSysLog(unsigned char value)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_CLEAR_SYSLOG;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetClearAccessLog(unsigned char value)
{
	FILE_MSG_BUF msgbuf;

	if(eCount >= 3){
		eCount = 0;
		qid = Msg_Init(gKey);
	}
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = gProcId;
	msgbuf.cmd = FILE_MSG_CLEAR_ACCESSLOG;
	msgbuf.shmid = mid;
	SemWait(hndlDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), gProcId, 0);
	SemRelease(hndlDrvSem);
	if(msgbuf.length != sizeof(SysInfo)){
		ERR("Data read error\n");
		eCount++;
		return -1;
	}
	return 0;
}

int fSetAudioReceiverEnable(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->audio_config.audiorecvenable, &value, sizeof(value));
	return SetSysInfo(0);
}


int fSetAudioSeverIp(void *buf, int len)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	if(sizeof(pSysInfo->audio_config.audioServerIp) < len+1)
		return -1;
	memcpy(pSysInfo->audio_config.audioServerIp, buf, len);
	pSysInfo->audio_config.audioServerIp[len] = '\0';

	return SetSysInfo(0);
}

/* Onvif */
int fsetimaging(Imaging_Conf_tmp *imaging)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Imaging[imaging->position], &imaging->imaging_conf_in, sizeof(imaging->imaging_conf_in));

	return SetSysInfo(0);
}
int fsetmediaprofile(int index,media_profile *profile)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[index], profile, sizeof(media_profile));
	pSysInfo->Onvif.PrifileNum++;
	fprintf(stderr, "fsetmediaprofile pSysInfo->Onvif.PrifileNum = %d\n", pSysInfo->Onvif.PrifileNum);
	return SetSysInfo(0);
}
int faddvideosourceconfiguration(int index,video_source_conf *Vsource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Onvif.Profile[Vsource->position].AVSC, &Vsource->video_source_conf_in_t, sizeof(Vsource->video_source_conf_in_t));
	return SetSysInfo(0);

}
int fsetvideosourceconfiguration(video_source_conf *SetVsource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[SetVsource->position].AVSC, &SetVsource->video_source_conf_in_t, sizeof(SetVsource->video_source_conf_in_t));
	pSysInfo->Onvif.Util.AVSC.Vcount++;
	return SetSysInfo(0);
}
int faddaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *addaudiosource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[addaudiosource->position].AASC.Atoken, &addaudiosource->Add_AudioSource_in.Atoken_t, sizeof(addaudiosource->Add_AudioSource_in.Atoken_t));
	return SetSysInfo(0);
}
int fsetaudiosourceconfiguration(Add_AudioSource_Configuration_tmp *addaudiosource)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[addaudiosource->position].AASC, &addaudiosource->Add_AudioSource_in, sizeof(addaudiosource->Add_AudioSource_in));
	return SetSysInfo(0);
}
int faddmetadataconfiguration(Metadata_configuration_tmp *metadata)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Onvif.Profile[metadata->position].MDC, &metadata->MDC_in, sizeof(metadata->MDC_in));
	return SetSysInfo(0);

}
int fsetmetadata(Metadata_configuration_tmp *metadata)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[metadata->position].MDC, &metadata->MDC_in, sizeof(metadata->MDC_in));
	return SetSysInfo(0);
}
int faddaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *addaudioencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[addaudioencoder->position].AAEC, &addaudioencoder->Add_AudioEncoder_in, sizeof(addaudioencoder->Add_AudioEncoder_in));
	return SetSysInfo(0);
}
int fsetaudioencoderconfiguration(Add_AudioEncoder_Configuration_tmp *addaudioencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[addaudioencoder->position].AAEC, &addaudioencoder->Add_AudioEncoder_in, sizeof(addaudioencoder->Add_AudioEncoder_in));
	return SetSysInfo(0);
}
int faddvideoencoderconfiguration(video_encoder_conf *Vencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
	      return -1;
	memcpy(&pSysInfo->Onvif.Profile[Vencoder->position].AESC, &Vencoder->video_encoder_conf_in_t, sizeof(Vencoder->video_encoder_conf_in_t));
	return SetSysInfo(0);
}

int fsetvideoencoderconfiguration(video_encoder_conf *SetVencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[SetVencoder->position].AESC, &SetVencoder->video_encoder_conf_in_t, sizeof(SetVencoder->video_encoder_conf_in_t));
	return SetSysInfo(0);
}

int fdelvideoencoderconfiguration(video_encoder_conf *delVencoder)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memset(&pSysInfo->Onvif.Profile[delVencoder->position].AESC, 0, sizeof(pSysInfo->Onvif.Profile[delVencoder->position].AESC));
	return SetSysInfo(0);
}
int fdelprofile(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	SysInfo SysDefault;
	if (pSysInfo == NULL)
	   return -1;
	if (index == pSysInfo->Onvif.PrifileNum)
	{
		memset(&pSysInfo->Onvif.Profile[index], 0, sizeof(pSysInfo->Onvif.Profile[index]));
	}
	else if(index != 0)
	{
		memcpy(&pSysInfo->Onvif.Profile[index], &pSysInfo->Onvif.Profile[pSysInfo->Onvif.PrifileNum - 1], sizeof(pSysInfo->Onvif.Profile[index]));
		memset(&pSysInfo->Onvif.Profile[pSysInfo->Onvif.PrifileNum - 1], 0, sizeof(pSysInfo->Onvif.Profile[index]));
	}

	pSysInfo->Onvif.PrifileNum--;
	return SetSysInfo(0);
}

int fremoveaudiosource(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	SysInfo SysDefault;
	if(pSysInfo == NULL)
	   return -1;
	memset(&pSysInfo->Onvif.Profile[index].AASC, 0, sizeof(pSysInfo->Onvif.Profile[index].AASC));

	return SetSysInfo(0);
}

int fremoveaudioencoder(int index)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	SysInfo SysDefault;
	if(pSysInfo == NULL)
	   return -1;
         memset(&pSysInfo->Onvif.Profile[index].AAEC, 0, sizeof(pSysInfo->Onvif.Profile[index].AAEC));

	return SetSysInfo(0);
}

int fsetdiscoverymode(unsigned char mode)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.discovery_mode, &mode, sizeof(mode));
	return SetSysInfo(0);
}

int faddscopes(Onvif_Scopes_t onvif_scopes_tmp)
{
	SysInfo *oSysInfo = (SysInfo*)pShareMem;
	if(oSysInfo == NULL)
		return -1;
	/* Indices */
/*	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int itr = 0;

	while(strlen(oSysInfo->Onvif.Scopes.type[i]) != 0)
	{
		i++;
	}
	while(strlen(oSysInfo->Onvif.Scopes.location[j]) != 0)
	{
		j++;
	}
	while(strstr(onvif_scopes_tmp.type_t[k], ONVIF_SCOPE) != NULL)
	{
		k++;
	}
	while(strstr(onvif_scopes_tmp.location_t[l], ONVIF_SCOPE) != NULL)
	{
		l++;
	}

	for(itr = 0;itr < k;itr++)
	{
		memcpy(&oSysInfo->Onvif.Scopes.type[i], &onvif_scopes_tmp.type_t[itr], sizeof(onvif_scopes_tmp.type_t[itr]));
		i++;
	}
	for(itr = 0;itr < l;itr++)
	{
		memcpy(&oSysInfo->Onvif.Scopes.location[j], &onvif_scopes_tmp.location_t[itr], sizeof(onvif_scopes_tmp.location_t[itr]));
		j++;
	}
*/
	return SetSysInfo(0);
}

int fdelscopes(Onvif_Scopes_t onvif_scopes_tmp)
{
	SysInfo *oSysInfo = (SysInfo*)pShareMem;
	if(oSysInfo == NULL)
		return -1;
	/* Indices */
/*	int i = 0;
	int j = 0;
	int TYPE_MAX = 0;
	int LOC_MAX = 0;
	int num_tp = 0;
	int num_loc = 0;
	while(strstr(oSysInfo->Onvif.Scopes.type[num_tp], ONVIF_SCOPE) != NULL)
	{
		TYPE_MAX++;
		num_tp++;
	}
	while(strstr(oSysInfo->Onvif.Scopes.location[num_loc], ONVIF_SCOPE) != NULL)
	{
		LOC_MAX++;
		num_loc++;
	}
	num_tp =0;
	num_loc=0;

	while(strstr(onvif_scopes_tmp.type_t[i], ONVIF_SCOPE) != NULL)
	{
		while(strstr(oSysInfo->Onvif.Scopes.type[num_tp], ONVIF_SCOPE) != NULL)
		{
			if(strcmp(onvif_scopes_tmp.type_t[i], oSysInfo->Onvif.Scopes.type[num_tp]) == 0)
			{
				break;
			}
			num_tp++;
		}
		if(num_tp == TYPE_MAX-1)
		{
			memset(&oSysInfo->Onvif.Scopes.type[num_tp], 0, sizeof(oSysInfo->Onvif.Scopes.type[num_tp]));
		}
		else
		{
			memcpy(&oSysInfo->Onvif.Scopes.type[num_tp], &oSysInfo->Onvif.Scopes.type[TYPE_MAX-1], sizeof(oSysInfo->Onvif.Scopes.type[num_tp]));
			memset(&oSysInfo->Onvif.Scopes.type[TYPE_MAX-1], 0, sizeof(oSysInfo->Onvif.Scopes.type[TYPE_MAX-1]));
		}
		i++;
	}
	while(strstr(onvif_scopes_tmp.location_t[j], ONVIF_SCOPE) != NULL)
	{
		while(strstr(oSysInfo->Onvif.Scopes.location[num_loc], ONVIF_SCOPE) != NULL)
		{
			if(strcmp(onvif_scopes_tmp.location_t[j], oSysInfo->Onvif.Scopes.location[num_loc]) == 0)
			{
				break;
			}
			num_loc++;
		}
		if(num_loc == LOC_MAX-1)
		{
			memset(&oSysInfo->Onvif.Scopes.location[num_loc], 0, sizeof(oSysInfo->Onvif.Scopes.location[num_loc]));
		}
		else
		{
			memcpy(&oSysInfo->Onvif.Scopes.location[num_loc], &oSysInfo->Onvif.Scopes.location[LOC_MAX-1], sizeof(oSysInfo->Onvif.Scopes.location[num_loc]));
			memset(&oSysInfo->Onvif.Scopes.location[LOC_MAX-1], 0, sizeof(oSysInfo->Onvif.Scopes.location[LOC_MAX-1]));
		}
		j++;
	}
*/
}
int fsetvideoanalytics(video_analytics_conf video_analytics_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[video_analytics_tmp.position].AVAC, &video_analytics_tmp.video_analytics_conf_t, sizeof(video_analytics_tmp.video_analytics_conf_t));
	return SetSysInfo(0);
}
int fsetvideooutput(video_output_conf video_output_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Profile[video_output_tmp.position].AVOC, &video_output_tmp.video_output_conf_t, sizeof(video_output_tmp.video_output_conf_t));
	return SetSysInfo(0);
}
int fsetrelay(relay_conf relay_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Relay[relay_tmp.position].onvif_relay_in, &relay_tmp.relay_conf_in_t, sizeof(relay_tmp.relay_conf_in_t));
	return SetSysInfo(0);
}
int fsetrelaylogicalstate(relay_conf relay_tmp)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->Onvif.Relay[relay_tmp.position].relaylogicalstate, &relay_tmp.logicalstate_t, sizeof(relay_tmp.logicalstate_t));
	return SetSysInfo(0);
}
/* End-Onvif */

int fSetUpnpPortMapping(AUTO_UPNP_MAPPING_INFO *auto_portmapping)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;

	memcpy(&pSysInfo->auto_portmapping.public_http_port, &auto_portmapping->public_http_port, sizeof(unsigned short));
	memcpy(&pSysInfo->auto_portmapping.public_rtsp_port, &auto_portmapping->public_rtsp_port, sizeof(unsigned short));
	memcpy(&pSysInfo->auto_portmapping.public_udp_rtsp_port, &auto_portmapping->public_udp_rtsp_port, sizeof(unsigned short));

	return SetSysInfo(0);
}


/***************************************************************/
/*by Larry                                                     */
/***************************************************************/
void get_kernelversion(void)
{
	FILE *fptr;
	char TempBuff[50];
	char *pStr[3];
	int buffsize = sizeof(TempBuff);
	int cnt = 0;
	SysInfo *pSysInfo = GetSysInfo();

	fptr = fopen("/proc/version","r");
	if (fptr == NULL)
	{
		fprintf(stderr,"\n Can't get Version information ");
		return;
	}
	fread( TempBuff, buffsize-1, 1,fptr);
	TempBuff[buffsize-1] = '\0';

	pStr[0] = strtok(TempBuff, " ");

	for (cnt = 1;cnt < 3;cnt++ )
		pStr[cnt] = strtok(NULL, " ");

	fclose(fptr);

	sprintf(pSysInfo->extrainfo.boot_version, "%s %s %s ",pStr[0],pStr[1],pStr[2]);
}

int CheckIPv4Format(char *ipv4_addr)
{
	unsigned int ip_element[4];
	int i = 0;

	char* token = strtok(ipv4_addr, DELIM);
	while (token)
	{
		ip_element[i] = (unsigned int)atoi(token);
		if (ip_element[i] < 0 || ip_element[i] > 255)
		{
			//printf("[Invaild]out of range : %d\n", ip_element[i]);
			return -1;
		}
		token = strtok(NULL, DELIM);
		i++;
	}
	if (i != 4)
	{
		//printf("[Invaild]incorrect format\n");
		return -1;
	}
	//printf("Vaild IP\n");
	return 0;
}

int GetMtdSNandPID(char *sn, char *pid, char *hdVer, int sn_size, int pid_size, int hd_size)
{
	FILE *sys_fd;
	char env_buf[mtd1_size] = "";
	//int i;
	int offset = 0;
	int ret = -1;

	memset(sn, 0, sn_size);
	memset(pid, 0, pid_size);
	memset(hdVer, 0, hd_size);

	system("/bin/dd if=/dev/mtdblock1 of=/tmp/mtd_env");
	sys_fd = fopen("/tmp/mtd_env", "rb");
	if (sys_fd == NULL)
		return ret;

	fseek(sys_fd, 0, SEEK_CUR);//fseek(sys_fd, env_offset, SEEK_CUR);
	fread(env_buf, 1, env_size, sys_fd);
	fclose(sys_fd);
	unlink("/tmp/mtd_env");

  	offset = 0;
againe_sn:
	while (1)
	{
		char *tmp = NULL;

		if (offset >= search_size)
		{
			ret = -1;
			break;
		}

		if (offset >= env_size)
		{
			ret = 2;
			break;
		}

		if ((tmp = strstr(env_buf+offset, "sn=")) != NULL)
		{
			int o_set = tmp - env_buf + 3;
      		memcpy(sn, env_buf+o_set, 11);
			ret = 0;
			break;
		}
		else
		{
			offset += 1;
		}
	}
	if (ret < 0)
		goto againe_sn;

  	offset = 0;
againe_pid:
	while (1)
	{
		char *tmp = NULL;

		if (offset >= search_size)
		{
			ret = -1;
			break;
		}

		if (offset >= env_size)
		{
			ret = 2;
			break;
		}

		if ((tmp = strstr(env_buf+offset, "pid=")) != NULL)
		{
			int o_set = tmp - env_buf + 4;
      		memcpy(pid, env_buf+o_set, 10);
			ret = 0;
			break;
		}
		else
		{
			offset += 1;
		}
	}
	if (ret < 0)
		goto againe_pid;

	offset = 0;
againe_hd:
	while (1)
	{
		char *tmp = NULL;

		if (offset >= search_size)
		{
			ret = -1;
			break;
		}

		if (offset >= env_size)
		{
			ret = 2;
			break;
		}

		if ((tmp = strstr(env_buf+offset, "hwversion=")) != NULL)
		{
			int o_set = tmp - env_buf + 10;
      		memcpy(hdVer, env_buf+o_set, 7);
			ret = 0;
			break;
		}
		else
		{
			offset += 1;
		}
	}
	if (ret < 0)
		goto againe_hd;

  	printf("sn = %s\n", sn);
	printf("pid = %s\n", pid);
	printf("hdVer = %s\n", hdVer);
	return ret;
}

int CheckEthIPV6Addr(char *ipv6_global_addr, char *ipv6_link_addr, int size)
{
	FILE *ipv6_file = NULL;
	char *ip6_info = NULL;
	char *get_info_ptr = NULL, *get_info_ptr_1 = NULL, *get_info_ptr_2 = NULL;
	long file_length = 0;

	unlink("/tmp/inet6_addr_info");
	system("ifconfig eth0 |grep \"inet6 addr:\" >> /tmp/inet6_addr_info");

	ipv6_file = fopen("/tmp/inet6_addr_info", "rb");
	if (ipv6_file == NULL)
		return -1;

	fseek(ipv6_file, 0, SEEK_END);
	file_length = ftell(ipv6_file);
	ip6_info = (char *)malloc(sizeof(char) * file_length);

	rewind(ipv6_file);
  	fread(ip6_info, 1, file_length, ipv6_file);
	fclose(ipv6_file);

	get_info_ptr = (char *)ip6_info;
	if (NULL != (get_info_ptr_1 = strstr( get_info_ptr, "inet6 addr:" )))
	{
	    if (NULL != (get_info_ptr_2 = strstr( get_info_ptr_1, "Scope:Global")))
		{
	       	get_info_ptr_1 += 12;
	       	memset(ipv6_global_addr, 0, sizeof(ipv6_global_addr));
			memcpy(ipv6_global_addr, get_info_ptr_1, get_info_ptr_2 - get_info_ptr_1);
  			printf("ipv6_addr_global = %s  %d\n", ipv6_global_addr, strlen(ipv6_global_addr));

			if (strlen(ipv6_global_addr) <= size && strlen(ipv6_global_addr) > 0)
				get_info_ptr = get_info_ptr_1 + strlen(ipv6_global_addr);
		}
	}

	if (NULL != (get_info_ptr_1 = strstr( get_info_ptr, "inet6 addr:" )))
	{
	    if (NULL != (get_info_ptr_2 = strstr( get_info_ptr_1, "Scope:Link")))
		{
	       	get_info_ptr_1 += 12;
	       	memset(ipv6_link_addr, 0, sizeof(ipv6_link_addr));
			memcpy(ipv6_link_addr, get_info_ptr_1, get_info_ptr_2 - get_info_ptr_1);
         	printf("ipv6_addr_link = %s %d\n", ipv6_link_addr, strlen(ipv6_link_addr));

			if (strlen(ipv6_link_addr) > size || strlen(ipv6_link_addr) == 0)
			{
			  	free(ip6_info);
				return -1;
			}
		}
	}
	else
	{
    	free(ip6_info);
	  	return -1;
  	}

  	free(ip6_info);
	return 0;
}

int Get_IPV6Addr(char *ipv6_addr, int size)
{
	FILE *ipv6_file = NULL;
	char *ip6_info = NULL;
  	long file_length = 0;
	/* Get Server IP */
	//unlink("/tmp/inet6_addr");
	//unlink("/tmp/inet6_type");
	//system("ifconfig eth0 |grep \"inet6 addr:\" >> /tmp/inet6_info");
  //system("ifconfig eth0 | awk \'/inet6/ {print $3}\' >> /tmp/inet6_addr");
  //system("ifconfig eth0 | awk \'/inet6/ {print $4}\' >> /tmp/inet6_type");

  	ipv6_file = fopen("/tmp/ipv6_addr", "rb");
  	if (ipv6_file == NULL)
  		return -1;

	fseek(ipv6_file, 0, SEEK_END);
	file_length = ftell(ipv6_file);

	if (file_length == 0)
		return -1;

	ip6_info = (char *)malloc(sizeof(char) * file_length);

	rewind(ipv6_file);
  	fread(ip6_info, 1, file_length, ipv6_file);
	fclose(ipv6_file);

	fprintf(stderr, "ipv6_info = %s\n", ip6_info);
	if (file_length > size)
	  	memcpy(ipv6_addr, ip6_info, size);
	else
		memcpy(ipv6_addr, ip6_info, file_length);

	free(ip6_info);
	return 0;
}

int netsplit(char *pAddress, void *ip)
{
	unsigned int ret;
	NET_IPV4 *ipaddr = (NET_IPV4 *)ip;

	if ((ret = atoi(pAddress + 9)) > 255)
		return 0;
	ipaddr->str[3] = ret;

	*( pAddress + 9 ) = '\x0';
	if ((ret = atoi(pAddress + 6)) > 255)
		return 0;
	ipaddr->str[2] = ret;

	*( pAddress + 6 ) = '\x0';
	if ((ret = atoi(pAddress + 3)) > 255)
		return 0;
	ipaddr->str[1] = ret;

	*( pAddress + 3 ) = '\x0';
	if ((ret = atoi(pAddress + 0)) > 255)
		return 0;
	ipaddr->str[0] = ret;

	return 1;
}

int ipv4_str_to_num(char *data, struct in_addr *ipaddr)
{
	char data_buf[16] = "";
	int ip_0, ip_1, ip_2, ip_3;

	if ( strchr(data, '.') == NULL )
		return netsplit(data, ipaddr);

  //by Larry
  	sscanf(data, "%d.%d.%d.%d", &ip_0, &ip_1, &ip_2, &ip_3);
  	sprintf(data_buf, "%d.%d.%d.%d", ip_0, ip_1, ip_2, ip_3);

	//return inet_aton(data, ipaddr);
	return inet_aton(data_buf, ipaddr);
}

#if 0
Motion_Str_Data *GetMDData(char *md_data)
{
	int enable = 0, left = 0, top = 0, right = 0, bottom = 0, sen = 0;
	char *ptr;

	ptr = md_data;

	memset(&MD_Str_Data, 0, sizeof(Motion_Str_Data));

	sscanf(ptr, "%d", &enable);
	if (NULL != (ptr = strstr(ptr, ",")))
	 	ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &left);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

	sscanf(ptr, "%d", &top);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &right);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

	sscanf(ptr, "%d", &bottom);
	if (NULL != (ptr = strstr(ptr, ",")))
        ptr++;
	else
		return NULL;

    sscanf(ptr, "%d", &sen);

    MD_Str_Data.enable = enable;
    MD_Str_Data.left   = left;
    MD_Str_Data.top    = top;
    MD_Str_Data.right  = right;
    MD_Str_Data.bottom = bottom;
    MD_Str_Data.sen    = sen;
    return (Motion_Str_Data *)&MD_Str_Data;
}
#endif
Extra_Info *GetExtraInfo(void)
{
#if 0
	FILE *fp = fopen(EXTRA_INFO_PATH, "rb");
  	if (fp == NULL)
    	fExtraFileReadCheck();
  	else
  		fclose(fp);

  	return (Extra_Info *)&extra_info;
#else
	SysInfo *pSysInfo = GetSysInfo();

  	return (Extra_Info *)&pSysInfo->extrainfo;
#endif
}
#if 0
Extra_Info2 *GetExtraInfo2(void)
{
#if 0
	FILE *fp = fopen(EXTRA_INFO2_PATH, "rb");
  	if (fp == NULL)
    	fExtraFile2ReadCheck();
  	else
  		fclose(fp);

  	return (Extra_Info2 *)&extra_info2;
#else
	SysInfo *pSysInfo = GetSysInfo();

  	return (Extra_Info *)&pSysInfo->extrainfo2;
#endif
}
#endif
int fsethostname(hostname_tmp *host)
{
 	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if(pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.hostname, &host->hostname_t, sizeof(host->hostname_t));
	return SetSysInfo(0);
}

#if WIFI_FEATURE

//alice_modify_2012_3_15_v1

int SemWifiInit(void)
{
	union semwifiun semopts;
	semopts.val=1;
	int semid,i;
	if ((semid = semget(WIFI_SEM_WIFI_key, 2, 0644 |IPC_CREAT| IPC_EXCL)) < 0)
	{
		if (errno == EEXIST)
			semid = semget(WIFI_SEM_WIFI_key, 1, 0);
		if (semid < -1)
			return -1;
	}
	else
	{
		for (i=0;i<1;i++)
		{
        	if (semctl(semid, i, SETVAL, semopts) < 0)
        	{
	        	semctl(semid, 0, IPC_RMID);
	        	return -1;
			}
    	}
	}
	return 0;

}

int ShmWifiLock(int locknum)
{
	int semid;
	struct sembuf op;
	if ((semid = semget(WIFI_SEM_WIFI_key, 1, 0644)) < 0)
		return -1;

	op.sem_num = locknum;
	op.sem_op = -1;
	op.sem_flg = SEM_UNDO;
	if (semop(semid,&op,1) < 0)
		return -1;
	return 0;
}
int ShmWifiUnLock(int locknum)
{
	int semid;
	struct sembuf op;
	if ((semid = semget(WIFI_SEM_WIFI_key, 1, 0644)) < 0)
		return -1;

	op.sem_num = locknum;
	op.sem_op = 1;
	op.sem_flg = SEM_UNDO;
	if (semop(semid,&op,1)<0)
		return -1;
	return 0;

}
int ShareMemWifiInfoDataInit(void)
{
	wifi_wps_data_t	wifi_wps_data;

	memset(&wifi_wps_data,0,sizeof(wifi_wps_data_t));

	ShmWifiInfoWrite(&wifi_wps_data);
	return 0;
}
int ShareMemsWifiInfoInit(void)  //by Larry
{
	int mid;
	mid = shmget(WIFI_WPS_INFO_KEY, sizeof (1024), IPC_CREAT | 0660);
	if (mid < 0)
		return -1;
	return mid;
}

int ShmWifiInfoRead (wifi_wps_data_t * wifi_wps_data)
{
	int mid;
	wifi_wps_data_t *getWifiWpsdataptr;
	if (ShmWifiLock(0) < 0)
		return -1;
	mid = shmget(WIFI_WPS_INFO_KEY, 0, 0);
	if (mid == -1)
	{
		ShmWifiUnLock(0);
		return -1;
	}
	getWifiWpsdataptr=(wifi_wps_data_t *)shmat(mid,0,0);
	memcpy(wifi_wps_data, getWifiWpsdataptr, sizeof(wifi_wps_data_t));
	shmdt(getWifiWpsdataptr);
	if (ShmWifiUnLock(0) < 0)
		return -1;
	return 0;
}
int ShmWifiInfoWrite (wifi_wps_data_t * wifi_wps_data)
{
	int mid;
	wifi_wps_data_t *WrWifiWpsdataptr;
	if (ShmWifiLock(0) < 0)
		return -1;
	mid = shmget(WIFI_WPS_INFO_KEY, 0, 0);
	if (mid == -1)
	{
		ShmWifiUnLock(0);
		return -1;
	}
	WrWifiWpsdataptr=(wifi_wps_data_t *)shmat(mid,0,0);
	memcpy(WrWifiWpsdataptr, wifi_wps_data, sizeof(wifi_wps_data_t));
	shmdt(WrWifiWpsdataptr);
	if (ShmWifiUnLock(0) < 0)
		return -1;
	return 0;
}
//alice_modify_2012_3_15_v1
#endif  // WIFI_FEATURE
int extra_info_init(void)
{
	FILE *fp, *vender_fp;
	unsigned char i, mac_addr[6] = "";

	SysInfo *pSysInfo = GetSysInfo();
	memset(&pSysInfo->extrainfo/*&extra_info*/, 0, sizeof(Extra_Info));

	//fp = fopen(EXTRA_INFO_PATH, "wb");

	net_get_hwaddr_util(ETH_NAME, mac_addr);
	get_kernelversion();

	sprintf(pSysInfo->extrainfo.mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

 	vender_fp = fopen(VENDER_CONF, "rb");
	strcpy(pSysInfo->extrainfo.app_version, APP_VERSION_DEFAULT);
 	if (vender_fp != NULL)
	{
		char ver_buf[32] = "", *FileBuf_Ptr, *FileBuf;
		unsigned long file_length = 0;

		fseek(vender_fp, 0, SEEK_END);
		file_length = ftell(vender_fp);
		rewind(vender_fp);
		FileBuf = (char *)malloc(sizeof(char) * file_length);
		fread(FileBuf, 1, file_length, vender_fp);

		FileBuf_Ptr = FileBuf;
		if (NULL != (FileBuf_Ptr = strstr(FileBuf_Ptr, "VERSION")))
		{
			sscanf(FileBuf_Ptr, "VERSION=%s", ver_buf);
			if (strlen(ver_buf))
			{
				memset(pSysInfo->extrainfo.app_version, 0, sizeof(pSysInfo->extrainfo.app_version));
				strcpy(pSysInfo->extrainfo.app_version, ver_buf);
			}
		}
		free(FileBuf);
		fclose(vender_fp);
	}

	strcpy(pSysInfo->extrainfo.app_version_date, APP_VERSION_DATE_DEFAULT);
	strcpy(pSysInfo->extrainfo.boot_version_date, BOOT_VERSION_DATE_DEFAULT);
	strcpy(pSysInfo->extrainfo.hardware_version, HARDWARE_VERSION_DEFAULT);
	strcpy(pSysInfo->extrainfo.system_object_id, SYSTEM_OBJECT_ID_DEFAULT);
	strcpy(pSysInfo->extrainfo.ipcam_location, IPCAM_LOCATION_DEFAULT);

	pSysInfo->extrainfo.notification_mode.tcp_port = 80;
	strcpy(pSysInfo->extrainfo.network_mode.snmp_readcommunity, SNMP_READCOMMUNITY_DEFAULY);
	strcpy(pSysInfo->extrainfo.network_mode.snmp_writecommunity, SNMP_WRITECOMMUNITY_DEFAULY);

	pSysInfo->extrainfo.bonjour_mode.upnp_mode = 1;
	pSysInfo->extrainfo.bonjour_mode.upnp_discover = 1;
	pSysInfo->extrainfo.bonjour_mode.bonjour_mode = 1;
	strcpy(pSysInfo->extrainfo.bonjour_mode.friendly_name, BONJOUR_NAME_DEFAULT);

//	pSysInfo->extrainfo.https_mode.https_mode = 1;
//	pSysInfo->extrainfo.https_mode.https_port = 443;

	strcpy(pSysInfo->extrainfo.imageseting_mode.imagetext, OVERLAY_IMAGE_TEXT);

	pSysInfo->extrainfo.general_net_mode.ipmode   = 0;
	pSysInfo->extrainfo.general_net_mode.rtspport = 554;
	pSysInfo->extrainfo.general_net_mode.rtpport  = 6660;

	strcpy(pSysInfo->extrainfo.event_server.jabber_message, JABBER_MSG_DEFAULT);

	for (i = 0; i < SCHEDULE_NUM; i++)
	 	strcpy((char*)pSysInfo->extrainfo.event_schedule[i].schedule_time.nDay, SCH_DAY_DEFAULT);

	//for Upnp AV
	strcpy((char*)pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.nDay, SCH_ALL_DAY_DEFAULT);
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tStart.nHour = 00;
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tStart.nMin  = 00;
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tStart.nSec  = 00;
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tDuration.nHour = 23;
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tDuration.nMin  = 59;
	pSysInfo->extrainfo.event_schedule[SCHEDULE_NUM].schedule_time.tDuration.nSec  = 59;

	extro_acount_index = 0;
	for (i = 0; i < ACOUNT_NUM; i++)
		pSysInfo->extrainfo.acounts[i].authority = 9;

	pSysInfo->extrainfo.xml_info.log_num = 500;

	for (i = 0; i < MAX_MD_NUM; i++)
	{
		strcpy((char*)pSysInfo->motion_config.md_data[i].motionblock, MD_BLOCK_DEFAULT);				/* Dean - 20140325 */
		strcpy((char*)pSysInfo->motion_config.md_data[i].motionblock_coordinates, MB_COORINATES);		/* Dean - 20140325 */
		//extrainfo.md_data[i].motioncvalue = MD_SENSITIVITY; //for RS test
	}

	strcpy((char*)pSysInfo->extrainfo.ptz_data[0].ptzspeed, PTZ_SPEED_DEFAULT); //RS, DC
	pSysInfo->extrainfo.bonjour_mode.upnp_traversal = UPNP_TRAVERAL_PORT;
	pSysInfo->extrainfo.ftpalarm_mode.postalarm     = MAX_POST_ALARM_TIME;//5;//POST_ALARM_DEFAULT;

	//system("rm /mnt/nand/user.yuv");
	unlink("/mnt/nand/user.yuv");

	//extrainfo.imageseting_mode.overlaydate = 1; //RS, Discard

	strcpy((char*)pSysInfo->extrainfo.network_mode.snmp_username, SNMP_USERNAME);
	strcpy((char*)pSysInfo->extrainfo.network_mode.snmp_auth_pw, SNMP_AUTH_PW);
	strcpy((char*)pSysInfo->extrainfo.network_mode.snmp_privacy_pw, SNMP_PRIVACY_PW);

	pSysInfo->extrainfo.audio_info.audioinvolume = AUDIO_IN_VOLUME;
	pSysInfo->extrainfo.audio_info.gain = DEFAULT_GAIN;

	pSysInfo->extrainfo.date_time_mode = DATE_TIME_MODE;
#if WIFI_FEATURE
//alice_modify_2012_4_11_v1
	pSysInfo->extrainfo.wifi.enable = 1;
//alice_modify_2012_4_11_v1
//alice_modify_2012_3_27_v1
	pSysInfo->extrainfo.wifi.wifiConfig.mode = WIFI_MODE_INFRA;
	pSysInfo->extrainfo.wifi.wifiConfig.channel = 1;
	pSysInfo->extrainfo.wifi.wifiConfig.security = WIFI_WEP_ENCR_NONE;
	pSysInfo->extrainfo.wifi.wifiConfig.wepauth = WIFI_WEP_AUTH_NONE;
	pSysInfo->extrainfo.wifi.wifiConfig.wepkeyfmt = WIFI_WEP_ENC_ASCII_64;
	pSysInfo->extrainfo.wifi.wifiConfig.wcountryreg = WIFI_FCC;
	pSysInfo->extrainfo.wifi.wifiConfig.wbandwidth = WIFI_BAND_WIDTH_20; //alice_modify_2012_6_29_v1
	pSysInfo->extrainfo.wifi.wifitestbegin = WIFI_TEST_END; //alice_modify_2012_5_10_v1
	strcpy(pSysInfo->extrainfo.wifi.wifiConfig.ssid, WIFI_DEFAULT_SSID);
//alice_modify_2012_3_27_v1
#endif  // WIFI_FEATURE
#if UPLOAD_FEATURE
	pSysInfo->extrainfo.upload_record.enable=UPLOAD_ENABLE;
	pSysInfo->extrainfo.upload_record.MediaType=UPLOAD_MEDIA_TYPE;
	pSysInfo->extrainfo.upload_record.MediaFileTm=UPLOAD_MEDIAFILE_TM;
	strcpy(pSysInfo->extrainfo.upload_record.MediaPrefixName,UPLOAD_MEDIA_PREFIX_NAME);
	pSysInfo->extrainfo.upload_record.Mediatimestamp=UPLOAD_MEDIA_TIMESTAMP;
	pSysInfo->extrainfo.upload_record.PeriodValue=UPLOAD_PERIOD_VALUE;
	pSysInfo->extrainfo.upload_record.PeriodUnit=UPLOAD_PERIOD_UNIT;
#endif
	//extrainfo.global_ptz_patrol = GLOBAL_PTZ_PATROL;//RS, Discard

	pSysInfo->samba_record.schedule_time.tDuration.nHour = 23;
	pSysInfo->samba_record.schedule_time.tDuration.nMin  = 59;
	pSysInfo->samba_record.schedule_time.tDuration.nSec  = 59;
	pSysInfo->samba_record.samba_space_max = SAMBA_RECORD_SPACE_MAX_SIZE;				/* Record Media - Dean - 20130530 */
	pSysInfo->samba_record.enable = RECORD_MEDIA_DEFAULT;								/* Samba Record Modification - Dean - 20131008 */
#if 0
	/* Privacy Mask Modification - Dean - 20131007 - Begin */
#if PRIVACY_MASK_FEATURE
	int loopi;
	for (loopi = 0; loopi < PRIVACY_MASK_ZONE_MAX; loopi++)
		snprintf(extrainfo.pm_info.zone[loopi].title, sizeof(extrainfo.pm_info.zone[loopi].title) - 1,"%s%d", PRIVACY_MASK_ZONE_TITLE_STR_DEFAULT, loopi + 1);
#endif
	/* Privacy Mask Modification - Dean - 20131007 - End */
	/* TAMPER_FEATURE - Dean - 20131007 - Begin */
#if TAMPER_FEATURE
	extrainfo.tamper_info.sensitivity = TAMPER_SENSITIVITY_DEFAULT;
	extrainfo.tamper_info.focus_value = TAMPER_FOCUS_VALUE_DEFAULT;
	extrainfo.tamper_info.detect_duration = TAMPER_DURATION_DEFAULT;
#endif
	/* TAMPER_FEATURE - Dean - 20131007 - End */
#endif
	//fwrite((void *)&extrainfo, sizeof(extrainfo), 1, fp);
	//fclose(fp);
	return 1;
}
/*
int extra_info2_init(void)
{
	FILE *fp;

	SysInfo *pSysInfo = GetSysInfo();
	memset(&pSysInfo->extrainfo2, 0, sizeof(Extra_Info2));

	//fp = fopen(EXTRA_INFO2_PATH, "wb");

	int loopi;
	for (loopi = 0; loopi < PRIVACY_MASK_ZONE_MAX; loopi++)
		snprintf(pSysInfo->extrainfo2.pm_info.zone[loopi].title, sizeof(pSysInfo->extrainfo2.pm_info.zone[loopi].title) - 1,"%s%d", PRIVACY_MASK_ZONE_TITLE_STR_DEFAULT, loopi + 1);
	pSysInfo->extrainfo2.tamper_info.sensitivity = TAMPER_SENSITIVITY_DEFAULT;
	pSysInfo->extrainfo2.tamper_info.focus_value = TAMPER_FOCUS_VALUE_DEFAULT;
	pSysInfo->extrainfo2.tamper_info.detect_duration = TAMPER_DURATION_DEFAULT;
	//fwrite((void *)&pSysInfo->extrainfo2, sizeof(Extra_Info2), 1, fp);
	//fclose(fp);
	return 1;
}
*/
int fReadExtraInfo(void)
{
#if 0
	FILE *fp, *vender_fp;
	int file_length = 0;

	fp = fopen(EXTRA_INFO_PATH, "rb");
	if (NULL == fp)
	{
		extra_info_init();
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);

	rewind(fp);
	if (file_length != sizeof(Extra_Info))
	{ //check file
		fclose(fp);
		extra_info_init();
		return -1;
	}

	fread((void *)&extra_info, sizeof(Extra_Info), 1, fp);
	fclose(fp);

	/* get vender */
	vender_fp = fopen(VENDER_CONF, "rb");

	strcpy(extra_info.app_version, APP_VERSION_DEFAULT);
 	if (vender_fp != NULL)
	{
		char ver_buf[32] = "", *FileBuf_Ptr, *FileBuf;
		unsigned long file_length = 0;

		fseek(vender_fp, 0, SEEK_END);
		file_length = ftell(vender_fp);
		rewind(vender_fp);
		FileBuf = (char *)malloc(sizeof(char) * file_length);
		fread(FileBuf, 1, file_length, vender_fp);

		FileBuf_Ptr = FileBuf;
		if (NULL != (FileBuf_Ptr = strstr(FileBuf_Ptr, "VERSION")))
		{
			sscanf(FileBuf_Ptr, "VERSION=%s", ver_buf);
			if (strlen(ver_buf))
			{
				memset(extra_info.app_version, 0, sizeof(extra_info.app_version));
				strcpy(extra_info.app_version, ver_buf);
			}
		}
		free(FileBuf);
		fclose(vender_fp);
	}
#endif
	return 1;
}

#if 0
int fReadExtraInfo2(void)
{
	FILE *fp;
	int file_length = 0;

	fp = fopen(EXTRA_INFO2_PATH, "rb");
	if (NULL == fp)
	{
		extra_info2_init();
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);

	rewind(fp);
	if (file_length != sizeof(Extra_Info2))
	{ //check file
		fclose(fp);
		extra_info2_init();
		return -1;
	}

	fread((void *)&extra_info2, sizeof(Extra_Info2), 1, fp);
	fclose(fp);

	return 1;
}
#endif

int fExtraFileReadCheck(void)
{
  	int fdlock, re_try = 0;

	fdlock = open("/tmp/.extro_file_lock", O_RDWR | O_CREAT, 0644);

	while (1)
	{
		if (flock(fdlock, LOCK_EX) < 0)
			fprintf(stderr, "Read File Lock Error !!!\n");
		else
		{
			//fprintf(stderr, "Read File Lock OK !!!\n");
			fReadExtraInfo();
			system("sync;sync;sync");

RE_FILE_UNLOCK:
			if (flock(fdlock, LOCK_UN) < 0)
			{
			  	//fprintf(stderr, "Read File UnLock Error !!!\n");
				if (re_try++ > 5)
				{
				  	close(fdlock);
				  	unlink("/tmp/.extro_file_lock");
				  	return -1;
				}
			  	goto RE_FILE_UNLOCK;
			}
			else
			{
			  	//fprintf(stderr, "Read File UnLock OK !!!\n");
			  	break;
		  	}
		}

		if (re_try++ > 5)
		{
		  	close(fdlock);
		  	unlink("/tmp/.extro_file_lock");
		  	return -1;
		}

		sleep(1);
  	}

	close(fdlock);
	unlink("/tmp/.extro_file_lock");
	return 1;
}

#if 0
int fExtraFile2ReadCheck(void)
{
  	int fdlock, re_try = 0;

	fdlock = open("/tmp/.extro_file2_lock", O_RDWR | O_CREAT, 0644);

	while (1)
	{
		if (flock(fdlock, LOCK_EX) < 0)
			fprintf(stderr, "Read File Lock Error !!!\n");
		else
		{
			//fprintf(stderr, "Read File Lock OK !!!\n");
			fReadExtraInfo2();
			system("sync;sync;sync");

RE_FILE_UNLOCK:
			if (flock(fdlock, LOCK_UN) < 0)
			{
			  	//fprintf(stderr, "Read File UnLock Error !!!\n");
				if (re_try++ > 5)
				{
				  	close(fdlock);
				  	unlink("/tmp/.extro_file2_lock");
				  	return -1;
				}
			  	goto RE_FILE_UNLOCK;
			}
			else
			{
			  	//fprintf(stderr, "Read File UnLock OK !!!\n");
			  	break;
		  	}
		}

		if (re_try++ > 5)
		{
		  	close(fdlock);
		  	unlink("/tmp/.extro_file2_lock");
		  	return -1;
		}

		sleep(1);
  	}

	close(fdlock);
	unlink("/tmp/.extro_file2_lock");
	return 1;
}
#endif
int fWriteExtraInfo(unsigned int field, char *data)
{
	FILE *fp;
	int time_hour, time_min, upnp_port, multicast, rtsp;
	char *ptr_1, *ptr_2, *acount_ptr, acount_buf[256] = "", rtsp_multi[128] = "";

    SysInfo *pSysInfo = GetSysInfo();

	switch (field)
	{
	case SET_SN:
		memset(pSysInfo->extrainfo.sn, 0, sizeof(pSysInfo->extrainfo.sn));
		memcpy(pSysInfo->extrainfo.sn, data, sizeof(pSysInfo->extrainfo.sn)-1);
		break;
	case SET_PID:
		memset(pSysInfo->extrainfo.pid, 0, sizeof(pSysInfo->extrainfo.pid));
		memcpy(pSysInfo->extrainfo.pid, data, sizeof(pSysInfo->extrainfo.pid)-1);
		break;
	case SET_MAC:
		memset(pSysInfo->extrainfo.mac, 0, sizeof(pSysInfo->extrainfo.mac));
		memcpy(pSysInfo->extrainfo.mac, data, sizeof(pSysInfo->extrainfo.mac)-1);
		//strcpy(&extrainfo.mac, data);
		break;
	case SET_APP_VER:
		memset(pSysInfo->extrainfo.app_version, 0, sizeof(pSysInfo->extrainfo.app_version));
		memcpy(pSysInfo->extrainfo.app_version, data, sizeof(pSysInfo->extrainfo.app_version)-1);
		//strcpy(&extrainfo.app_version, data);
		break;
	case SET_APP_VER_DATE:
		memset(pSysInfo->extrainfo.app_version_date, 0, sizeof(pSysInfo->extrainfo.app_version_date));
		memcpy(pSysInfo->extrainfo.app_version_date, data, sizeof(pSysInfo->extrainfo.app_version_date)-1);
		//strcpy(&extrainfo.app_version_date, data);
		break;
	case SET_BOOT_VER:
		memset(pSysInfo->extrainfo.boot_version, 0, sizeof(pSysInfo->extrainfo.boot_version));
		memcpy(pSysInfo->extrainfo.boot_version, data, sizeof(pSysInfo->extrainfo.boot_version)-1);
		//strcpy(extrainfo.boot_version, data);
		break;
	case SET_BOOT_VER_DATE:
		memset(pSysInfo->extrainfo.boot_version_date, 0, sizeof(pSysInfo->extrainfo.boot_version_date));
		memcpy(pSysInfo->extrainfo.boot_version_date, data, sizeof(pSysInfo->extrainfo.boot_version_date)-1);
		//strcpy(&extrainfo.boot_version_date, data);
		break;
	case SET_HW_VER:
		memset(pSysInfo->extrainfo.hardware_version, 0, sizeof(pSysInfo->extrainfo.hardware_version));
		memcpy(pSysInfo->extrainfo.hardware_version, data, sizeof(pSysInfo->extrainfo.hardware_version)-1);
		//strcpy(&extrainfo.hardware_version, data);
		break;
	case SET_SYS_OBJ_ID:
		memset(pSysInfo->extrainfo.system_object_id, 0, sizeof(pSysInfo->extrainfo.system_object_id));
		memcpy(pSysInfo->extrainfo.system_object_id, data, sizeof(pSysInfo->extrainfo.system_object_id)-1);
		//strcpy(&extrainfo.system_object_id, data);
		break;
	case SET_IPCAM_LOCATION:
		memset(pSysInfo->extrainfo.ipcam_location, 0, sizeof(pSysInfo->extrainfo.ipcam_location));
		memcpy(pSysInfo->extrainfo.ipcam_location, data, sizeof(pSysInfo->extrainfo.ipcam_location)-1);
		//strcpy(&extrainfo.ipcam_location, data);
		break;
	case SET_IPCAM_CAMERA_NAME:
		memset(pSysInfo->extrainfo.camera_name, 0, sizeof(pSysInfo->extrainfo.camera_name));
		memcpy(pSysInfo->extrainfo.camera_name, data, sizeof(pSysInfo->extrainfo.camera_name)-1);
		//strcpy(&extrainfo.camera_name, data);
		break;
	case SET_IPCAM_LED:
		pSysInfo->extrainfo.ipcam_led = atoi(data);
		break;
	case SET_QOS_MODE:
		pSysInfo->extrainfo.network_mode.cos_mode = atoi(data);
		break;
	case SET_QOS_TYPE:
		pSysInfo->extrainfo.network_mode.qos_type = atoi(data);
		break;
	case SET_QOS_VIDEO_DSCP:
		pSysInfo->extrainfo.network_mode.qos_video_dscp = atoi(data);
		break;
	case SET_QOS_AUDIO_DSCP:
		pSysInfo->extrainfo.network_mode.qos_audio_dscp = atoi(data);
		break;
	case SET_COS_MODE:
		pSysInfo->extrainfo.network_mode.cos_mode = atoi(data);
		break;
	case SET_COS_VLANID:
		pSysInfo->extrainfo.network_mode.cos_vlanid = atoi(data);
		break;
	case SET_COS_VIDEOPRIORITY:
		pSysInfo->extrainfo.network_mode.cos_videopriority = atoi(data);
		break;
	case SET_COS_AUDIOPRIORITY:
		pSysInfo->extrainfo.network_mode.cos_audiopriority = atoi(data);
		break;
	case SET_COS_ADDRESS:
		memset(&pSysInfo->extrainfo.network_mode.cos_address, 0, sizeof(pSysInfo->extrainfo.network_mode.cos_address));
		memcpy(&pSysInfo->extrainfo.network_mode.cos_address, data, sizeof(pSysInfo->extrainfo.network_mode.cos_address)-1);
		//strcpy(&extrainfo.network_mode.cos_address, data);
		break;
	case SET_SNMP_MODE:
		pSysInfo->extrainfo.network_mode.snmp_mode = atoi(data);
		break;
	case SET_SNMP_TYPE:
		pSysInfo->extrainfo.network_mode.snmp_type = atoi(data);
		break;
	case SET_SNMP_USERNAME:
		memset(pSysInfo->extrainfo.network_mode.snmp_username, 0, sizeof(pSysInfo->extrainfo.network_mode.snmp_username));
		memcpy(pSysInfo->extrainfo.network_mode.snmp_username, data, sizeof(pSysInfo->extrainfo.network_mode.snmp_username)-1);
		//strcpy(&extrainfo.network_mode.snmp_username, data);
		break;
	case SET_SNMP_USERNAME_PRIVACY:
		pSysInfo->extrainfo.network_mode.snmp_username_privacy = atoi(data);
		break;
	case SET_SNMP_AUTH:
		pSysInfo->extrainfo.network_mode.snmp_auth = atoi(data);
		break;
	case SET_SNMP_PRIVACY:
		pSysInfo->extrainfo.network_mode.snmp_privacy = atoi(data);
		break;
	case SET_SNMP_AUTH_PW:
		memset(pSysInfo->extrainfo.network_mode.snmp_auth_pw, 0, sizeof(pSysInfo->extrainfo.network_mode.snmp_auth_pw));
		memcpy(pSysInfo->extrainfo.network_mode.snmp_auth_pw, data, sizeof(pSysInfo->extrainfo.network_mode.snmp_auth_pw)-1);
		//strcpy(&extrainfo.network_mode.snmp_auth_pw, data);
		break;
	case SET_SNMP_PRIVACY_PW:
		memset(pSysInfo->extrainfo.network_mode.snmp_privacy_pw, 0, sizeof(pSysInfo->extrainfo.network_mode.snmp_privacy_pw));
		memcpy(pSysInfo->extrainfo.network_mode.snmp_privacy_pw, data, sizeof(pSysInfo->extrainfo.network_mode.snmp_privacy_pw)-1);
		//strcpy(&extrainfo.network_mode.snmp_privacy_pw, data);
		break;
	case SET_SNMP_READCOMMUNITY:
		memset(pSysInfo->extrainfo.network_mode.snmp_readcommunity, 0, sizeof(pSysInfo->extrainfo.network_mode.snmp_readcommunity));
		memcpy(pSysInfo->extrainfo.network_mode.snmp_readcommunity, data, sizeof(pSysInfo->extrainfo.network_mode.snmp_readcommunity)-1);
		//strcpy(&extrainfo.network_mode.snmp_readcommunity, data);
		break;
	case SET_SNMP_WRITECOMMUNITY:
		memset(pSysInfo->extrainfo.network_mode.snmp_writecommunity, 0, sizeof(pSysInfo->extrainfo.network_mode.snmp_writecommunity));
		memcpy(pSysInfo->extrainfo.network_mode.snmp_writecommunity, data, sizeof(pSysInfo->extrainfo.network_mode.snmp_writecommunity)-1);
		//strcpy(&extrainfo.network_mode.snmp_writecommunity, data);
		break;
	case SET_UPNP_MODE:
		pSysInfo->extrainfo.bonjour_mode.upnp_mode = atoi(data);
		break;
	case SET_UPNP_DISCOVER:
		pSysInfo->extrainfo.bonjour_mode.upnp_discover = atoi(data);
		break;
	case SET_UPNP_TRAVERSAL:
		sscanf(data, "%d", &upnp_port);
		pSysInfo->extrainfo.bonjour_mode.upnp_traversal = upnp_port;//atoi(data);
		//fprintf(stderr, "upnp_traversal = %d\n", extrainfo.bonjour_mode.upnp_traversal);
		break;
	case SET_UPNP_PORTMAPPING:
		pSysInfo->extrainfo.bonjour_mode.upnp_portmapping = atoi(data);
		break;
	case SET_UPNP_AVMODE:
		pSysInfo->extrainfo.bonjour_mode.upnp_avenable = atoi(data);
		break;
	case SET_BONJOUR_MODE:
		pSysInfo->extrainfo.bonjour_mode.bonjour_mode = atoi(data);
		break;
	case SET_FRIENDLY_NAME:
		memset(pSysInfo->extrainfo.bonjour_mode.friendly_name, 0, sizeof(pSysInfo->extrainfo.bonjour_mode.friendly_name));
		memcpy(pSysInfo->extrainfo.bonjour_mode.friendly_name, data, sizeof(pSysInfo->extrainfo.bonjour_mode.friendly_name)-1);
		//strcpy(&extrainfo.bonjour_mode.friendly_name, data);
		break;
	case SET_MAIL_MODE:
		pSysInfo->extrainfo.notification_mode.mail_mode = atoi(data);
		break;
	case SET_MAIL_SENDTO:
		memset(pSysInfo->extrainfo.notification_mode.mail_sendto, 0, sizeof(pSysInfo->extrainfo.notification_mode.mail_sendto));
		memcpy(pSysInfo->extrainfo.notification_mode.mail_sendto, data, sizeof(pSysInfo->extrainfo.notification_mode.mail_sendto)-1);
		//strcpy(&extrainfo.notification_mode.mail_sendto, data);
		break;
	case SET_MAIL_SUBJECT:
		memset(pSysInfo->extrainfo.notification_mode.mail_subject, 0, sizeof(pSysInfo->extrainfo.notification_mode.mail_subject));
		memcpy(pSysInfo->extrainfo.notification_mode.mail_subject, data, sizeof(pSysInfo->extrainfo.notification_mode.mail_subject)-1);
		//strcpy(&extrainfo.notification_mode.mail_subject, data);
		break;
	case SET_TCP_MODE:
		pSysInfo->extrainfo.notification_mode.tcp_mode = atoi(data);
		break;
	case SET_TCP_ADDR:
		memset(pSysInfo->extrainfo.notification_mode.tcp_addr, 0, sizeof(pSysInfo->extrainfo.notification_mode.tcp_addr));
		memcpy(pSysInfo->extrainfo.notification_mode.tcp_addr, data, sizeof(pSysInfo->extrainfo.notification_mode.tcp_addr)-1);
		//strcpy(&extrainfo.notification_mode.tcp_addr, data);
		break;
	case SET_TCP_PORT:
		pSysInfo->extrainfo.notification_mode.tcp_port = atoi(data);
		break;
	case SET_HTTP_MODE:
		pSysInfo->extrainfo.notification_mode.http_mode = atoi(data);
		break;
	case SET_HTTP_URL:
		memset(pSysInfo->extrainfo.notification_mode.http_url, 0, sizeof(pSysInfo->extrainfo.notification_mode.http_url));
		memcpy(pSysInfo->extrainfo.notification_mode.http_url, data, sizeof(pSysInfo->extrainfo.notification_mode.http_url)-1);
		//strcpy(&extrainfo.notification_mode.http_url, data);
		break;
	case SET_HTTP_USERNAME:
		memset(pSysInfo->extrainfo.notification_mode.http_username, 0, sizeof(pSysInfo->extrainfo.notification_mode.http_username));
		memcpy(pSysInfo->extrainfo.notification_mode.http_username, data, sizeof(pSysInfo->extrainfo.notification_mode.http_username)-1);
		//strcpy(&extrainfo.notification_mode.http_username, data);
		break;
	case SET_HTTP_PASSWORD:
		memset(pSysInfo->extrainfo.notification_mode.http_password, 0, sizeof(pSysInfo->extrainfo.notification_mode.http_password));
		memcpy(pSysInfo->extrainfo.notification_mode.http_password, data, sizeof(pSysInfo->extrainfo.notification_mode.http_password)-1);
		//strcpy(&extrainfo.notification_mode.http_password, data);
		break;
	case SET_SIP_MODE:
		pSysInfo->extrainfo.sip_mode.sip_mode = atoi(data);
		break;
	case SET_SIP_PHONENUMBER:
		memset(pSysInfo->extrainfo.sip_mode.sip_phonenumber, 0, sizeof(pSysInfo->extrainfo.sip_mode.sip_phonenumber));
		memcpy(pSysInfo->extrainfo.sip_mode.sip_phonenumber, data, sizeof(pSysInfo->extrainfo.sip_mode.sip_phonenumber)-1);
		//strcpy(&extrainfo.sip_mode.sip_phonenumber, data);
		break;
	case SET_SIP_PORT:
		pSysInfo->extrainfo.sip_mode.sip_port = atoi(data);
		break;
	case SET_SIP_IPADDRESS:
		memset(pSysInfo->extrainfo.sip_mode.sip_ipaddress, 0, sizeof(pSysInfo->extrainfo.sip_mode.sip_ipaddress));
		memcpy(pSysInfo->extrainfo.sip_mode.sip_ipaddress, data, sizeof(pSysInfo->extrainfo.sip_mode.sip_ipaddress)-1);
		//strcpy(&extrainfo.sip_mode.sip_ipaddress, data);
		break;
	case SET_SIP_LOGINID:
		memset(&pSysInfo->extrainfo.sip_mode.sip_loginid, 0, sizeof(pSysInfo->extrainfo.sip_mode.sip_loginid));
		memcpy(&pSysInfo->extrainfo.sip_mode.sip_loginid, data, sizeof(pSysInfo->extrainfo.sip_mode.sip_loginid)-1);
		//strcpy(&extrainfo.sip_mode.sip_loginid, data);
		break;
	case SET_SIP_LOGINPW:
		memset(pSysInfo->extrainfo.sip_mode.sip_loginpw, 0, sizeof(pSysInfo->extrainfo.sip_mode.sip_loginpw));
		memcpy(pSysInfo->extrainfo.sip_mode.sip_loginpw, data, sizeof(pSysInfo->extrainfo.sip_mode.sip_loginpw)-1);
		//strcpy(&extrainfo.sip_mode.sip_loginpw, data);
		break;
	case SET_FTP_EVENTCONFIG:
		pSysInfo->extrainfo.ftpalarm_mode.eventconfig = atoi(data);
		break;
	case SET_FTP_PREALARM:
		pSysInfo->extrainfo.ftpalarm_mode.prealarm = atoi(data);
		break;
	case SET_FTP_POSTALARM:
		pSysInfo->extrainfo.ftpalarm_mode.postalarm = atoi(data);
		break;
	case SET_IPF_IPF_MODE:
		pSysInfo->extrainfo.ipfilter_mode.ipf_mode = atoi(data);
		break;
	case SET_IPF_IP4_MODE:
		pSysInfo->extrainfo.ipfilter_mode.ip4_mode[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number] = (char)atoi(data);
		break;
	case SET_IPF_IP6_MODE:
		pSysInfo->extrainfo.ipfilter_mode.ip6_mode[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number] = (char)atoi(data);
		break;
	case SET_IPF_IP_ADD_NUMBER:
		pSysInfo->extrainfo.ipfilter_mode.ip_number++;
		if (pSysInfo->extrainfo.ipfilter_mode.ip_number >= MAX_IPFILTER_NUMBER)
			pSysInfo->extrainfo.ipfilter_mode.ip_number = MAX_IPFILTER_NUMBER-1;
		break;
	case SET_IPF_IP_DEL_NUMBER:
		if (pSysInfo->extrainfo.ipfilter_mode.ip_number != 0)
			pSysInfo->extrainfo.ipfilter_mode.ip_number--;
		pSysInfo->extrainfo.ipfilter_mode.ip_number = 0;
		break;
	case SET_IPF_IP_TYPE:
		pSysInfo->extrainfo.ipfilter_mode.ip_type[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number] = (char)atoi(data);
		break;
	case SET_IPF_IP_START:
		memset(pSysInfo->extrainfo.ipfilter_mode.ip_start[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number], 0, sizeof(pSysInfo->extrainfo.ipfilter_mode.ip_start[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number]));
		memcpy(pSysInfo->extrainfo.ipfilter_mode.ip_start[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number], data, sizeof(pSysInfo->extrainfo.ipfilter_mode.ip_start[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number])-1);
		//strcpy(&extrainfo.ipfilter_mode.ip_start[extrainfo.ipfilter_mode.ip_number], data);
		break;
	case SET_IPF_IP_END:
		memset(pSysInfo->extrainfo.ipfilter_mode.ip_end[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number], 0, sizeof(pSysInfo->extrainfo.ipfilter_mode.ip_end[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number]));
		memcpy(pSysInfo->extrainfo.ipfilter_mode.ip_end[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number], data, sizeof(pSysInfo->extrainfo.ipfilter_mode.ip_end[(int)pSysInfo->extrainfo.ipfilter_mode.ip_number])-1);
		//strcpy(&extrainfo.ipfilter_mode.ip_end[extrainfo.ipfilter_mode.ip_number], data);
		break;
	case SET_HTTPS_MODE:
		pSysInfo->extrainfo.https_mode.https_mode = atoi(data);
		break;
	case SET_HTTPS_PORT:
		pSysInfo->extrainfo.https_mode.https_port = atoi(data);
		break;
	case SET_HTTP_STATUS:
		pSysInfo->extrainfo.https_mode.http_mode = atoi(data);
		break;
	case SET_MIRRORTYPE:
		pSysInfo->extrainfo.imageseting_mode.mirrortype = atoi(data);
		break;
	case SET_OVERLAYDATE:
		pSysInfo->extrainfo.imageseting_mode.overlaydate = atoi(data);
		break;
	case SET_OVERLAYTEXT:
		pSysInfo->extrainfo.imageseting_mode.overlaytext = atoi(data);
		break;
	case SET_IMAGETEXT:
		memset(pSysInfo->extrainfo.imageseting_mode.imagetext, 0, sizeof(pSysInfo->extrainfo.imageseting_mode.imagetext));
		memcpy(pSysInfo->extrainfo.imageseting_mode.imagetext, data, sizeof(pSysInfo->extrainfo.imageseting_mode.imagetext)-1);
		//strcpy(&extrainfo.imageseting_mode.imagetext, data);
		break;
	case SET_OVERLAYIMAGE:
		pSysInfo->extrainfo.imageseting_mode.overlayimage = atoi(data);
		break;
	case SET_OVERLAYIMAGE_FILE:
		memset(pSysInfo->extrainfo.imageseting_mode.overlayimage_file, 0, sizeof(pSysInfo->extrainfo.imageseting_mode.overlayimage_file));
		memcpy(pSysInfo->extrainfo.imageseting_mode.overlayimage_file, data, sizeof(pSysInfo->extrainfo.imageseting_mode.overlayimage_file)-1);
		//strcpy(&extrainfo.imageseting_mode.overlayimage_file, data);
		break;
	case SET_IPMODE:
		pSysInfo->extrainfo.general_net_mode.ipmode = atoi(data);
		break;
	case SET_DNS2IP:
		memset(&pSysInfo->extrainfo.general_net_mode.dns2ip, 0, sizeof(pSysInfo->extrainfo.general_net_mode.dns2ip));
		memcpy(&pSysInfo->extrainfo.general_net_mode.dns2ip, data, sizeof(pSysInfo->extrainfo.general_net_mode.dns2ip)-1);
		break;
	case SET_RTSP_PORT:
		pSysInfo->extrainfo.general_net_mode.rtspport = atoi(data);
		break;
	case SET_RTP_PORT:
		pSysInfo->extrainfo.general_net_mode.rtpport = atoi(data);
		break;
	case SET_VIDEO_PORT:
		pSysInfo->extrainfo.general_net_mode.videoport = atoi(data);
		break;
	case SET_AUDIO_PORT:
		pSysInfo->extrainfo.general_net_mode.audioport = atoi(data);
		break;
	case SET_TTL:
		pSysInfo->extrainfo.general_net_mode.ttl = atoi(data);
		break;
	case SET_RTP_DATAPORT:
		pSysInfo->extrainfo.general_net_mode.rtpdataport = atoi(data);
		break;
	case SET_GROUP_ADDR:
		memset(pSysInfo->extrainfo.general_net_mode.group_addr, 0, sizeof(pSysInfo->extrainfo.general_net_mode.group_addr));
		memcpy(pSysInfo->extrainfo.general_net_mode.group_addr, data, sizeof(pSysInfo->extrainfo.general_net_mode.group_addr)-1);
		break;
	case SET_MULTICAST_ENABLE:
		pSysInfo->extrainfo.general_net_mode.multicastenable = atoi(data);
		break;
	case SET_PPPOE_ENABLE:
		pSysInfo->extrainfo.general_net_mode.pppoeenable = atoi(data);
		break;
	case SET_PPPOE_ACCOUNT:
		memset(pSysInfo->extrainfo.general_net_mode.pppoeaccount, 0, sizeof(pSysInfo->extrainfo.general_net_mode.pppoeaccount));
		memcpy(pSysInfo->extrainfo.general_net_mode.pppoeaccount, data, sizeof(pSysInfo->extrainfo.general_net_mode.pppoeaccount)-1);
		//strcpy(&extrainfo.general_net_mode.pppoeaccount, data);
		break;
	case SET_PPPOE_PWD:
		memset(pSysInfo->extrainfo.general_net_mode.pppoepwd, 0, sizeof(pSysInfo->extrainfo.general_net_mode.pppoepwd));
		memcpy(pSysInfo->extrainfo.general_net_mode.pppoepwd, data, sizeof(pSysInfo->extrainfo.general_net_mode.pppoepwd));
		//strcpy(&extrainfo.general_net_mode.pppoepwd, data);
		break;
	case SET_DDNS_ENABLE:
		pSysInfo->extrainfo.general_net_mode.ddnsenable = atoi(data);
		break;
	case SET_DDNS_PROVIDER:
		pSysInfo->extrainfo.general_net_mode.ddnsprovider = atoi(data);
		break;
	/* Add DDNS Provider PCI - Begin - Dean - 20120601 */
	case SET_DDNS_DOMAIN:
		pSysInfo->extrainfo.general_net_mode.ddnsdomain = atoi(data);
		break;
	/* Add DDNS Provider PCI - End - Dean - 20120601 */
	case SET_DDNS_HOSTNAME:
		memset(pSysInfo->extrainfo.general_net_mode.ddnshostname, 0, sizeof(pSysInfo->extrainfo.general_net_mode.ddnshostname));
		memcpy(pSysInfo->extrainfo.general_net_mode.ddnshostname, data, sizeof(pSysInfo->extrainfo.general_net_mode.ddnshostname)-1);
		//strcpy(&extrainfo.general_net_mode.ddnshostname, data);
		break;
	case SET_DDNS_ACCOUNT:
		memset(pSysInfo->extrainfo.general_net_mode.ddnsaccount, 0, sizeof(pSysInfo->extrainfo.general_net_mode.ddnsaccount));
		memcpy(pSysInfo->extrainfo.general_net_mode.ddnsaccount, data, sizeof(pSysInfo->extrainfo.general_net_mode.ddnsaccount)-1);
		//strcpy(&pSysInfo->extrainfo.general_net_mode.ddnsaccount, data);
		break;
	case SET_DDNS_PASSWORD:
		memset(pSysInfo->extrainfo.general_net_mode.ddnspassword, 0, sizeof(pSysInfo->extrainfo.general_net_mode.ddnspassword));
		memcpy(pSysInfo->extrainfo.general_net_mode.ddnspassword, data, sizeof(pSysInfo->extrainfo.general_net_mode.ddnspassword)-1);
		//strcpy(&pSysInfo->extrainfo.general_net_mode.ddnspassword, data);
		break;
	// Modify DDNS Provider & Add DDNS Provider Planet - Begin - Dean - 20120411
	case SET_DDNS_UPDATE_PERIOD:
		pSysInfo->extrainfo.ddns_mode.period_time = atoi(data);	// Modify DDNS Provider & Add DDNS Provider Planet - DDNS Update Period - Dean - 20120419
		break;
	// Modify DDNS Provider & Add DDNS Provider Planet - End - Dean - 20120411
	/* Add DDNS Connecting - Begin - Dean - 20120605 */
	case SET_DDNS_CONNECTING:
		pSysInfo->extrainfo.ddns_mode.connecting = atoi(data);
		break;
	/* Add DDNS Connecting - End - Dean - 20120605 */
	/* DIPS_FEATURE - Begin - Dean - 20120530 */
#if DIPS_FEATURE
	case SET_DIPS_ENABLE:
		pSysInfo->extrainfo.dips_info.dips_enable = atoi(data);
		break;
	case SET_DIPS_PROVIDER:
		pSysInfo->extrainfo.dips_info.dips_provider = atoi(data);
		break;
	case SET_DIPS_ID:
		memset(pSysInfo->extrainfo.dips_info.dips_id, 0, sizeof(pSysInfo->extrainfo.dips_info.dips_id));
		memcpy(pSysInfo->extrainfo.dips_info.dips_id, data, sizeof(pSysInfo->extrainfo.dips_info.dips_id)-1);
		//strcpy(&pSysInfo->extrainfo.dips_info.ddnshostname, data);
		break;
	/* Add DIPS Connecting - Begin - Dean - 20120817 */
	case SET_DIPS_CONNECTING:
		pSysInfo->extrainfo.dips_info.dips_connecting= atoi(data);
		break;
	/* Add DIPS Connecting - End - Dean - 20120817 */
#endif
	/* DIPS_FEATURE - End - Dean - 20120530 */
	case SET_RTSP_MULTI:
		memset(rtsp_multi, 0, sizeof(rtsp_multi));
		strcpy(rtsp_multi, data);
		sscanf(rtsp_multi, "%d|%d", &multicast,&rtsp);
		pSysInfo->extrainfo.general_net_mode.rtspport = rtsp;
		pSysInfo->extrainfo.general_net_mode.multicastenable = multicast;
		//fprintf(stderr,"rtsp_multi = %s %d %d\n", rtsp_multi, rtsp, multicast);
		//fprintf(stderr,"pSysInfo->extrainfo.general_net_mode.rtspport = %d\n", pSysInfo->extrainfo.general_net_mode.rtspport);
		//fprintf(stderr,"pSysInfo->extrainfo.general_net_mode.multicastenable = %d\n", pSysInfo->extrainfo.general_net_mode.multicastenable);
		break;
	case SET_EVENT_SAMBA_SERVER_IP:
		memset(pSysInfo->extrainfo.event_server.samba_server_ip, 0, sizeof(pSysInfo->extrainfo.event_server.samba_server_ip));
		memcpy(pSysInfo->extrainfo.event_server.samba_server_ip, data, sizeof(pSysInfo->extrainfo.event_server.samba_server_ip)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.samba_server_ip, data);
		break;
	case SET_EVENT_SAMBA_PATH:
		memset(pSysInfo->extrainfo.event_server.samba_path, 0, sizeof(pSysInfo->extrainfo.event_server.samba_path));
		memcpy(pSysInfo->extrainfo.event_server.samba_path, data, sizeof(pSysInfo->extrainfo.event_server.samba_path)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.samba_path, data);
		break;
	case SET_EVENT_SAMBA_MOUNT_PATH:
		memset(pSysInfo->extrainfo.event_server.samba_mount_path, 0, sizeof(pSysInfo->extrainfo.event_server.samba_mount_path));
		memcpy(pSysInfo->extrainfo.event_server.samba_mount_path, data, sizeof(pSysInfo->extrainfo.event_server.samba_mount_path)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.samba_mount_path, data);
		break;
	case SET_EVENT_SAMBA_USERNAME:
		memset(pSysInfo->extrainfo.event_server.samba_username, 0, sizeof(pSysInfo->extrainfo.event_server.samba_username));
		memcpy(pSysInfo->extrainfo.event_server.samba_username, data, sizeof(pSysInfo->extrainfo.event_server.samba_username)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.samba_username, data);
		break;
	case SET_EVENT_SAMBA_PASSWORD:
		memset(pSysInfo->extrainfo.event_server.samba_password, 0, sizeof(pSysInfo->extrainfo.event_server.samba_password));
		memcpy(pSysInfo->extrainfo.event_server.samba_password, data, sizeof(pSysInfo->extrainfo.event_server.samba_password)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.samba_password, data);
		break;
	case SET_EVENT_HTTP_URL:
		memset(pSysInfo->extrainfo.event_server.http_url, 0, sizeof(pSysInfo->extrainfo.event_server.http_url));
		memcpy(pSysInfo->extrainfo.event_server.http_url, data, sizeof(pSysInfo->extrainfo.event_server.http_url)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.http_url, data);
		break;
	case SET_EVENT_HTTP_USERNAME:
		memset(pSysInfo->extrainfo.event_server.http_username, 0, sizeof(pSysInfo->extrainfo.event_server.http_username));
		memcpy(pSysInfo->extrainfo.event_server.http_username, data, sizeof(pSysInfo->extrainfo.event_server.http_username)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.http_username, data);
		break;
	case SET_EVENT_HTTP_PASSWORD:
		memset(pSysInfo->extrainfo.event_server.http_password, 0, sizeof(pSysInfo->extrainfo.event_server.http_password));
		memcpy(pSysInfo->extrainfo.event_server.http_password, data, sizeof(pSysInfo->extrainfo.event_server.http_password)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.http_password, data);
		break;
	case SET_EVENT_JABBER_SERVER_IM_SERVER:
		memset(pSysInfo->extrainfo.event_server.jabber_im_server, 0, sizeof(pSysInfo->extrainfo.event_server.jabber_im_server));
		memcpy(pSysInfo->extrainfo.event_server.jabber_im_server, data, sizeof(pSysInfo->extrainfo.event_server.jabber_im_server)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.jabber_im_server, data);
		break;
	case SET_EVENT_JABBER_LOGIN_ID:
		memset(pSysInfo->extrainfo.event_server.jabber_login_id, 0, sizeof(pSysInfo->extrainfo.event_server.jabber_login_id));
		memcpy(pSysInfo->extrainfo.event_server.jabber_login_id, data, sizeof(pSysInfo->extrainfo.event_server.jabber_login_id)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.jabber_login_id, data);
		break;
	case SET_EVENT_JABBER_LOGIN_PW:
		memset(pSysInfo->extrainfo.event_server.jabber_login_pw, 0, sizeof(pSysInfo->extrainfo.event_server.jabber_login_pw));
		memcpy(pSysInfo->extrainfo.event_server.jabber_login_pw, data, sizeof(pSysInfo->extrainfo.event_server.jabber_login_pw)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.jabber_login_pw, data);
		break;
	case SET_EVENT_JABBER_SEND_TO:
		memset(pSysInfo->extrainfo.event_server.jabber_send_to, 0, sizeof(pSysInfo->extrainfo.event_server.jabber_send_to));
		memcpy(pSysInfo->extrainfo.event_server.jabber_send_to, data, sizeof(pSysInfo->extrainfo.event_server.jabber_send_to)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.jabber_send_to, data);
		break;
	case SET_EVENT_JABBER_MESSAGE:
		memset(pSysInfo->extrainfo.event_server.jabber_message, 0, sizeof(pSysInfo->extrainfo.event_server.jabber_message));
		memcpy(pSysInfo->extrainfo.event_server.jabber_message, data, sizeof(pSysInfo->extrainfo.event_server.jabber_message)-1);
		//strcpy(&pSysInfo->extrainfo.event_server.jabber_message, data);
		break;
	case SET_EVENT_FTP_ENABLE:
		pSysInfo->extrainfo.event_server.ftp_enable = atoi(data);
		//fprintf(stderr,"SET_EVENT_FTP_ENABLE(%x,%x)\n", pSysInfo->extrainfo.event_server.ftp_enable, atoi(data));
		break;
	case SET_EVENT_SMTP_ENABLE:
		pSysInfo->extrainfo.event_server.smtp_enable = atoi(data);
		//fprintf(stderr,"SET_EVENT_SMTP_ENABLE(%x,%x)\n", pSysInfo->extrainfo.event_server.smtp_enable, atoi(data));
		break;
	case SET_EVENT_SAMBA_ENABLE:
		pSysInfo->extrainfo.event_server.samba_enable = atoi(data);
		//fprintf(stderr,"SET_EVENT_SAMBA_ENABLE(%x,%x)\n", pSysInfo->extrainfo.event_server.samba_enable,atoi(data));
		break;
	case SET_EVENT_JABBER_ENABLE:
		pSysInfo->extrainfo.event_server.jabber_enable = atoi(data);
		//fprintf(stderr,"SET_EVENT_JABBER_ENABLE(%x,%x)\n", pSysInfo->extrainfo.event_server.jabber_enable,atoi(data));
		break;
	case SET_8021_X_MODE:
		pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_enable = atoi(data);
		break;
	case SET_8021_X_MODE_TMP:
		pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_enable_tmp = atoi(data);
		break;
	case SET_8021_X_PROTOCOL:
		pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_protocol = atoi(data);
		break;
	case SET_8021_X_USERNAME:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_username, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_username));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_username, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_username)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_username, data);
		break;
	case SET_8021_X_PASSWORD:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_password, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_password));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_password, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_password)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_password, data);
		break;
	case SET_8021_X_CA_FILE:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_ca_file, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_ca_file));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_ca_file, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_ca_file)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_ca_file, data);
		break;
	case SET_8021_X_CERT_FILE:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_cert_file, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_cert_file));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_cert_file, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_cert_file)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_cert_file, data);
		break;
	case SET_8021_X_PRIVKEY_FILE:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_file, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_file));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_file, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_file)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_file, data);
		break;
	case SET_8021_X_PRIVKEY_PASSWORD:
		memset(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_password, 0, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_password));
		memcpy(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_password, data, sizeof(pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_password)-1);
		//strcpy(&pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_privkey_password, data);
		break;
	case SET_8021_X_TUNNEL_PROTOCOL:
		pSysInfo->extrainfo.sec8021_x_mode.sec8021_x_tunnel_protocol = atoi(data);
		break;
	case SET_ACOUNT_INDEX:
		extro_acount_index = atoi(data);
		if (extro_acount_index < 0)
			extro_acount_index = 0;
		else if (extro_acount_index > ACOUNT_NUM+1)
			extro_acount_index = ACOUNT_NUM+1;
		break;
	case SET_ACOUNT_USER:
		memset(pSysInfo->extrainfo.acounts[extro_acount_index].user, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user));
		memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].user, data, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user)-1);
		//strcpy(&pSysInfo->extrainfo.acounts[extro_acount_index].user, data);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].user = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].user);
		break;
	case SET_ACOUNT_PASSWORD:
		memset(pSysInfo->extrainfo.acounts[extro_acount_index].password, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password));
		memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].password, data, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password)-1);
		//strcpy(&pSysInfo->extrainfo.acounts[extro_acount_index].password, data);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].user = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].user);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].password = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].password);
		break;
	case SET_ACOUNT_AUTHORITY:
		pSysInfo->extrainfo.acounts[extro_acount_index].authority = atoi(data);
		break;
	case SET_ACOUNT_MODIFY:
		memset(acount_buf, 0, sizeof(acount_buf));
		acount_ptr = (char *)acount_buf;
		memcpy(acount_ptr, data, strlen(data));
		if (NULL != (ptr_1 = strstr(acount_ptr, "ID=")))
		{
			int id = 0;
			ptr_1 += 3;
			sscanf(ptr_1, "%d", &id);

			extro_acount_index = id;
			if (extro_acount_index < 0)
				extro_acount_index = 0;
			else if (extro_acount_index > ACOUNT_NUM+1)
				extro_acount_index = ACOUNT_NUM+1;
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "USER=")))
		{
			ptr_1 += 5;
			if (NULL != (ptr_2 = strstr(ptr_1, ":")))
			{
				memset(pSysInfo->extrainfo.acounts[extro_acount_index].user, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user));
				memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].user, ptr_1, ptr_2 - ptr_1);
				pSysInfo->extrainfo.acounts[extro_acount_index].user[sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user)-1] = '\0';
			}
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "PW=")))
		{
			ptr_1 += 3;
			if (NULL != (ptr_2 = strstr(ptr_1, ":")))
			{
				memset(&pSysInfo->extrainfo.acounts[extro_acount_index].password, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password));
				memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].password, ptr_1, ptr_2 - ptr_1);
				pSysInfo->extrainfo.acounts[extro_acount_index].password[sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password)-1] = '\0';
			}
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "AUTH=")))
		{
			int auth = 0;
			ptr_1 += 5;
			sscanf(ptr_1, "%d", &auth);
			pSysInfo->extrainfo.acounts[extro_acount_index].authority = auth;
		}
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].user = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].user);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].password = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].password);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].authority = %d\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].authority);
		break;
	case SET_ACOUNT_MODIFY_CHAR:
		memset(acount_buf, 0, sizeof(acount_buf));
		acount_ptr = (char *)acount_buf;
		memcpy(acount_ptr, data, strlen(data));
		if (NULL != (ptr_1 = strstr(acount_ptr, "ID=")))
		{
			int id = 0;
			ptr_1+=3;
			sscanf(ptr_1, "%d", &id);

			extro_acount_index = id;
			if (extro_acount_index < 0)
				extro_acount_index = 0;
			else if (extro_acount_index > ACOUNT_NUM+1)
				extro_acount_index = ACOUNT_NUM+1;
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "USER=")))
		{
			ptr_1 += 5;
			if (NULL != (ptr_2 = strstr(ptr_1, "cisco")))
			{
				memset(pSysInfo->extrainfo.acounts[extro_acount_index].user, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user));
				memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].user, ptr_1, ptr_2 - ptr_1);
				pSysInfo->extrainfo.acounts[extro_acount_index].user[sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].user)-1] = '\0';
			}
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "PW=")))
		{
			ptr_1 += 3;
			if (NULL != (ptr_2 = strstr(ptr_1, "cisco")))
			{
				memset(pSysInfo->extrainfo.acounts[extro_acount_index].password, 0, sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password));
				memcpy(pSysInfo->extrainfo.acounts[extro_acount_index].password, ptr_1, ptr_2 - ptr_1);
				pSysInfo->extrainfo.acounts[extro_acount_index].password[sizeof(pSysInfo->extrainfo.acounts[extro_acount_index].password)-1] = '\0';
			}
		}
		if (NULL != (ptr_1 = strstr(acount_ptr, "AUTH=")))
		{
			int auth = 0;
			ptr_1 += 5;
			sscanf(ptr_1, "%d", &auth);
			pSysInfo->extrainfo.acounts[extro_acount_index].authority = auth;
		}
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].user = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].user);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].password = %s\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].password);
		//fprintf(stderr, "pSysInfo->extrainfo.acounts[%d].authority = %d\n", extro_acount_index, pSysInfo->extrainfo.acounts[extro_acount_index].authority);
		break;
	case SET_TIME_TYPE:
		pSysInfo->extrainfo.xml_info.time_type = atoi(data);
		break;
	case SET_RESTART_TYPE:
		pSysInfo->extrainfo.xml_info.restart_type = atoi(data);
		break;
	case SET_LOG_NUM:
		pSysInfo->extrainfo.xml_info.log_num = atoi(data);
		break;
	case SET_EVENT_INDEX_ADD:
		//event_sch_index++;
		pSysInfo->extrainfo.event_schedule_number++;
		if (pSysInfo->extrainfo.event_schedule_number >= SCHEDULE_NUM)
			pSysInfo->extrainfo.event_schedule_number = SCHEDULE_NUM - 1;
		event_sch_index = pSysInfo->extrainfo.event_schedule_number;
		break;
	case SET_EVENT_INDEX_DEL:
		//event_sch_index--;
		if (pSysInfo->extrainfo.event_schedule_number > 0)
			pSysInfo->extrainfo.event_schedule_number--;
		event_sch_index = pSysInfo->extrainfo.event_schedule_number;
		break;
	case SET_EVENT_INDEX:
		event_sch_index = atoi(data);
		break;
	case SET_ENABLE_EVENT:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_event = atoi(data);
		//fprintf(stderr, "pSysInfo->extrainfo.event_schedule[%d].enable_event = %d\n", event_sch_index, pSysInfo->extrainfo.event_schedule[event_sch_index].enable_event);
		break;
	case SET_TRIGGER_TIME_GAP:													/* TAMPER_FEATURE - Dean - 20131007 */
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_time_gap = atoi(data);		/* TAMPER_FEATURE - Dean - 20131007 */
		break;																	/* TAMPER_FEATURE - Dean - 20131007 */
	case SET_EVENT_NAME:
		memset(pSysInfo->extrainfo.event_schedule[event_sch_index].event_name, 0, sizeof(pSysInfo->extrainfo.event_schedule[event_sch_index].event_name));
		memcpy(pSysInfo->extrainfo.event_schedule[event_sch_index].event_name, data, sizeof(pSysInfo->extrainfo.event_schedule[event_sch_index].event_name)-1);
		//strcpy(&pSysInfo->extrainfo.event_schedule[event_sch_index].event_name, data);
		//fprintf(stderr, "pSysInfo->extrainfo.event_schedule[%d].event_name = %s\n", event_sch_index, pSysInfo->extrainfo.event_schedule[event_sch_index].event_name);
		break;
	case SET_TRIGGER_MOTION_DETECTION:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_motion_detection = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_IN_MODE1:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_in_mode1 = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_IN_TYPE1:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_in_type1 = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_IN_MODE2:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_in_mode2 = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_IN_TYPE2:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_in_type2 = atoi(data);
		break;
	case SET_TRIGGER_SCH_TIME_MODE:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_schedule_time_mode = atoi(data);
		break;
	case SET_TRIGGER_PERIODICALLY_MODE:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_periodically_mode = atoi(data);
		break;
	case SET_TRIGGER_PERIODICALLY_MIN:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_periodically_min = atoi(data);
		break;
	case SET_SCH_TIME_STATUS:
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.bStatus = atoi(data);
		break;
	case SET_SCH_TIME_DAY:
		memset(pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay, 0, sizeof(pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay));
		memcpy(pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay, data, sizeof(pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay)-1);
		//strcpy(&pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay, data);
		break;
	case SET_SCH_TIME_START:
		//pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tStart = atoi(data);
		sscanf(data, "%02d%02d", &time_hour, &time_min);
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tStart.nHour = time_hour;
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tStart.nMin = time_min;
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tStart.nSec = 0;
		break;
	case SET_SCH_TIME_DURATION:
		//pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tDuration = atoi(data);
		sscanf(data, "%02d%02d", &time_hour, &time_min);
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tDuration.nHour = time_hour;
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tDuration.nMin = time_min;
		pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.tDuration.nSec = 59;
		break;
	case SET_ENABLE_EVENT_FTP:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_ftp = atoi(data);
		break;
	case SET_ENABLE_EVENT_JABBER:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_jabber = atoi(data);
		break;
	case SET_ENABLE_EVENT_EMAIL:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_email = atoi(data);
		break;
	case SET_ENABLE_EVENT_SAMBA:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_samba = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_OUT_MODE:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_out_mode = atoi(data);
		break;
	case SET_TRIGGER_DIGITAL_OUT_SEC:
		pSysInfo->extrainfo.event_schedule[event_sch_index].trigger_digital_out_sec = atoi(data);
		break;
	case SET_EVENT_PRESET_MODE:
		pSysInfo->extrainfo.event_schedule[event_sch_index].event_preset_mode = atoi(data);
		break;
	case SET_EVENT_PRESET_POSITION:
		pSysInfo->extrainfo.event_schedule[event_sch_index].event_preset_position = atoi(data);
		break;
	case SET_ENABLE_EVENT_USB:
		pSysInfo->extrainfo.event_schedule[event_sch_index].enable_usb = atoi(data);
		break;
	case SET_EVENT_CLEAN_DATA:
		memset(&pSysInfo->extrainfo.event_schedule[event_sch_index], 0, sizeof(EVENT_SCHEDULE));
		strcpy((char*)pSysInfo->extrainfo.event_schedule[event_sch_index].schedule_time.nDay, SCH_DAY_DEFAULT);
		break;
	case SET_EVENT_CLEAN_ALL_DATA:
		memset(pSysInfo->extrainfo.event_schedule, 0,  sizeof(EVENT_SCHEDULE) * SCHEDULE_NUM);
		break;
	case SET_EVENT_WRITE_ALL_DATA:
		break;
	case SET_REMOTE_IP4:
		memset(pSysInfo->extrainfo.remote_ip4_addr, 0, sizeof(pSysInfo->extrainfo.remote_ip4_addr));
		memcpy(pSysInfo->extrainfo.remote_ip4_addr, data, sizeof(pSysInfo->extrainfo.remote_ip4_addr)-1);
		//strcpy(&pSysInfo->extrainfo.remote_ip4_addr, data);
		break;
	case SET_REMOTE_IP6:
		memset(pSysInfo->extrainfo.remote_ip6_addr, 0, sizeof(pSysInfo->extrainfo.remote_ip6_addr));
		memcpy(pSysInfo->extrainfo.remote_ip6_addr, data, sizeof(pSysInfo->extrainfo.remote_ip6_addr)-1);
		//strcpy(&pSysInfo->extrainfo.remote_ip6_addr, data);
		break;
	case SET_DATETIME_MODE:
		pSysInfo->extrainfo.date_time_mode = atoi(data);
		break;
	case SET_MD_INDEX:
		md_index = atoi(data);
		if (md_index >= MAX_MD_NUM)
			md_index = MAX_MD_NUM - 1;
		if (md_index < 0)
			md_index = 0;
		break;
	case SET_MD_ENABLE:
		pSysInfo->motion_config.md_data[md_index].motionenable = atoi(data);					/* Dean - 20140325 */
		break;
	case SET_MD_SEN_MODE:
		pSysInfo->motion_config.md_data[md_index].motioncenable = atoi(data);				/* Dean - 20140325 */
		break;
	case SET_MD_SEN_VALUE:
		pSysInfo->motion_config.md_data[md_index].motioncvalue = atoi(data);					/* Dean - 20140325 */
		break;
	case SET_MD_BLK_STR:
		memset(pSysInfo->motion_config.md_data[md_index].motionblock_coordinates, 0, sizeof(pSysInfo->motion_config.md_data[md_index].motionblock_coordinates));				/* Dean - 20140325 */
		memcpy(pSysInfo->motion_config.md_data[md_index].motionblock_coordinates, data, sizeof(pSysInfo->motion_config.md_data[md_index].motionblock_coordinates)-1);			/* Dean - 20140325 */
		//strcpy(&pSysInfo->extrainfo.md_data[md_index].motionblock_coordinates, data);
		break;
	case SET_MD_BLK:
		memset(pSysInfo->motion_config.md_data[md_index].motionblock, 0, sizeof(pSysInfo->motion_config.md_data[md_index].motionblock));										/* Dean - 20140325 */
		memcpy(pSysInfo->motion_config.md_data[md_index].motionblock, data, sizeof(pSysInfo->motion_config.md_data[md_index].motionblock)-1);								/* Dean - 20140325 */
		//strcpy(&pSysInfo->extrainfo.md_data[md_index].motionblock, data);
		break;
	case SET_MD_PERCENTAGE:
		pSysInfo->motion_config.md_data[md_index].percentage	= atoi(data);					/* Dean - 20140325 */
		break;
	case SET_MD_INDICATOR:
		pSysInfo->motion_config.md_data[md_index].indicator = atoi(data);						/* Dean - 20140325 */
		break;
	case SET_DETECT_IP:
		memset(&pSysInfo->extrainfo.detect_ip, 0, sizeof(pSysInfo->extrainfo.detect_ip));
		memcpy(&pSysInfo->extrainfo.detect_ip, data, sizeof(pSysInfo->extrainfo.detect_ip)-1);
		//strcpy(&pSysInfo->extrainfo.detect_ip, data);
		break;
	case SET_STREAM_FORMAT:
		pSysInfo->extrainfo.streamformat = atoi(data);
		break;
	case SET_AUDIO_IN_VOLUME:
		pSysInfo->extrainfo.audio_info.audioinvolume = atoi(data);
		break;
	case SET_MIRCOPHONE_VOLUME:
		pSysInfo->extrainfo.audio_info.microphonevolume = atoi(data);
		break;
	case SET_SPEAKER_VOLUME:
		pSysInfo->extrainfo.audio_info.speakervolume = atoi(data);
		break;
	case SET_AUDIO_GAIN:
		pSysInfo->extrainfo.audio_info.gain = atoi(data);
		break;
	case SET_GLROBAL_PTZ_PATROL:
		pSysInfo->extrainfo.global_ptz_patrol = atoi(data);
		break;
	case SET_WIZARD_FLAG:
		pSysInfo->extrainfo.wizard_flag = atoi(data);
		break;
	case SET_SMTP_AUTH_MODE:
		pSysInfo->extrainfo.smtp_info.auth = atoi(data);
		break;
	case SET_SMTP_SSL_MODE:
		pSysInfo->extrainfo.smtp_info.ssl_mode = atoi(data);
		break;
/*
	case SET_SMTP_PORT:
		pSysInfo->extrainfo.smtp_info.port = atoi(data);
		break;
*/
	case SET_SYSLOG_ENABLE:
		pSysInfo->extrainfo.syslog_info.enable = atoi(data);
		break;
	case SET_SYSLOG_IP:
		memset(pSysInfo->extrainfo.syslog_info.ip, 0, sizeof(pSysInfo->extrainfo.syslog_info.ip));
		memcpy(pSysInfo->extrainfo.syslog_info.ip, data, sizeof(pSysInfo->extrainfo.syslog_info.ip)-1);
		//strcpy(&pSysInfo->extrainfo.syslog_info.ip, data);
		break;
	case SET_SYSLOG_PORT:
		pSysInfo->extrainfo.syslog_info.port = atoi(data);
		break;
	case SET_SAMBA_REC_MODE:
		pSysInfo->samba_record.enable = *data;//atoi(data); /* Samba Record Modification - 20131004 */
		break;
	case SET_SAMBA_REC_IP:
		memset(pSysInfo->samba_record.samba_server_ip, 0, sizeof(pSysInfo->samba_record.samba_server_ip));
		memcpy(pSysInfo->samba_record.samba_server_ip, data, sizeof(pSysInfo->samba_record.samba_server_ip)-1);
		break;
	case SET_SAMBA_REC_PATH:
		memset(pSysInfo->samba_record.samba_path, 0, sizeof(pSysInfo->samba_record.samba_path));
		memcpy(pSysInfo->samba_record.samba_path, data, sizeof(pSysInfo->samba_record.samba_path)-1);
		break;
	case SET_SAMBA_REC_MOUNT_PATH:
		memset(pSysInfo->samba_record.samba_mount_path, 0, sizeof(pSysInfo->samba_record.samba_mount_path));
		memcpy(pSysInfo->samba_record.samba_mount_path, data, sizeof(pSysInfo->samba_record.samba_mount_path)-1);
		break;
	case SET_SAMBA_REC_USER:
		memset(pSysInfo->samba_record.samba_username, 0, sizeof(pSysInfo->samba_record.samba_username));
		memcpy(pSysInfo->samba_record.samba_username, data, sizeof(pSysInfo->samba_record.samba_username)-1);
		break;
	case SET_SAMBA_REC_PW:
		memset(pSysInfo->samba_record.samba_password, 0, sizeof(pSysInfo->samba_record.samba_password));
		memcpy(pSysInfo->samba_record.samba_password, data, sizeof(pSysInfo->samba_record.samba_password)-1);
		break;
	case SET_SAMBA_REC_SIZE:
		pSysInfo->samba_record.max_size = atoi(data);
		break;
	case SET_SAMBA_REC_SPACE:
		pSysInfo->samba_record.samba_space_max = atoi(data);		
		break;
	case SET_SAMBA_REC_TIME_MODE:
		pSysInfo->samba_record.schedule_time.bStatus = atoi(data);
		break;
	case SET_SAMBA_REC_SCH:
		memset(pSysInfo->samba_record.schedule_time.nDay, 0, sizeof(pSysInfo->samba_record.schedule_time.nDay));
		memcpy(pSysInfo->samba_record.schedule_time.nDay, data, sizeof(pSysInfo->samba_record.schedule_time.nDay)-1);
		break;
	case SET_SAMBA_REC_TIME:
		sscanf(data, "%02d%02d", &time_hour, &time_min);
		pSysInfo->samba_record.schedule_time.tStart.nHour = time_hour;
		pSysInfo->samba_record.schedule_time.tStart.nMin = time_min;
		pSysInfo->samba_record.schedule_time.tStart.nSec = 0;
		break;
	case SET_SAMBA_REC_DURATION:
		sscanf(data, "%02d%02d", &time_hour, &time_min);
		pSysInfo->samba_record.schedule_time.tDuration.nHour = time_hour;
		pSysInfo->samba_record.schedule_time.tDuration.nMin = time_min;
		pSysInfo->samba_record.schedule_time.tDuration.nSec = 0;
		if (time_hour == 11 && time_min == 59)
			pSysInfo->samba_record.schedule_time.tDuration.nSec = 59;
		else
			pSysInfo->samba_record.schedule_time.tDuration.nSec = 0;
		break;
	case SET_SAMBA_REC_CLEAN_SCH:
		memset(pSysInfo->samba_record.schedule_time.nDay, 0, sizeof(pSysInfo->samba_record.schedule_time.nDay));
		strcpy((char*)pSysInfo->samba_record.schedule_time.nDay, SCH_DAY_DEFAULT);
		break;
	case SET_SAMBA_REC_TIMESTAMP:
		pSysInfo->samba_record.timestamp = atoi(data);
		break;
	default:
		break;
	}

	//fp = fopen(extrainfo_PATH, "wb");
	//fwrite((void *)&extrainfo, sizeof(extrainfo), 1, fp);
	//fclose(fp);
	return 1;
}
#if 0
int fWriteExtraInfo2(unsigned int field, char *data)
{
	//FILE *fp;
	SysInfo* pSysinfo = GetSysInfo();

	switch (field)
	{
	default:
		break;
	}

	//fp = fopen(extrainfo2_PATH, "wb");
	//fwrite((void *)&extra_info2, sizeof(Extra_Info2), 1, fp);
	//fclose(fp);
	return 1;
}
#endif
int fExtraFileWriteCheck(unsigned int field, char *data)
{
  	int fdlock, re_try = 0;
	fdlock = open("/tmp/.extro_file_lock", O_RDWR | O_CREAT, 0644);

 	while (1)
	{
		if (flock(fdlock, LOCK_EX) < 0)
		  	fprintf(stderr, "Write File Lock Error !!!\n");
		else
		{
		  	//fprintf(stderr, "Write File Lock OK !!!\n");
		  	fWriteExtraInfo(field, data);
		  	system("sync;sync;sync");
RE_FILE_UNLOCK:
			if (flock(fdlock, LOCK_UN) < 0)
			{
			  	//fprintf(stderr, "Write File UnLock Error !!!\n");
				if (re_try++ > 5)
				{
				  	close(fdlock);
				  	unlink("/tmp/.extro_file_lock");
				  	return -1;
				}
			  	goto RE_FILE_UNLOCK;
			}
			else
			{
			  	//fprintf(stderr, "Write File UnLock OK !!!\n");
			  	break;
			}
		}

		if (re_try++ > 5)
		{
		  	close(fdlock);
		  	unlink("/tmp/.extro_file_lock");
		  	return -1;
		}

		sleep(1);
  	}

	close(fdlock);
	unlink("/tmp/.extro_file_lock");
	return 1;
}
#if 0
int fExtraFile2WriteCheck(unsigned int field, char *data)
{
  	int fdlock, re_try = 0;
	fdlock = open("/tmp/.extro_file2_lock", O_RDWR | O_CREAT, 0644);

 	while (1)
	{
		if (flock(fdlock, LOCK_EX) < 0)
		  	fprintf(stderr, "Write File Lock Error !!!\n");
		else
		{
		  	//fprintf(stderr, "Write File Lock OK !!!\n");
		  	fWriteExtraInfo2(field, data);
		  	system("sync;sync;sync");
RE_FILE_UNLOCK:
			if (flock(fdlock, LOCK_UN) < 0)
			{
			  	//fprintf(stderr, "Write File UnLock Error !!!\n");
				if (re_try++ > 5)
				{
				  	close(fdlock);
				  	unlink("/tmp/.extro_file2_lock");
				  	return -1;
				}
			  	goto RE_FILE_UNLOCK;
			}
			else
			{
			  	//fprintf(stderr, "Write File UnLock OK !!!\n");
			  	break;
			}
		}

		if (re_try++ > 5)
		{
		  	close(fdlock);
		  	unlink("/tmp/.extro_file2_lock");
		  	return -1;
		}

		sleep(1);
  	}

	close(fdlock);
	unlink("/tmp/.extro_file2_lock");
	return 1;
}
#endif
void ExtreBufLocalCopy(Extra_Info *buf, int mode)
{
	SysInfo *pSysInfo = GetSysInfo();

	if (mode == 1)
	{
	  	memcpy(buf, (Extra_Info *)&pSysInfo->extrainfo, sizeof(Extra_Info));
		//fprintf(stderr, "1ExtreBufLocalCopy extrainfo.general_net_mode.multicastenable  = %d\n", extrainfo.general_net_mode.multicastenable);
		//fprintf(stderr, "1ExtreBufLocalCopy extrainfo.general_net_mode.rtspport = %d\n", extrainfo.general_net_mode.rtspport);
	}
	else if (mode == 2)
	{
    	memcpy((Extra_Info *)&pSysInfo->extrainfo, buf, sizeof(Extra_Info));
		//fprintf(stderr, "2ExtreBufLocalCopy extra_info.general_net_mode.multicastenable  = %d\n", extra_info.general_net_mode.multicastenable);
		//fprintf(stderr, "2ExtreBufLocalCopy extrainfo.general_net_mode.rtspport = %d\n", extrainfo.general_net_mode.rtspport);
	}
	return;
}

int fResetExtraInfo(void)
{
	char rm_cmd[256] = "";

	sprintf(rm_cmd, "rm %s", EXTRA_INFO_PATH);
	system(rm_cmd);
	system("sync;sync;sync");

	return extra_info_init();
}
#if 0
int fResetExtraInfo2(void)
{
	char rm_cmd[256] = "";

	sprintf(rm_cmd, "rm %s", EXTRA_INFO2_PATH);
	system(rm_cmd);
	system("sync;sync;sync");

	return extra_info2_init();
}
#endif
ThreeGPPInfo *GetThrGPPInfo(void)
{
  	return (ThreeGPPInfo *)&thr_gpp_info;
}

int three_gpp_info_init(void)
{
	FILE *fp;
	char *file_buf, *buf_ptr;
	int  file_length = 0;

  	memset(&thr_gpp_info, 0, sizeof(ThreeGPPInfo));

  	fp = fopen(THREE_GPP_PATH, "rb");
  	if (NULL != fp)
	{
		while (!feof(fp))
		{
			fgetc(fp);
	    	file_length++;
		}
		fseek(fp, 0, SEEK_SET);
    	file_buf = (char *)malloc(sizeof(char) * file_length);

	  	fread(file_buf, 1, file_length, fp);

	    if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_STATUS=")))
		{
	       	buf_ptr += 17;
	       	sscanf(buf_ptr, "%d", &thr_gpp_info.status);
	    }

   		if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_WITHOUT_AUDIO=")))
		{
       		buf_ptr += 24;
       		sscanf(buf_ptr, "%d", &thr_gpp_info.without_audio);
    	}

    	if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_WITH_AUDIO=")))
		{
       		buf_ptr += 21;
       		sscanf(buf_ptr, "%d", &thr_gpp_info.with_audio);
    	}

    	free(file_buf);
    	fclose(fp);

    	return 0;
  	}
  	else
	{
		fp = fopen(THREE_GPP_PATH, "wb");

		fprintf(fp,"There_GPP_STATUS=%d",        thr_gpp_info.status);
		fprintf(fp,"There_GPP_WITHOUT_AUDIO=%d", thr_gpp_info.without_audio);
		fprintf(fp,"There_GPP_WITH_AUDIO=%d",    thr_gpp_info.with_audio);
		fprintf(fp,"END_OF_INFO\n");

		fclose(fp);
		return 1;
  	}
}

int fReadThrGPPInfo(void)
{
	FILE *fp;
	char *file_buf, *buf_ptr;
	int  file_length = 0;

  	memset(&thr_gpp_info, 0, sizeof(ThreeGPPInfo));

  	fp = fopen(THREE_GPP_PATH, "rb");
	if (NULL == fp)
	{
	 	three_gpp_info_init();
	 	return -1;
	}

	while (!feof(fp))
	{
		fgetc(fp);
	    file_length++;
	}
	fseek(fp, 0, SEEK_SET);
  	file_buf = (char *)malloc(sizeof(char) * file_length);

	fread(file_buf, 1, file_length, fp);

	if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_STATUS=")))
	{
	   	buf_ptr += 17;
	   	sscanf(buf_ptr, "%d", &thr_gpp_info.status);
	}

	if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_WITHOUT_AUDIO=")))
	{
	   	buf_ptr += 24;
	   	sscanf(buf_ptr, "%d", &thr_gpp_info.without_audio);
	}

	if (NULL != (buf_ptr = strstr(file_buf, "There_GPP_WITH_AUDIO=")))
	{
	   	buf_ptr += 21;
	   	sscanf(buf_ptr, "%d", &thr_gpp_info.with_audio);
	}

	free(file_buf);
	fclose(fp);

	return 1;
}

int fWriteThrGPPInfo(void)
{
	FILE *fp = fopen(THREE_GPP_PATH, "wb");

	fprintf(fp,"There_GPP_STATUS=%d", thr_gpp_info.status);
	fprintf(fp,"There_GPP_WITHOUT_AUDIO=%d", thr_gpp_info.without_audio);
	fprintf(fp,"There_GPP_WITH_AUDIO=%d", thr_gpp_info.with_audio);
	fprintf(fp,"END_OF_INFO\n");

	fclose(fp);
	return 1;
}

int fResetThrGppInfo(void)
{
	char rm_cmd[256] = "";

	sprintf(rm_cmd, "rm %s", THREE_GPP_PATH);
	system(rm_cmd);
	system("sync;sync;sync");

	return three_gpp_info_init();
}

/**
* @brief Set System Init Flag in file manager
* @param flag [c] Flag.
* @retval 0 Success.
* @retval -1 Fail
*/
int fSetSWWatchDogEnable(char flag)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.swwatchdogenable, &flag, sizeof(flag));
	return SetSysInfo(0);
}

int user_info_init(void)
{
//	FILE *fp;
//	char *file_buf, *buf_ptr;
//	int file_length = 0;
	static int run_status = 0;

	if (!run_status)
	{
		run_status = 1;
	  	CurrConnectNum = 0;
	  	memset(user_info, 0, sizeof(UserInfo) * 16);
	  	memset(user_arth_info, 0, sizeof(UserArthInfo) * 16);
	}
#if 1   // modify by kenny chiu per frank.lin
	Set_MaxUserLogin  = 15;   //Maximim user login
	Set_ServerityUser = 9;	  //9 for AUTHORITY_NONE
	return 1;
#else
	fp = fopen(LIMIT_USER_NUM_PATH, "rb");
	if (NULL != fp)
	{
		while (!feof(fp))
		{
			fgetc(fp);
		    file_length++;
		}
		fseek(fp, 0, SEEK_SET);
	    file_buf = (char *)malloc(sizeof(char) * file_length);

	    fread(file_buf, 1, file_length, fp);
	    if (NULL != (buf_ptr = strstr(file_buf, "Set_MaxUserLogin=")))
         	sscanf(buf_ptr+17, "%d", &Set_MaxUserLogin);

	    if (NULL != (buf_ptr = strstr(file_buf, "Set_ServerityUser=")))
         	sscanf(buf_ptr+18, "%d", &Set_ServerityUser);
      	Set_MaxUserLogin  = 15;   //Maximim user login
      	Set_ServerityUser = 9;    //9 for AUTHORITY_NONE

	    free(file_buf);
      	fclose(fp);

      	return 0;
	}
	else
	{
	  	fp = fopen(LIMIT_USER_NUM_PATH, "wb");

      	Set_MaxUserLogin  = 15;   //Maximim user login
      	Set_ServerityUser = 9;    //9 for AUTHORITY_NONE

      	fprintf(fp,"Set_MaxUserLogin=%d", Set_MaxUserLogin);
      	fprintf(fp,"Set_ServerityUser=%d", Set_ServerityUser);
      	fprintf(fp,"END_OF_INFO");

     	fclose(fp);

    	return 1;
	}

	return -1;
#endif
}

int SetMJPGStatus(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;
	pSysInfo->lan_config.mjpgdisable = value;
	fprintf(stderr, "SetMJPGStatus mjpgdisable = %d\n", pSysInfo->lan_config.mjpgdisable);
	return SetSysInfo(0);
}

/* SD Card Mount - Dean - 20121213 - Begin */
long long Diskfreespace[DISK_MEDIA_TOTAL];
struct statfs DiskStatfs[DISK_MEDIA_TOTAL];
long long GetDiskfreeSpace(DISK_MEDIA dMedia)
{
	char DiskPath[DISK_MEDIA_STR_LEN] = "";
	char DiskPathList[DISK_MEDIA_TOTAL][DISK_MEDIA_STR_LEN] = DISK_MEDIA_STR_PATH_LIST;			/* Record Media - Dean - 20130530 */

	sprintf(DiskPath,"%s", DiskPathList[dMedia]);												/* Record Media - Dean - 20130530 */

	if (statfs(DiskPath, &DiskStatfs[dMedia]) >= 0)
		Diskfreespace[dMedia] = (((long long)DiskStatfs[dMedia].f_bsize * (long long)DiskStatfs[dMedia].f_bavail)/(long long)1024);		/* AVI Record Msg - Dean - 20130530 */
	else
		Diskfreespace[dMedia] = 0;

	return Diskfreespace[dMedia];
}

/* Record Media - Dean - 20130530 - Begin */
long long Disktotalspace[DISK_MEDIA_TOTAL];
long long GetDiskTotalSpace(DISK_MEDIA dMedia)
{
	char DiskPath[DISK_MEDIA_STR_LEN] = "";
	char DiskPathList[DISK_MEDIA_TOTAL][DISK_MEDIA_STR_LEN] = DISK_MEDIA_STR_PATH_LIST;

	sprintf(DiskPath,"%s", DiskPathList[dMedia]);

	if (statfs(DiskPath, &DiskStatfs[dMedia]) >= 0)
		Disktotalspace[dMedia] = (((long long)DiskStatfs[dMedia].f_bsize * (long long)DiskStatfs[dMedia].f_blocks)/(long long)1024);
	else
		Disktotalspace[dMedia] = 0;

	return Disktotalspace[dMedia];
}
/* Record Media - Dean - 20130530 - End */

/* DB File Maintain - Dean - 20130530 - Begin */
int GetfreememSize( void )
{
	int fd_mem = -1;
	int ret = -1;
	char StrBuffer[200] = "";
	char TmpBuf[50] = "";
	char *pStr = NULL;
	int freeInKb = 0;
	char delima_buf[] = ":\r\n ";

	fd_mem = open("/proc/meminfo", O_RDONLY);
	if (fd_mem < 0)
	{
		//FD_ERR("meminfo open file fail \n");
		ret = -1;
		goto CHECK_MEM_END;
	}

	ret = read(fd_mem, StrBuffer, sizeof(StrBuffer)-1);
	if (ret <= 0)
	{
		//FD_ERR("read device error !!");
		ret = -1;
		goto CHECK_MEM_END;
	}

	ret = -1;
	StrBuffer[sizeof(StrBuffer)-1] = '\0';
	pStr = strtok(StrBuffer, delima_buf );

	while (pStr != NULL)
	{
		sscanf(pStr, "%s", TmpBuf);
		if (strncmp(TmpBuf, "MemFree", sizeof("MemFree")) == 0 )
		{
			pStr = strtok(NULL, delima_buf);
			//fprintf(stderr, " %s \n", pStr);
			sscanf(pStr, "%d", &freeInKb);
			//Free_MEM = (long)freeInKb;
			if (freeInKb < 2000/*1600*/)
			{
				fprintf(stderr, "Free Memory = %ld KByte is not enough \n", (long)freeInKb);
				ret = -1;
			}
			else
			{
				ret = freeInKb;
			}
			goto CHECK_MEM_END;
		}
		pStr = strtok(NULL, delima_buf);
	}

CHECK_MEM_END:

	if( fd_mem >= 0 )
		close(fd_mem);

	return ret;
}
/* DB File Maintain - Dean - 20130530 - End */

/* Dean - 20140317 - Begin */
//#if REC_TO_USB /* Record Media - Dean - 20130530 */// [dean huang videorec] - Modify device type for videorec - - 20111014
long long RESERVE_SPACE	= 512*1024;   //reserve RESERVE_SPACE Gbytes for write tail,RS
//int	CheckDiskFreeSapce(const char *path, int type) /* SD Card Mount - Dean - 20121213 *///20091014,RS, Space Check
int iCheckResult[DISK_MEDIA_TOTAL] = {0};
int CheckDiskFreeSapce(DISK_MEDIA dMedia, unsigned int iReserverSpace, int _log_flag) /* Record Media - Dean - 20130530 *//* SD Card Mount - Dean - 20121213 */
{
	long long freespace = 0;	/* SDCard Space Check - Dean - 20120501 */
	char MediaList[DISK_MEDIA_TOTAL][DISK_MEDIA_STR_LEN] = DISK_MEDIA_STR_LIST;			/* Record Media - Dean - 20130530 */
	int RtnValue;			/* Record Media - Dean - 20130530 */
	LogEntry_t tLog;
	time_t tCurrentTime;
	struct tm *tmnow;

	/* SD Card Mount - Dean - 20121213 - Begin */
	freespace = GetDiskfreeSpace(dMedia);
	char strSpace[25] ="";							
	sprintf(strSpace,"Space=%ld KB", freespace);	
//    fprintf(stderr,"[%s][%d] freespace = %ld, iReserverSpace = %d\n", __func__,__LINE__,freespace,iReserverSpace);
	
	time(&tCurrentTime);
	tmnow = localtime(&tCurrentTime);
	memcpy(&tLog.time, tmnow, sizeof(struct tm));
	/* Record Media - Dean - 20130530 - Begin */
	if (freespace < 0)
	{
		sprintf(tLog.event, "[Recording]:%s Fail to Get Space", MediaList[dMedia]);
		RtnValue = RECORD_CHECK_DISK_SPACE_FAIL;//-3;/* Samba - Check Space - Dean - 20130530 */// [dean huang videorec] - Modify Return Value - 20111020
	}
	else if ( freespace == 0 )	// [dean huang videorec] - Add Case : freespace ==0 - 20111020
	{
		sprintf(tLog.event, "[Recording]:%s is Empty Space", MediaList[dMedia]);
		RtnValue = RECORD_CHECK_DISK_SPACE_EMPTY;//-2;
	}
	else if( freespace < iReserverSpace )
	{
		sprintf(tLog.event, "[Recording]:%s %s is not enough", MediaList[dMedia], strSpace);
		RtnValue = RECORD_CHECK_DISK_SPACE_NOT_ENOUGH;//-1;
	}
	else
	{
		sprintf(tLog.event, "[Recording]:%s %s is enough", MediaList[dMedia], strSpace);
		RtnValue = RECORD_CHECK_DISK_SPACE_ENOUGH;//1;
	}
	// if (iCheckResult[dMedia] != RtnValue)
	if(_log_flag == 1 || (iCheckResult[dMedia] != RtnValue)) fSetSysLog(&tLog);
	
	iCheckResult[dMedia] = RtnValue;

	return RtnValue;
	/* Record Media - Dean - 20130530 - End */
}
//#endif			/* Record Media - Dean - 20130530 */

void CheckSambaRootPath(char *path, char *root_path)
{
	char *path_ptr_1, *path_ptr_2;
	if (strlen(path) == 0)
		return;

	path_ptr_1 = path;
	if (NULL != (path_ptr_2 = (strstr(path, "/"))))
		memcpy(root_path, path_ptr_1, path_ptr_2 - path_ptr_1);
	else
		strcpy(root_path, path);
}

int MountSambaRec(void)
 {
	SysInfo *pSysInfo = GetSysInfo();
	Extra_Info *get_extra_info_ptr = GetExtraInfo();
	FILE *fp = NULL;
	char cmd[256] = "", root_path[256] = "";
	int ret = 0;

	CheckSambaRootPath(pSysInfo->samba_record.samba_path, &root_path[0]);				/* Samba - Record - Dean - 20130530 */
#if REC_CIFS_TYPE		//20100203,RS
	sprintf(cmd, "mount //%s/%s %s -t cifs -o username=%s,password=%s"
				,pSysInfo->samba_record.samba_server_ip
				,root_path//,get_extra_info_ptr->event_server.samba_path
				,SAMBA_REC_DIR
				,pSysInfo->samba_record.samba_username
				,pSysInfo->samba_record.samba_password);
#else
	sprintf(cmd, "mount //%s/%s %s -o username=%s,password=%s"
				,pSysInfo->samba_record.samba_server_ip
				,root_path//,get_extra_info_ptr->event_server.samba_path
				,SAMBA_REC_DIR
				,pSysInfo->samba_record.samba_username
				,pSysInfo->samba_record.samba_password);

#endif
	ret = umount2(SAMBA_REC_DIR, MNT_FORCE);

	mkdir(SAMBA_REC_DIR, 0777);

	unlink("/tmp/samba_rec.sh");
	unlink(SAMBA_REC_SUCCESS);
	unlink(SAMBA_REC_FAIL);
	fp = fopen("/tmp/samba_rec.sh", "wb");
	if (fp == NULL)
		return -1;

	fprintf(fp, "if %s ; then\n", cmd);
	//fprintf(fp, "echo \"Mount Samba Success\"\n");
	fprintf(fp, "touch %s\n", SAMBA_REC_SUCCESS);
	fprintf(fp,	"mkdir -p %s\n",pSysInfo->samba_record.samba_mount_path);
	fprintf(fp, "else\n");
	//fprintf(fp, "echo \"Mount Samba Fail\"\n");
	fprintf(fp, "touch %s\n", SAMBA_REC_FAIL);
	fprintf(fp, "fi\n");
	fclose(fp);

	chmod("/tmp/samba_rec.sh", 0777);

	//memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "/bin/sh /tmp/samba_rec.sh >/dev/null 2>&1");
	system(cmd);

	/* Samba - Record - Dean - 20130530 - Begin */
//	if (FileCheck(SAMBA_REC_FOLDER) != FILE_ACTION_SUCCESS)
//	if (FileCheck(pSysInfo->samba_record.samba_mount_path) != FILE_ACTION_SUCCESS)
//	{
//		fprintf(stderr, "MountSambaRec - mkdir - SAMBA_REC_FOLDER\n");
//		mkdir(pSysInfo->samba_record.samba_mount_path, 0777);
//	}
	/* Samba - Record - Dean - 20130530 - End */

	fp = fopen(SAMBA_REC_SUCCESS, "rb");
	if (fp != NULL)
	{
		fclose(fp);
		return 0;
	}
	return -1;
}

int UnMountSambaRec(void)
 {
	int ret = -1;

	ret = umount2(SAMBA_REC_DIR, MNT_FORCE);//umount(SAMBA_REC_DIR);
	if (ret != 0)
		return -1;

	return 0;
}

int usb_mount_flag = 0;
int MountUSB(void)
{
	FILE *procpt = NULL;
	char line[100] = "", ptname[12] = "";
	int ma, mi, sz, ma_tmp, mi_tmp, sz_tmp;
	int det = -1, ret = -1;

	if (usb_mount_flag == 1)
		return 0;

	umount2(USB_REC_DIR, MNT_FORCE);
	rmdir(USB_REC_DIR);
	unlink("/tmp/recsda1");
	unlink("/tmp/recsdb1");

	if (mkdir(USB_REC_DIR, 0755) < 0)
	{
		usb_mount_flag = 0;
		return -1;
	}

	if ((procpt = fopen("/proc/partitions", "r")) == NULL)
	{
		usb_mount_flag = 0;
		return ret;
	}
	while (fgets(line, sizeof(line), procpt))
	{
		if (sscanf(line, " %d %d %d %[^\n ]", &ma, &mi, &sz, ptname) != 4)
			continue;

		if (strstr(ptname, "sda") != NULL)
		{
			ma_tmp = ma;
			mi_tmp = mi;
			sz_tmp = sz;
			if (fgets(line, sizeof(line), procpt))
			{
				if (sscanf(line, " %d %d %d %[^\n ]", &ma, &mi, &sz, ptname) == 4)
				{
					if (strstr(ptname, "sda1") != NULL)
					{
						if (mknod("/tmp/recsda1", S_IFBLK|0777, makedev(ma, mi)))
						{
							usb_mount_flag = 0;
							return -1;
						}
						det = 0;
						break;
					}
				}
			}
 			if (mknod("/tmp/recsda1", S_IFBLK|0777, makedev(ma_tmp, mi_tmp)))
			{
				usb_mount_flag = -1;
				return -1;
			}
	    	det = 0;
	    	break;
	    }
	    else if (strstr(ptname, "sdb") != NULL)
		{
			ma_tmp = ma;
			mi_tmp = mi;
			sz_tmp = sz;
			if (fgets(line, sizeof(line), procpt))
			{
				if (sscanf(line, " %d %d %d %[^\n ]", &ma, &mi, &sz, ptname) == 4)
				{
					if (strstr(ptname, "sdb1") != NULL)
					{
						if (mknod("/tmp/recsdb1", S_IFBLK|0777, makedev(ma, mi)))
						{
							usb_mount_flag = -1;
							return -1;
						}
						det = 1;
						break;
					}
				}
			}
			if (mknod("/tmp/recsdb1", S_IFBLK|0777, makedev(ma_tmp, mi_tmp)))
			{
				usb_mount_flag = -1;
				return -1;
			}
	    	det = 1;
	    	break;
		}
	}
	fclose(procpt);

	if (det == 0)
	{
		if (!mount("/tmp/recsda1", USB_REC_DIR, "vfat", MS_SYNCHRONOUS, NULL))
		{
			fprintf(stderr, "MountUSB Success !!!!!...\n");
			usb_mount_flag = 1;
			return 0;
		}
		else
		{
			fprintf(stderr, "MountUSB Fail !!!!!...\n");
			UmountUSB();
			return 1;
		}
	}
	else if (det == 1)
	{
		if (!mount("/tmp/recsdb1", USB_REC_DIR, "vfat", MS_SYNCHRONOUS, NULL))
		{
			fprintf(stderr, "MountUSB Success !!!!!...\n");
			usb_mount_flag = 1;
			return 0;
		}
		else
		{
			fprintf(stderr, "MountUSB Fail !!!!!...\n");
			UmountUSB();
			return 1;
		}
	}
	else
	{
		usb_mount_flag = -1;
		return -1;
	}
}

void UmountUSB(void)
{
	umount2(USB_REC_DIR, MNT_FORCE);
	rmdir(USB_REC_DIR);//system("rm -rf "USB_REC_DIR);
	unlink("/tmp/recsda1");
	unlink("/tmp/recsdb1");
	unlink("/tmp/recusbsd");
	usb_mount_flag = -1;
}
/* Dean - 20140317 - End */

int SDCard_Insert(void)
{
	//fprintf(stderr, "CheckSDInsert %d !!\n", value);
	/* SD Card Check - Dean - 20131127 - Begin */
#if 0
	int ret = dm355_gio_read(GIO_SD_DETECT) ? 0 : 1;
	return ((ret) ? SDCARD_MSG_NUM_CARD_EXISTS : SDCARD_MSG_NUM_NO_CARD);
#else
	int RtnValue;
	int mmc_path_id, mmc_major_id, mmc_minor_id, mmc_size_blocks;
	int ret = SDCARD_MSG_NUM_NO_CARD;

	RtnValue = SDCard_SearchPartitionTable(&mmc_path_id, &mmc_major_id, &mmc_minor_id, &mmc_size_blocks);

	if ((RtnValue == SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS)
		|| (RtnValue == SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS))
		ret =  SDCARD_MSG_NUM_CARD_EXISTS;

	return ret;
#endif
	/* SD Card Check - Dean - 20131127 - End */
}

int SDCard_Protect(void)
{
	/* 20140225 */
	int ret = 0;//dm355_gio_read(GIO_SD_WRITE_PROTECT);

	return ((ret) ? SDCARD_MSG_NUM_CARD_PROTECT : SDCARD_MSG_NUM_CARD_WRITABLE);
}

int SDCard_SaveFlag(int iFlag)
{
	char DataPath[256] = {0};
	int  RtnValue = 0;
	FILE* pezfile;
	struct stat st;

	if (stat(MMC_STATUS_FLAG_BUSY, &st) == 0)
		return RtnValue;

	sprintf(DataPath, "%s", MMC_STATUS_FLAG_BUSY);
	pezfile = fopen(DataPath, "wb");
	fclose(pezfile);

	sprintf(DataPath, "%s", MMC_STATUS_FLAG);

	pezfile = fopen(DataPath, "wb");
	if (pezfile == NULL)
	{
		RtnValue = 0;
	}
	else
	{
		RtnValue = 1;
		fwrite(&iFlag, sizeof(int), 1, pezfile);
		fclose(pezfile);
	}

	unlink(MMC_STATUS_FLAG_BUSY);

	return RtnValue;
}

int SDCard_DoMount(char bSaveFlag)
{
	int RtnValue;
	int mmc_path_id, mmc_major_id, mmc_minor_id, mmc_size_blocks;

	RtnValue = SDCard_SearchPartitionTable(&mmc_path_id, &mmc_major_id, &mmc_minor_id, &mmc_size_blocks);
	if ((RtnValue != SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS)
		&& (RtnValue != SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS))
	{
		if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
			SDCard_SaveFlag(RtnValue);
		SDCard_UnMount(SDCARD_UMOUNT_GENEARL);

		return SDCARD_ACTION_ERROR;
	}

	int RetryCount = 0;
	while (RetryCount++ < 3)//30)// modifty by Kenny Chiu 20151106
	{
		RtnValue = SDCard_Mount(mmc_path_id, mmc_major_id, mmc_minor_id, mmc_size_blocks);
		if (RtnValue == SDCARD_MSG_NUM_MOUNT_SUCCESS)
			break;
		fprintf(stderr, "SDCard_Mount fail !!!!!... RetryCount = %d\n", RetryCount);
		sleep(1);
	}

	if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
		SDCard_SaveFlag(RtnValue);

	/* DB Search - Dean - 20140217 - Begin */
	if (RtnValue == SDCARD_MSG_NUM_MOUNT_SUCCESS)
	{
		char strDataPathSearch[256] = {0};

		sprintf(strDataPathSearch, "%s/%s", DB_FILE_SEARCH_PATH, DB_FILE_SEARCH_SDCARD_RECORD);
		if (FileCheck(strDataPathSearch) != FILE_ACTION_SUCCESS)
			DB_FileMaintain(DB_MEDIA_SDCARD_RECORD, DB_MAINTAIN_DATA_ACTION_SEARCH);
		sprintf(strDataPathSearch, "%s/%s", DB_FILE_SEARCH_PATH, DB_FILE_SEARCH_SDCARD_EVENT);
		if (FileCheck(strDataPathSearch) != FILE_ACTION_SUCCESS)
			DB_FileMaintain(DB_MEDIA_SDCARD_EVENT, DB_MAINTAIN_DATA_ACTION_SEARCH);

		return SDCARD_ACTION_SUCCESS;
	}
	/* DB Search - Dean - 20140217 - End */

	return SDCARD_ACTION_RETRY;
}

int SDCard_CheckPartitionTable(char bSaveFlag)
{
	int RtnValue;
	int mmc_path_id, mmc_major_id, mmc_minor_id, mmc_size_blocks;

	RtnValue = SDCard_SearchPartitionTable(&mmc_path_id, &mmc_major_id, &mmc_minor_id, &mmc_size_blocks);

	if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
		SDCard_SaveFlag(RtnValue);

	if ((RtnValue == SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS)
		|| (RtnValue == SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS))
		RtnValue = SDCARD_ACTION_SUCCESS;
	else
		RtnValue = SDCARD_ACTION_ERROR;

	return RtnValue;
}

int SDCard_SearchPartitionTable(int* ppath_id, int* pmajor_id, int* pminor_id, int* pszblocks)
{
	FILE *procpt;
	char line[100] = "", ptname[12] = "";
	int major_id, minor_id, szblocks;
	int ret = SDCARD_MSG_NUM_NO_CARD;//SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_FAIL;

	int loopi = 0;
	int loopMax = MMC_PATH_ID_TOTAL/2;
	char strPath[MMC_PATH_ID_TOTAL][MMC_PATH_LIST_LEN] = MMC_PATH_LIST;
	int iMsgNum[MMC_PATH_ID_TOTAL] = MMC_MSG_NUM_LIST;

//       fprintf(stderr,"%s[%d] starting ...\n",__func__,__LINE__);
	/* SD Card Check - Dean - 20131127 - Begin */
//	for (loopi = 0; loopi < MMC_PATH_ID_TOTAL; loopi++) {  fprintf(stderr,"[%d]%s\n",__LINE__,strPath[loopi]); }
#if 0
	if ( SDCard_Insert() == SDCARD_MSG_NUM_NO_CARD)
		return SDCARD_MSG_NUM_NO_CARD;
#endif
	/* SD Card Check - Dean - 20131127 - End */

	procpt = fopen("/proc/partitions", "r");
	if (procpt == NULL)
		return SDCARD_MSG_NUM_MOUNT_FAIL_OPEN_PARTITION_TABLE;

	while (fgets(line, sizeof(line), procpt))
	{
//		fprintf(stderr,"[%d]%s\n",__LINE__,line);
		if (sscanf(line, " %d %d %d %[^\n ]", &major_id, &minor_id, &szblocks, ptname) != 4 )
			continue;

		*pmajor_id = major_id;
		*pminor_id = minor_id;
		*pszblocks = szblocks;
#if 0
		    fprintf(stderr,"[%d]%s\n",__LINE__,ptname);
// 		    fprintf(stderr,"[%d]%s\n",__LINE__,strPath[loopi*2]);
            ret = SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS;
#else
		for (loopi = 0; loopi < loopMax; loopi++)
		{
// 		    fprintf(stderr,"[%d]%s\n",__LINE__,line);
//		    fprintf(stderr,"[%d]%s %d\n",__LINE__,ptname, strlen(ptname));
// 		    fprintf(stderr,"[%d]%s %d\n",__LINE__,strPath[loopi*2], strlen(strPath[loopi*2]));
			if (strstr(ptname, strPath[loopi*2]) != NULL)
			{
				if (fgets(line, sizeof(line), procpt))
				{
// 				       fprintf(stderr,"[%d]%s\n",__LINE__,line);
					if (sscanf(line, " %d %d %d %[^\n ]", &major_id, &minor_id, &szblocks, ptname) == 4 )
					{
						if (strstr(ptname, strPath[loopi*2+1]) != NULL)
						{
							ret = iMsgNum[loopi*2+1];
							*pmajor_id = major_id;
							*pminor_id = minor_id;
							*pszblocks = szblocks;
							*ppath_id = loopi*2+1;
							break;
						}
					}
				}
				*ppath_id = loopi*2;
				ret = iMsgNum[*ppath_id];
				break;
			}
		}
	    if (ret != SDCARD_MSG_NUM_NO_CARD) break;
#endif						
	}

	fclose(procpt);
//    fprintf(stderr,"SDPartitionTable[%d]  - ptname = %s, major_id = %d, minor_id = %d, szblocks = %d\n", __LINE__,ptname, major_id, minor_id, szblocks);
//	fprintf(stderr,"SDPartitionTable[%d]  - ret = %d, ptname = %s, *pmajor_id = %d, *pminor_id = %d, *pszblocks = %d\n", __LINE__,ret, ptname, *pmajor_id, *pminor_id, *pszblocks);

	return ret;
}

int SDCard_CheckMountStatus(void)
{
	struct stat st;

	/* Check if MMC Card Already Mounted */
	if ((stat(MMC_SUCCESS_FLAG_0, &st) == 0)
		|| (stat(MMC_SUCCESS_FLAG_1, &st) == 0))
		return SDCARD_MSG_NUM_STATUS_MOUNT;

	return SDCARD_MSG_NUM_STATUS_UMOUNT;
}

// [dean huang videorec] - modify return value - 20111014 - begin
int SDCard_Mount(int path_id, int major_id, int minor_id, int szblocks)
{
	char strNodPath[MMC_PATH_ID_TOTAL][MMC_NODE_PATH_LIST_LEN] = MMC_NODE_PATH_LIST;
	int NodeErrCode[MMC_PATH_ID_TOTAL] = MMC_NODE_ERROR_CODE_LIST;
	struct stat st;

	/* Check if MMC Card Busy*/
	if (stat(MMC_BUSY_FLAG, &st) == 0)
		return SDCARD_MSG_NUM_CARD_BUSY;

	/* Touch MMC Card Busy Flag */
	//sprintf(strFilename,"%s", MMC_BUSY_FLAG);
	if (FileTouch(MMC_BUSY_FLAG) < 0)
		return SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_FAIL;

	/* Check if MMC Card Already Mounted */
	if (SDCard_CheckMountStatus() == SDCARD_MSG_NUM_STATUS_MOUNT)
	{
		unlink(MMC_BUSY_FLAG);
		return SDCARD_MSG_NUM_MOUNT_SUCCESS;
	}

	/* Check Path ID */
	if (path_id >= MMC_PATH_ID_TOTAL)
	{
		unlink(MMC_BUSY_FLAG);
		return SDCARD_MSG_NUM_WRONG_PATH_ID;
	}

	/* Make MMC Working Directory*/
	if (stat(MMC_DIR, &st) != 0)
	{
		if (mkdir(MMC_DIR, 0755) < 0)
		{
			fprintf(stderr, "SDCard_Mount Make working directory fail !!!!!...\n");
			unlink(MMC_BUSY_FLAG);
			return SDCARD_MSG_NUM_MOUNT_FAIL_MAKE_REC_DIR;
	  	}
	}

	/* Make MMC Node*/
	if (stat(strNodPath[path_id], &st) != 0)
	{
		if (mknod(strNodPath[path_id], S_IFBLK|0777, makedev(major_id, minor_id)))
		{
			fprintf(stderr, "SDCard_Mount Make node fail !!!!!...\n");
			unlink(MMC_BUSY_FLAG);
			return NodeErrCode[path_id];
		}
	}

	/* Mount MMC */
	if (!mount(strNodPath[path_id], MMC_DIR, "vfat", 32768, NULL) == NULL)
	{
		/* Mount Fail */
		fprintf(stderr, "<%d>SDCard_Mount Fail - strNodPath[%d] = %s!!!!!...\n", __LINE__, path_id, strNodPath[path_id]);
		SDCard_UnMount(SDCARD_UMOUNT_GENEARL);
		unlink(MMC_BUSY_FLAG);
		return SDCARD_MSG_NUM_MOUNT_FAIL;
	}

	/* Make Event Working Directory */
	if (stat(MMC_EVENT_DIR, &st) != 0)
	{
		if (mkdir(MMC_EVENT_DIR, 0755) < 0)
		{
			fprintf(stderr, "SDCard_Mount mkdir event Faill\n");
			unlink(MMC_BUSY_FLAG);
			return SDCARD_MSG_NUM_MAKE_DIR_EVENT_FAIL;
		}
	}

	/* Make Recording Working Directory */
	if (stat(MMC_RECORD_DIR, &st) != 0)
	{
		if (mkdir(MMC_RECORD_DIR, 0755) < 0)
		{
			fprintf(stderr, "SDCard_Mount mkdir backup Faill\n");
			system("ls -al /tmp");
			unlink(MMC_BUSY_FLAG);
			return SDCARD_MSG_NUM_MAKE_DIR_RECORD_FAIL;
		}
	}

	if (FileTouch(((path_id == MMC_PATH_ID_MMCBLK0) || (path_id == MMC_PATH_ID_MMCBLK0P1)) ? MMC_SUCCESS_FLAG_0 : MMC_SUCCESS_FLAG_1) < 0)
	{
		fprintf(stderr, "SDCard_Mount toch success flag Faill\n");
		unlink(MMC_BUSY_FLAG);
		return SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_FAIL;
	}

	/* Remove MMC Card Busy Flag */
	unlink(MMC_BUSY_FLAG);

	/* Mount Procedure Success*/
	fprintf(stderr, "SDCard_Mount Success !!!!!...\n");
	return SDCARD_MSG_NUM_MOUNT_SUCCESS;
}
// [dean huang videorec] - modify return value - 20111014 - end

int SDCard_UnMount(int bIsFormat)
{
	struct stat st;
	int RtnValue = SDCARD_MSG_NUM_UMOUNT_SUCCESS;									/* SD Card UnMount - Dean - 20130530 */

	/* Check if MMC Card Busy*/
	if (stat(MMC_BUSY_FLAG, &st) == 0)
		return SDCARD_MSG_NUM_CARD_BUSY;

	/* Touch MMC Card Busy Flag */
	//sprintf(strFilename,"%s", MMC_BUSY_FLAG);
	if (FileTouch(MMC_BUSY_FLAG) < 0)
		return SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_FAIL;

	if (SDCard_CheckMountStatus() != SDCARD_MSG_NUM_STATUS_MOUNT)
	{
		unlink(MMC_BUSY_FLAG);
		return SDCARD_MSG_NUM_NONE;
	}

	if (umount2(MMC_DIR, MNT_FORCE) < 0)
	{
		char strCmd[256] = "";
		snprintf(strCmd, sizeof(strCmd),"umount -l %s", MMC_DIR);

//		SendSystemCallCmd(0, SYSTEM_CALL_EVENT_RUN, strCmd, strlen(strCmd));	/* SD Card UnMount - Dean - 20130530 */

		RtnValue = SDCARD_MSG_NUM_UMOUNT_FAIL;									/* SD Card UnMount - Dean - 20130530 */
	}
	if (!bIsFormat)
	{
		if (stat(MMC_DIR, &st) == 0)
			rmdir(MMC_DIR);//system("rm -rf "MMC_DIR);
	   	unlink("/tmp/mmcblk0");
		unlink("/tmp/mmcblk1");
	}

	/* DB Search - Dean - 20140214 - Begin */
	char strDataPathSearch[256] = {0};
	sprintf(strDataPathSearch, "%s/%s", DB_FILE_SEARCH_PATH, DB_FILE_SEARCH_SDCARD_RECORD);
	unlink(strDataPathSearch);
	sprintf(strDataPathSearch, "%s/%s", DB_FILE_SEARCH_PATH, DB_FILE_SEARCH_SDCARD_EVENT);
	unlink(strDataPathSearch);
	/* DB Search - Dean - 20140214 - End */

	unlink(MMC_BACKUP_SUCCESS);
	unlink(MMC_BACKUP_ERROR);
	unlink(MMC_RESTORE_SUCCESS);
	unlink(MMC_RESTORE_ERROR);
	unlink(MMC_SUCCESS_FLAG_0);
	unlink(MMC_SUCCESS_FLAG_1);
	unlink(MMC_ERROR_FLAG);
	unlink(MMC_BUSY_FLAG);
	//mmc_mount_flag = -1;

	return RtnValue;			/* SD Card UnMount - Dean - 20130530 */
}
/* SD Card Format - Dean - 20130207 - Begin */
int SDCard_Format(void)
{
	int rtnValue = SDCARD_MSG_NUM_FORMAT_SUCCESS;

	if (FileCheck(MMC_SUCCESS_FLAG_0) == FILE_ACTION_SUCCESS)
	{
		if (system("/sbin/mkdosfs -F 32 /tmp/mmcblk0\n") < 0)
			rtnValue = SDCARD_MSG_NUM_FORMAT_FAIL;
	}
	else if (FileCheck(MMC_SUCCESS_FLAG_1) == FILE_ACTION_SUCCESS)
	{
		if (system("/sbin/mkdosfs -F 32 /tmp/mmcblk1\n") < 0)
			rtnValue = SDCARD_MSG_NUM_FORMAT_FAIL;
	}
	else
		rtnValue = SDCARD_MSG_NUM_FORMAT_FAIL;

	return rtnValue;
}

int SDCard_DoFormat(char bSaveFlag)
{
	int rtnValue;

	rtnValue = SDCard_UnMount(SDCARD_UMOUNT_FORMAT);
	if (rtnValue != SDCARD_MSG_NUM_UMOUNT_SUCCESS)
	{
		if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
			SDCard_SaveFlag(rtnValue);
		return SDCARD_ACTION_ERROR;
	}

	rtnValue = SDCard_Format();
	if (rtnValue != SDCARD_MSG_NUM_UMOUNT_SUCCESS)
	{
		if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
			SDCard_SaveFlag(rtnValue);
		return SDCARD_ACTION_ERROR;
	}

	if (bSaveFlag == SDCARD_MSG_OP_SAVE_ON)
		SDCard_SaveFlag(rtnValue);

	return SDCARD_ACTION_SUCCESS;
}
/* SD Card Format - Dean - 20130207 - End */

int SDCard_CheckAVIFileBusy(char* dirname, char* Filename)
{
	char cFullnameTmp[256] = {0};

	sprintf(cFullnameTmp, "%s/%s", dirname, Filename);

	char* strPtr;
	strPtr = cFullnameTmp + strlen(cFullnameTmp) - 3/*strlen("avi")*/;
	sprintf(strPtr, "%s", "tmp");

	struct stat sb;

	if (stat(cFullnameTmp, &sb) == 0)
		return SDCARD_MSG_NUM_DELETE_FILE_BUSY;

	return SDCARD_MSG_NUM_NONE;
}
/* SD Card Mount - End - Dean - 20121213*/

/* DB File Maintain - Dean - 20121219 - Begin *//*20121228 - Move to system_default.h */
//#define FILE_ACTION_ERROR		-1				/* Record Media - Dean - 20130530 */
//#define FILE_ACTION_SUCCESS	1				/* Record Media - Dean - 20130530 */
//#define FILE_ACTION_FAIL		0				/* Record Media - Dean - 20130530 */
/* DB File Maintain - Dean - 20121219 - End *//*20121228 - Move to system_default.h */

/* DB - Begin - Dean - 20121213*/
/* DB File Maintain - Dean - 20121219 - Begin *//*20121228 - Move to system_default.h */
//#define FILE_ACTION_ERROR		-1
//#define FILE_ACTION_SUCCESS	1
//#define FILE_ACTION_FAIL		0
/* DB File Maintain - Dean - 20121219 - End *//*20121228 - Move to system_default.h */

/* DB File Maintain - Dean - 20121226 - Begin */
//#define DB_CHECK_MAIN_FILE	1				/* Duplicate DB File for Search - Dean - 20130613 */
//#define DB_CHECK_BACKUP_FILE	2			/* Duplicate DB File for Search - Dean - 20130613 */

#define DB_FILE_LEN	16

char strDataMac[6] = {0};
char strDataBaseMain[DB_FILE_LEN] = {0};
char strDataBaseBackup[DB_FILE_LEN] = {0};
char strDataBaseSearch[DB_FILE_LEN] = {0};	/* DB Search - Dean - 20140214 */

int IsSetMac = 0;
int DB_SetMac(void)
{
	if (IsSetMac)
		return DB_ACTION_SUCCESS;

	/* DB MAC - 20131009  - Begin */
	if (FileCheck("/tmp/mac.con") == FILE_ACTION_SUCCESS)
	{
		char mac[64] = {0};

		FileRead("/tmp/mac.con", mac, sizeof(mac));

		sprintf(strDataMac, "%c%c%c%c", mac[12], mac[13], mac[15], mac[16]);
		//fprintf(stderr,"DB_SetMac 1- mac = %s\n", mac);
		//fprintf(stderr,"DB_SetMac 1- strDataMac = %s\n", strDataMac);
	}
	else
	{
		//fExtraFileReadCheck();
		SysInfo *pSysInfo = GetSysInfo();
		FileSave("/tmp/mac.con", pSysInfo->extrainfo.mac, strlen(pSysInfo->extrainfo.mac));		/* DB MAC - Dean - 20131014 */

		sprintf(strDataMac, "%c%c%c%c", pSysInfo->extrainfo.mac[12], pSysInfo->extrainfo.mac[13], pSysInfo->extrainfo.mac[15], pSysInfo->extrainfo.mac[16]);
		//fprintf(stderr,"DB_SetMac 2- strDataMac = %s\n", strDataMac);
	}
	/* DB MAC - 20131009  - End */

	IsSetMac = 1;

	return DB_ACTION_SUCCESS;
}

int DB_GetDatabaseFileName(DB_MEDIA dbMedia, int idbCheckFile, char* strDataBase)
{
	int RtnValue = DB_ACTION_SUCCESS;
	char strDataFilename[DB_CHECK_FILE_TOTAL][DB_CHECK_FILE_NAME_LEN] = DB_CHECK_FILE_NAME_LIST;		/* DB Search - Dean - 20140214 */

//	DB_SetMac();					/* DB File - Dean - 20130530 */
	//fprintf(stderr, "DB_GetDatabaseFileName\n");
	/* DB Search - Dean - 20140214 - Begin */
#if 0
	if (idbCheckFile == DB_CHECK_MAIN_FILE)
		sprintf(strDataBase, ".db%s_file", strDataMac);
	else
		sprintf(strDataBase, ".db%s_backup", strDataMac);
#else
	if (idbCheckFile == DB_CHECK_SEARCH_FILE)
	{
		switch(dbMedia)
		{
			case DB_MEDIA_SDCARD_EVENT:
				sprintf(strDataBase, DB_FILE_SEARCH_SDCARD_EVENT);
				break;
			case DB_MEDIA_SDCARD_RECORD:
				sprintf(strDataBase, DB_FILE_SEARCH_SDCARD_RECORD);
				break;
			case DB_MEDIA_SAMBA_EVENT:
				sprintf(strDataBase, DB_FILE_SEARCH_SAMBA_EVENT);
				break;
			case DB_MEDIA_SAMBA_RECORD:
				sprintf(strDataBase, DB_FILE_SEARCH_SAMBA_RECORD);
				break;
			case DB_MEDIA_SEARCH_EVENT:
				sprintf(strDataBase, DB_FILE_SEARCH_SDCARD_EVENT);
				break;
			case DB_MEDIA_SEARCH_RECORD:
				sprintf(strDataBase, DB_FILE_SEARCH_SDCARD_RECORD);
				break;
			default:
				sprintf(strDataBase, ".db_file");
				break;
		}
	}
	else
		sprintf(strDataBase, ".db%s_%s", strDataMac, strDataFilename[idbCheckFile]);
#endif
	/* DB Search - Dean - 20140214 - End */

	return RtnValue;
}
/* DB File - Dean - 20130530 - End */

int DB_FileCheck(DB_MEDIA dbMedia, int idbCheckFile)
{
	char DataPath[256] = {0};
	struct stat st;
	int bIsDataBaseExist = DB_ACTION_ERROR;

	/* DB File - Dean - 20130530 - Begin */
	if (idbCheckFile == DB_CHECK_MAIN_FILE)
	{
		DB_GetDatabaseFileName(dbMedia, idbCheckFile, strDataBaseMain);
		//sprintf(DataPath, "%s/%s", DBDir[dbMedia], strDataBaseMain);	/* Samba Record Modification - 20131004 */
		DB_GenerateFullname(dbMedia, DataPath, strDataBaseMain); 		/* Samba Record Modification - 20131004 */
		//sprintf(DataPath, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_FILENAME);
		//fprintf("DB_FileCheck - DataPath = %s\n", DataPath);
	}
	else
	{
		DB_GetDatabaseFileName(dbMedia, idbCheckFile, strDataBaseBackup);
		//sprintf(DataPath, "%s/%s", DBDir[dbMedia], strDataBaseBackup);/* Samba Record Modification - 20131004 */
		DB_GenerateFullname(dbMedia, DataPath, strDataBaseBackup); 		/* Samba Record Modification - 20131004 */
		//sprintf(DataPath, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_BACKUP_FILENAME);
	}
	/* DB File - Dean - 20130530 - End */

	if (stat(DataPath, &st) == 0)
		bIsDataBaseExist = DB_ACTION_SUCCESS;

	return bIsDataBaseExist;
}
/* DB File Maintain - Dean - 20121226 - End */

int DB_FileUnlink(DB_MEDIA dbMedia)
{
	char DataPath[256] = {0};
	int RtnValue = DB_ACTION_SUCCESS;
	//fprintf(stderr, "DB_FileOpen\n");
	/* DB File - Dean - 20130530 - Begin */
	DB_GetDatabaseFileName(dbMedia, DB_CHECK_MAIN_FILE, strDataBaseMain);
	//sprintf(DataPath, "%s/%s", DBDir[dbMedia], strDataBaseMain);	/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, DataPath, strDataBaseMain);		/* Samba Record Modification - 20131004 */
	//sprintf(DataPath, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_FILENAME);
	/* DB File - Dean - 20130530 - End */

	//fprintf(stderr,"DB_FileUnlink - DataPath = %s\n", DataPath);

	unlink(DataPath);

	return RtnValue;
}

int DB_FileOpen(FILE** ppFile, DB_MEDIA dbMedia, int iAction)
{
	char DataPath[256] = {0};
	int RtnValue = DB_ACTION_SUCCESS;

	/* DB File - Dean - 20130530 - Begin */
	/* DB Search - Dean - 20140214 - Begin */
	/* Duplicate DB File for Search - Dean - 20130613 - Begin */
	if ((dbMedia == DB_MEDIA_SEARCH_RECORD)
		|| (dbMedia == DB_MEDIA_SEARCH_EVENT))
	{
		/* DB Search - Dean - 20140214 - Begin */
#if 1
		DB_GetDatabaseFileName(dbMedia, DB_CHECK_SEARCH_FILE, strDataBaseSearch);
		sprintf(DataPath, "%s/%s", DB_FILE_SEARCH_PATH, strDataBaseSearch);
		if (FileCheck(DataPath) == FILE_ACTION_ERROR)
		{
			if (DB_FileMaintain(dbMedia, DB_MAINTAIN_DATA_ACTION_SEARCH) == DB_ACTION_ERROR)
			{
				FileTouch("/var/run/sdcard_db_search_fail");
				return DB_ACTION_FAIL;
			}
		}
		FileTouch("/var/run/sdcard_db_search");
#else
		DB_GetDatabaseFileName(dbMedia, DB_CHECK_MAIN_FILE, strDataBaseMain);
		sprintf(DataPath, "%s/%s%s", TEMP_FOLDER, strDataBaseMain, (dbMedia == DB_MEDIA_SEARCH_RECORD) ? "Record" : "Event");
#endif
		/* DB Search - Dean - 20140214 - End */
	}
	else
	{
		DB_GetDatabaseFileName(dbMedia, DB_CHECK_MAIN_FILE, strDataBaseMain);
		//sprintf(DataPath, "%s/%s", DBDir[dbMedia], strDataBaseMain);	/* Samba Record Modification - 20131004 */
		DB_GenerateFullname(dbMedia, DataPath, strDataBaseMain);		/* Samba Record Modification - 20131004 */
	}
	/* DB Search - Dean - 20140214 - End */
	/* Duplicate DB File for Search - Dean - 20130613 - End */
	//sprintf(DataPath, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_FILENAME);
	/* DB File - Dean - 20130530 - End */

//	fprintf(stderr,"DB_FileOpen - iAction= %d - DataPath = %s\n", iAction, DataPath);

	*ppFile = NULL;
	if (iAction == DB_FILE_READ)
		*ppFile = fopen(DataPath, DB_STR_DATABASE_READ_ATTR);
	else if (iAction == DB_FILE_WRITE)
		*ppFile = fopen(DataPath, DB_STR_DATABASE_WRITE_ATTR);
	if (*ppFile == NULL)
	{
		fprintf(stderr,"DB_FileOpen - ppFile == NULL\n");
		RtnValue = DB_ACTION_FAIL;
	}
//	else
//		fprintf(stderr,"DB_FileOpen - Success\n");

	return RtnValue;
}

/* DB Search - Dean - 20140214 - Begin */
int DB_SearchFileOpen(FILE** ppFile, DB_MEDIA dbMedia, int iAction)
{
	char DataPath[256] = {0};
	int RtnValue = DB_ACTION_SUCCESS;

	fprintf(stderr, "DB_SearchFileOpen - 1\n");

	DB_GetDatabaseFileName(dbMedia, DB_CHECK_SEARCH_FILE, strDataBaseSearch);
	sprintf(DataPath, "%s/%s", DB_FILE_SEARCH_PATH, strDataBaseSearch);

	if (DB_CheckSearchAvailable(dbMedia) == DB_ACTION_BUSY)
		return DB_ACTION_ERROR;

	DB_DoSearchFlagTouch(dbMedia);
	FileUnlink(DataPath);													/* DB File - Dean - 20130530 */

	*ppFile = NULL;
	if (iAction == DB_FILE_READ)
		*ppFile = fopen(DataPath, DB_STR_DATABASE_READ_ATTR);
	else if (iAction == DB_FILE_WRITE)
		*ppFile = fopen(DataPath, DB_STR_DATABASE_WRITE_ATTR);
	if (*ppFile == NULL)
	{
		fprintf(stderr,"DB_FileOpen - ppFile == NULL\n");
		RtnValue = DB_ACTION_FAIL;
	}

	return RtnValue;
}
/* DB Search - Dean - 20140214 - End */

int DB_FileClose(FILE* pfile)
{
//	int RtnValue = DB_ACTION_SUCCESS;
	fclose(pfile);

	return DB_ACTION_SUCCESS;
}

/* DB Maintain - Dean - 20130110 - Begin */
int DB_FileRead(FILE* pfile, DB_MEDIA dbMedia, int iAction, long lStarTime, long lEndTime)
{
	int loopi;//, nRead;
//	int num, record;
	int RtnValue = DB_ACTION_SUCCESS;
//	char tmp[DB_FILE_INFO_TEMP_LEN + 1] = "";
	int iFlagReadLen = 0;

	//fprintf(stderr,"DB_FileRead - dbMedia = %d, iAction=%d\n", dbMedia, iAction);
	if ((iAction & DB_READ_DATA_ACTION_NUMBER)					/* Record Media - Dean - 20130530 */
		|| (iAction & DB_READ_DATA_ACTION_SPACE))				/* Record Media - Dean - 20130530 */
	{
		if (fread(&DBList[dbMedia].head, sizeof(DBListHead), 1, pfile))
		{
			//fprintf(stderr,"DB_FileRead - sizeof(DBListHead) = %d\n", sizeof(DBListHead));
			//fprintf(stderr,"DB_FileRead - head.total = %d, head.record = %d\n", DBList[dbMedia].head.total, DBList[dbMedia].head.record);
			//fprintf(stderr,"DB_FileRead - DBFile[dbMedia].head.\n", DBList[dbMedia].head.record);
		}
		else
		{
			DBList[dbMedia].head.total = 0;				/*DB Header - Dean - 20130117*/
			DBList[dbMedia].head.totalsize = 0;			/* Record Media - Dean - 20130530 */
			DBList[dbMedia].head.record = 0xFFFF;		/*DB Header - Dean - 20130117*/
			return DB_ACTION_ERROR;						/* DB File Maintain - Dean - 20121219 */
		}

		if (iAction & DB_READ_DATA_ACTION_NUMBER)			/* Record Media - Dean - 20130530 */
			RtnValue = DBList[dbMedia].head.total;			/* Record Media - Dean - 20130530 *//*DB Header - Dean - 20130117*/
		else if (iAction & DB_READ_DATA_ACTION_SPACE)		/* Record Media - Dean - 20130530 */
			RtnValue = DBList[dbMedia].head.totalsize;		/* Record Media - Dean - 20130530 *//*DB Header - Dean - 20130117*/

		iFlagReadLen = 1;
	}

	//fprintf(stderr,"DB_FileRead - DBFile[dbMedia].total=%d\n", DBList[dbMedia].total);/*DB Header - Dean - 20130117*/
	int loopMax = DBList[dbMedia].head.total;						/*DB Header - Dean - 20130117*/
	long lFTime;
	if ((iFlagReadLen == 0) && (DBList[dbMedia].head.total  != 0))	/*DB Header - Dean - 20130117*/
		fseek(pfile, sizeof(DBListHead), SEEK_SET);					/*DB Header - Dean - 20130117*/

	if (iAction & DB_READ_DATA_ACTION_DATA)
	{
		int iRead = fread(&DBList[dbMedia].InfoList, DB_FILE_INFO_DATA_LEN, DBList[dbMedia].head.total, pfile);
		//printf("DB_FileRead - 1 - iRead=%d\n", iRead); /*DB Header - Dean - 20130117*/
		loopMax = iRead;
		DBList[dbMedia].head.total = 0;								/*DB Header - Dean - 20130117*/
		for(loopi = 0; loopi < loopMax; loopi++)
		{
			sscanf(&DBList[dbMedia].InfoList[loopi].time, "%10d", &lFTime);	/*DB Header - Dean - 20130117*/
			//printf("(%d)(%ld<%ld<%ld)%s\n", loopi, lStarTime, lFTime, lEndTime, DBList[dbMedia].InfoList[loopi].time);
			DBList[dbMedia].KeepList[loopi] = DB_KEEP_LIST_OP_REMOVE;	/*DB Header - Dean - 20130117*/
			if ((lFTime >= lStarTime) && (lFTime <= lEndTime))
			{
				DBList[dbMedia].KeepList[DBList[dbMedia].head.total] = DB_KEEP_LIST_OP_ADD;	/*DB Header - Dean - 20130117*/
				if (DBList[dbMedia].head.total < loopi)
					memcpy(&DBList[dbMedia].InfoList[DBList[dbMedia].head.total], &DBList[dbMedia].InfoList[loopi], DB_FILE_INFO_DATA_LEN);
				DBList[dbMedia].head.total = DBList[dbMedia].head.total + 1;	/*DB Header - Dean - 20130117*/
			}
		}
		RtnValue = DBList[dbMedia].head.total;							/*DB Header - Dean - 20130117*/
	}

	if (iAction & DB_READ_DATA_ACTION_DATA_ALL)
	{
		int iRead = fread(&DBList[dbMedia].InfoList, DB_FILE_INFO_DATA_LEN, DBList[dbMedia].head.total, pfile);
		for (loopi=0; loopi< iRead; loopi++)
			DBList[dbMedia].KeepList[loopi] = DB_KEEP_LIST_OP_ADD;
		//fprintf(stderr,"DB_FileRead - iRead=%d\n", iRead); /*DB Header - Dean - 20130117*/
		if (iRead != DBList[dbMedia].head.total)
			return DB_ACTION_ERROR;

		RtnValue = DBList[dbMedia].head.total;				/*DB Header - Dean - 20130117*/
	}

	return RtnValue;
}
/* DB Maintain - Dean - 20130110 - End */

/* DB File Maintain - Dean - 20121219 - Begin */
/* DB Search - Dean - 20140214 - Begin */
#define DB_FLAG_SEARCH_SDCARD_RECORD_BUSY "/var/run/db_flag_search_sdcard_record_busy"
#define DB_FLAG_SEARCH_SDCARD_EVENT_BUSY "/var/run/db_flag_search_sdcard_event_busy"
#define DB_FLAG_SEARCH_SAMBA_RECORD_BUSY "/var/run/db_flag_search_samba_record_busy"
#define DB_FLAG_SEARCH_SAMBA_EVENT_BUSY "/var/run/db_flag_search_samba_event_busy"
int DBCountSearch[DB_MEDIA_TOTAL] = {0};

int DB_GetSearchFlagName(DB_MEDIA dbMedia, char *strFlag)
{
	if (dbMedia >= DB_MEDIA_TOTAL)
		return DB_ACTION_ERROR;

	switch(dbMedia)
	{
		case DB_MEDIA_SDCARD_EVENT:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SDCARD_EVENT_BUSY);
			break;
		case DB_MEDIA_SDCARD_RECORD:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SDCARD_RECORD_BUSY);
			break;
		case DB_MEDIA_SAMBA_EVENT:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SAMBA_EVENT_BUSY);
			break;
		case DB_MEDIA_SAMBA_RECORD:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SAMBA_RECORD_BUSY);
			break;
		case DB_MEDIA_SEARCH_EVENT:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SDCARD_EVENT_BUSY);
			break;
		case DB_MEDIA_SEARCH_RECORD:
			sprintf(strFlag,"%s", DB_FLAG_SEARCH_SDCARD_RECORD_BUSY);
			break;
		default:
			break;
	}

	return FILE_ACTION_SUCCESS;
}

int DB_DoSearchFlagTouch(DB_MEDIA dbMedia)
{
	int RtnValue = FILE_ACTION_ERROR;
	char strFlag[64] = {0};

	if (dbMedia >= DB_MEDIA_TOTAL)
		return DB_ACTION_BUSY;

	DB_GetSearchFlagName(dbMedia, strFlag);

	RtnValue = FileTouch(strFlag);

	return RtnValue;
}

int DB_DoSearchFlagUnlink(DB_MEDIA dbMedia)
{
	int RtnValue = FILE_ACTION_ERROR;
	char strFlag[64] = {0};

	if (dbMedia >= DB_MEDIA_TOTAL)
		return DB_ACTION_BUSY;

	DB_GetSearchFlagName(dbMedia, strFlag);

	RtnValue = FileUnlink(strFlag);

	return RtnValue;
}


int DB_DoSearchFlagCheck(DB_MEDIA dbMedia)
{
	int RtnValue = FILE_ACTION_ERROR;
	char strFlag[64] = {0};

	if (dbMedia >= DB_MEDIA_TOTAL)
		return DB_ACTION_BUSY;

	DB_GetSearchFlagName(dbMedia, strFlag);

	RtnValue = FileCheck(strFlag);

	return RtnValue;
}


int DB_CheckSearchAvailable(DB_MEDIA dbMedia)
{
///	char strFlag[64] = {0};

	if (dbMedia >= DB_MEDIA_TOTAL)
		return DB_ACTION_BUSY;

	if (DBSearchCount[dbMedia])
		return DB_ACTION_BUSY;

	DBSearchCount[dbMedia] = 0;
	while (DB_DoSearchFlagCheck(dbMedia) == FILE_ACTION_SUCCESS)
	{
		DBSearchCount[dbMedia] ++;
		if (DBSearchCount[dbMedia] > DB_RETRY_COUNT_MAX)
		{
			DBSearchCount[dbMedia] = 0;
			return DB_ACTION_BUSY;
		}
		usleep(DB_RETRY_COUNT_LOOP_TIME);
	}
	DBSearchCount[dbMedia] = 0;

	return DB_ACTION_SUCCESS;
}
/* DB Search - Dean - 20140214 - End */

int DB_DoRead(DB_MEDIA dbMedia, DB_READ_DATA_ACTION dbReadAct, long lStartTime, long lEndTime)	/* DB Maintain - Dean - 20130110 */
{
	FILE*	pfile;
	int iTotalFileNum = 0;
	int RtnValue;

	RtnValue = DB_FileOpen(&pfile, dbMedia, DB_FILE_READ);

	//printf("DB_DoRead - pfile=%p|", pfile);
	if (RtnValue == DB_ACTION_SUCCESS)
	{
		iTotalFileNum = DB_FileRead(pfile, dbMedia, dbReadAct, lStartTime, lEndTime);	/* DB Maintain - Dean - 20130110 */
		DB_FileClose(pfile);
	}
	else		/* DB Search - Dean - 20140214 - Begin */
	{
		DB_InitList(dbMedia);
		return DB_ACTION_ERROR;
	}		/* DB Search - Dean - 20140214 - End */

	return iTotalFileNum;
}

int DB_DoList(DB_MEDIA dbMedia, DB_READ_DATA_ACTION dbReadAct, long lStartTime, long lEndTime)	/* DB Maintain - Dean - 20130110 */
{
	FILE*	pfile;
	int iTotalFileNum = 0;
	int RtnValue;

	/* DB Search - Dean - 20140214 - Begin */
	if (DB_CheckSearchAvailable(dbMedia) == DB_ACTION_BUSY)
		return iTotalFileNum;

	DB_DoSearchFlagTouch(dbMedia);
	/* DB Search - Dean - 20140214 - End */

	RtnValue = DB_FileOpen(&pfile, dbMedia, DB_FILE_READ);

	FileCheck(DB_FILE_SEARCH_PATH);

	//printf("DB_DoRead - pfile=%p|", pfile);
	if (RtnValue == DB_ACTION_SUCCESS)
	{
		iTotalFileNum = DB_FileRead(pfile, dbMedia, dbReadAct, lStartTime, lEndTime);	/* DB Maintain - Dean - 20130110 */
		DB_FileClose(pfile);
	}
	else		/* DB Search - Dean - 20140214 - Begin */
	{
		DB_InitList(dbMedia);
		DB_DoSearchFlagUnlink(dbMedia);
		return DB_ACTION_ERROR;
	}		/* DB Search - Dean - 20140214 - End */

	DB_DoSearchFlagUnlink(dbMedia);			/* DB Search - Dean - 20140214 */


	return iTotalFileNum;
}

int DB_FileMaintain(DB_MEDIA dbMedia, DB_MAINTAIN_DATA_ACTION dbMaintain)
{
//	FILE* fpSrc, fpDest;
	char strDataPath[256] = "";
	char strDataPathBackup[256] = "";
	char strDataPathSearch[256] = "";		/* DB Search - Dean - 20140214 */
	int RtnValue;

	/* DB File - Dean - 20130530 - Begin */
	DB_GetDatabaseFileName(dbMedia, DB_CHECK_MAIN_FILE, strDataBaseMain);
	//sprintf(strDataPath, "%s/%s", DBDir[dbMedia], strDataBaseMain);			/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, strDataPath, strDataBaseMain);					/* Samba Record Modification - 20131004 */
	//sprintf(strDataPath, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_FILENAME);
	DB_GetDatabaseFileName(dbMedia, DB_CHECK_BACKUP_FILE, strDataBaseBackup);
	//sprintf(strDataPathBackup, "%s/%s", DBDir[dbMedia], strDataBaseBackup);	/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, strDataPathBackup, strDataBaseBackup);			/* Samba Record Modification - 20131004 */
	//sprintf(strDataPathBackup, "%s/%s", DBDir[dbMedia], DB_STR_DATABASE_BACKUP_FILENAME);
	/* DB Search - Dean - 20140214 - Begin */
	DB_GetDatabaseFileName(dbMedia, DB_CHECK_SEARCH_FILE, strDataBaseSearch);
	//sprintf(strDataPath, "%s/%s", DBDir[dbMedia], strDataBaseMain);
	sprintf(strDataPathSearch, "%s/%s", DB_FILE_SEARCH_PATH, strDataBaseSearch);
	/* DB Search - Dean - 20140214 - End */
	/* DB File - Dean - 20130530 - End */

	switch(dbMaintain)
	{
		case DB_MAINTAIN_DATA_ACTION_BACKUP:
			//fprintf(stderr,"DB_FileMaintain - 1 - DB_MAINTAIN_DATA_ACTION_BACKUP\n");
			FileUnlink(strDataPathBackup);													/* Record Media - Dean - 20130530 *//* DB Maintain - Dean - 20121226 - Remove */
			RtnValue = (FileCopy(strDataPath, strDataPathBackup) == FILE_ACTION_SUCCESS)? DB_ACTION_SUCCESS : DB_ACTION_ERROR;	/* DB Maintain - Dean - 20121225 */
			break;
		case DB_MAINTAIN_DATA_ACTION_RESTORE:
			//fprintf(stderr,"DB_FileMaintain - 2 - DB_MAINTAIN_DATA_ACTION_RESTORE\n");
			FileUnlink(strDataPath);														/* Record Media - Dean - 20130530 *//* DB Maintain - Dean - 20121226 - Remove */
			RtnValue = (FileCopy(strDataPathSearch/*strDataPathBackup*/, strDataPath) == FILE_ACTION_SUCCESS)? DB_ACTION_SUCCESS : DB_ACTION_ERROR;			/* DB Search - Dean - 20140214 */

			break;
		/* DB Search - Dean - 20140214 - Begin */
		case DB_MAINTAIN_DATA_ACTION_REPAIR:
			RtnValue = DB_ACTION_ERROR;
			break;
		case DB_MAINTAIN_DATA_ACTION_SEARCH:
			//fprintf(stderr,"DB_FileMaintain - 3 - DB_MAINTAIN_DATA_ACTION_SEARCH\n");
			if (DB_CheckSearchAvailable(dbMedia) == FILE_ACTION_BUSY)
				break;
			//fprintf(stderr,"DB_FileMaintain - 3a - DB_MAINTAIN_DATA_ACTION_SEARCH\n");
			DB_DoSearchFlagTouch(dbMedia);
			FileUnlink(strDataPathSearch);
			RtnValue = (FileCopy(strDataPath, strDataPathSearch) == FILE_ACTION_SUCCESS)? DB_ACTION_SUCCESS : DB_ACTION_ERROR;
			DB_DoSearchFlagUnlink(dbMedia);
			//fprintf(stderr,"DB_FileMaintain - 3b - DB_MAINTAIN_DATA_ACTION_SEARCH\n");
			break;
		/* DB Search - Dean - 20140214 - End */
		default :
			RtnValue = DB_ACTION_ERROR;													/* DB Maintain - Dean - 20121226 */
			break;
	}

	if (RtnValue != DB_ACTION_SUCCESS)													/* DB Maintain - Dean - 20121226 */
		return DB_ACTION_ERROR;

	return DB_ACTION_SUCCESS;
}

int FileCopy(char* strFileSrc, char* strFileDest)
{
#define BUF_LEN 4096
	FILE *fpSrc, *fpDest;
	int lenSrc, lenDest, len, rlen;
	char szBuf[BUF_LEN];
#if 0
	struct stat stSrc;

	//fprintf(stderr,"FileCopy - Enter\n");
	if ((stat(strFileSrc, &stSrc) != 0))
	{
		fprintf(stderr,"FileCopy - Error 1\n");
		return FILE_ACTION_ERROR;
	}
#endif
	if ((fpSrc = fopen(strFileSrc, "rb")) == 0)
	{
		//fprintf(stderr,"FileCopy - open file %s Error\n", strFileSrc);
		return FILE_ACTION_ERROR;
	}
	if ((fpDest = fopen(strFileDest, "wb")) == 0)
	{
		fclose(fpDest);
		//fprintf(stderr,"FileCopy - open file %s Error\n", strFileDest);
		return FILE_ACTION_ERROR;
	}

	fseek(fpSrc, 0, SEEK_END);
	lenSrc = ftell(fpSrc);
	fseek(fpSrc, 0, SEEK_SET);
	len = lenSrc;
	while (len > 0)
	{
		if (len > BUF_LEN)
			rlen = BUF_LEN;
		else
			rlen = len;
		fread(szBuf, 1, rlen, fpSrc);
		fwrite(szBuf, 1, rlen, fpDest);
		len -= rlen;
	}
	fflush(fpDest);
	fclose(fpDest);
	fclose(fpSrc);

	if ((fpDest = fopen(strFileDest, "rb")) == 0)
	{
		//fprintf(stderr,"FileCopy - Error 3\n");
		return FILE_ACTION_ERROR;
	}
	fseek(fpDest, 0, SEEK_END);
	lenDest = ftell(fpDest);
	fclose(fpDest);

	if (lenDest != lenSrc)
	{
		//fprintf(stderr,"FileCopy - Error 4\n");
		return FILE_ACTION_ERROR;
	}

	//fprintf(stderr,"FileCopy - Complete\n");
	return FILE_ACTION_SUCCESS;
}
/* DB File Maintain - Dean - 20121219 - End */

int DB_FileWrite(FILE* pfile, DB_MEDIA dbMedia)
{
	int loopi;
	int nWrite;
	int loopMax = DBList[dbMedia].head.total;						/*DB Header - Dean - 20130117*/
//	char tmp[DB_FILE_INFO_TEMP_LEN + 1] = "";

#if 0
	fwrite(&DBList[dbMedia].head.total, sizeof(int), 1, pfile);
	fwrite(&DBList[dbMedia].head.record, sizeof(int), 1, pfile);	/* DB Maintain - Check Duplicate File - Dean - 20130114 */
	fwrite(tmp, sizeof(char), DB_FILE_INFO_TEMP_LEN, pfile);
#else
	fwrite(&DBList[dbMedia].head, sizeof(DBListHead), 1, pfile);	/*DB Header - Dean - 20130117*/
#endif
	//fprintf(stderr,"DB_FileWrite - loopMax = %d\n", loopMax);
	DBList[dbMedia].head.total = 0;									/*DB Header - Dean - 20130117*/
#if 0
	for (loopi = 0; loopi < loopMax; loopi++)
	{
		if (DBList[dbMedia].KeepList[loopi])
		{
			nWrite = fwrite(&DBList[dbMedia].InfoList[loopi], DB_FILE_INFO_DATA_LEN, 1, pfile);
			DBList[dbMedia].head.total = DBList[dbMedia].head.total + 1;						/*DB Header - Dean - 20130117*/
			//fprintf(stderr,"DB_FileWrite - Save - [%d]loopi = %d\n", DBList[dbMedia].total, loopi);
		}
		else
		{
			//fprintf(stderr,"DB_FileWrite - Remove - [%d]loopi = %d\n", DBList[dbMedia].total, loopi);
		}
	}
#else
	int iStartIdx = 0;
	int iEndIdx = loopMax - 1;
	while (iStartIdx < loopMax)
	{
		//fprintf(stderr,"DB_FileWrite - 1 - iStartIdx = %d, iEndIdx = %d\n", iStartIdx, iEndIdx); /*DB Header - Dean - 20130117*/
		/* Find Start Index */
		for (loopi = iStartIdx; loopi < loopMax; loopi ++)
		{
			//fprintf(stderr,"DB_FileWrite - 1a - DBList[dbMedia].KeepList[%d] = %d\n", loopi, DBList[dbMedia].KeepList[loopi]); /*DB Header - Dean - 20130117*/
			if (DBList[dbMedia].KeepList[loopi] == 1)
			{
				iStartIdx = loopi;
				break;
			}
		}
		//fprintf(stderr,"DB_FileWrite - 2 - iStartIdx = %d, iEndIdx = %d\n", iStartIdx, iEndIdx); /*DB Header - Dean - 20130117*/

		if (loopi == loopMax)
		{
			//fprintf(stderr,"DB_FileWrite - 2 - 1 - Reach the End - loopi = %d, loopMax = %d\n", loopi, loopMax); /*DB Header - Dean - 20130117*/
			break;
		}

		/* Find End Index */
		iEndIdx = iStartIdx;
		for (loopi = iStartIdx + 1 ; loopi < loopMax; loopi ++)
		{
			//fprintf(stderr,"DB_FileWrite - 2a - DBList[dbMedia].KeepList[%d] = %d\n", loopi, DBList[dbMedia].KeepList[loopi]); /*DB Header - Dean - 20130117*/
			if (DBList[dbMedia].KeepList[loopi] == 0)
			{
				iEndIdx = loopi - 1;
				break;
			}
		}

		//fprintf(stderr,"DB_FileWrite - 3 - iStartIdx = %d, iEndIdx = %d\n", iStartIdx, iEndIdx); /*DB Header - Dean - 20130117*/
		/* Check if reach the loopMax */
		if ((iEndIdx == iStartIdx) && (loopi == loopMax))
			iEndIdx = loopMax - 1;
		//fprintf(stderr,"DB_FileWrite - a - iStartIdx = %d, iEndIdx = %d\n", iStartIdx, iEndIdx); /*DB Header - Dean - 20130117*/

		int iNum = iEndIdx - iStartIdx + 1;
		nWrite = fwrite(&DBList[dbMedia].InfoList[iStartIdx], DB_FILE_INFO_DATA_LEN, iNum, pfile);
		//fprintf(stderr,"DB_FileWrite - nWrite = %d\n", nWrite); /*DB Header - Dean - 20130117*/
		DBList[dbMedia].head.total = DBList[dbMedia].head.total + nWrite;						/*DB Header - Dean - 20130117*/
		/* DB Search - Dean - 20140214 - Begin */
		memcpy(&DBList[dbMedia].InfoList[DBList[dbMedia].head.total-1], &DBList[dbMedia].InfoList[iStartIdx], iNum*DB_FILE_INFO_DATA_LEN);
		/* DB Search - Dean - 20140214 - Emd */
		iStartIdx =  iEndIdx + 1;
	}
#endif

	rewind(pfile);
	//fprintf(stderr,"DB_FileWrite - End - [%d]\n", DBList[dbMedia].head.total);	/*DB Header - Dean - 20130117*/
	fwrite(&DBList[dbMedia].head.total, sizeof(int), 1, pfile);	/*DB Header - Dean - 20130117*/
	fflush(pfile);											/* DB File Maintain - Dean - 20121219 */

	return DBList[dbMedia].head.total;						/*DB Header - Dean - 20130117*/
}

int DB_AllocateDataBuffer(char** ppBuffer, int iNum, int iLen)
{
	int RtnValue = DB_ACTION_SUCCESS;

	*ppBuffer = malloc(iNum*iLen);
	if (*ppBuffer == NULL)
		RtnValue = DB_ACTION_FAIL;
	else
		memset(*ppBuffer, 0, iNum*iLen);

	return RtnValue;
}

int DB_DestroyDataBuffer(char* pBuffer)
{
	int RtnValue = DB_ACTION_SUCCESS;

	free(pBuffer);

	return RtnValue;
}

void DB_InitListPath(DB_MEDIA dbMedia)
{
	SysInfo *pSysInfo = GetSysInfo();
	
	if (REC_TO_SDCARD)
	{
		if (dbMedia == DB_MEDIA_SDCARD_EVENT)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, MMC_EVENT_DIR);
		if (dbMedia == DB_MEDIA_SDCARD_RECORD)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, MMC_RECORD_DIR);
	}
	if (REC_TO_SAMBA)
	{
		if (dbMedia == DB_MEDIA_SAMBA_EVENT)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, pSysInfo->extrainfo.event_server.samba_mount_path);
		if (dbMedia == DB_MEDIA_SAMBA_RECORD)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, pSysInfo->samba_record.samba_mount_path);
	}
	if (REC_TO_USB)
	{
		if (dbMedia == DB_MEDIA_USB_EVENT)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, USB_EVENT_DIR);
		if (dbMedia == DB_MEDIA_USB_RECORD)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, USB_RECORD_DIR);
	}
	if (REC_TO_TMP)
	{
		if (dbMedia == DB_MEDIA_TMP_EVENT)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, TEMP_FOLDER);
		if (dbMedia == DB_MEDIA_TMP_RECORD)
			snprintf(&DBList[dbMedia].DBPath[0], DB_MEDIA_DIR_LEN-1, TEMP_FOLDER);
	}

	fprintf(stderr,"DB_InitPath - %s\n", &DBList[dbMedia].DBPath);
	return;
}

int DB_InitList(DB_MEDIA dbMedia)
{
	int RtnValue = DB_ACTION_SUCCESS;

	DBList[dbMedia].head.total = 0;					/*DB Header - Dean - 20130117*/
	DBList[dbMedia].head.record = 0xFFFF;			/*DB Header - Dean - 20130117*//* DB Maintain - Check Duplicate File - Dean - 20130114 */
	memset(DBList[dbMedia].KeepList, 0, (DB_FILE_NUM_MAX + 1) * sizeof(char));
	memset(DBList[dbMedia].InfoList, 0, (DB_FILE_NUM_MAX + 1) * sizeof(DBItemInfo));

	DB_InitListPath(dbMedia);		/* DB Search - Dean - 20140214 */

	return RtnValue;
}

int DB_GetList(DB_MEDIA dbMedia, DB_READ_DATA_ACTION dbReadAct, long lStartTime, long lEndTime)		/* DB Maintain - Dean - 20130110 */
{
	int iTotalFileNum = DB_ACTION_ERROR;
	int iCheckMain = DB_ACTION_SUCCESS;
	int iCheckBackup = DB_ACTION_SUCCESS;

	DB_InitListPath(dbMedia);														/* DB Search - Dean - 20140214 */

	/* Check Database Main File */
	if (DB_FileCheck(dbMedia, DB_CHECK_MAIN_FILE) == DB_ACTION_ERROR)
		iCheckMain = DB_ACTION_ERROR;

	/* Check Database Backup File */
	if (DB_FileCheck(dbMedia, DB_CHECK_BACKUP_FILE) == DB_ACTION_ERROR)
		iCheckBackup = DB_ACTION_ERROR;

	/* Read Data */
	if (iCheckMain == DB_ACTION_SUCCESS)
	{
		iTotalFileNum = DB_DoRead(dbMedia, dbReadAct, lStartTime, lEndTime);			/* DB Maintain - Dean - 20130110 */

		/* Read Data if Success then Return */
		if (iTotalFileNum != DB_ACTION_ERROR)
			return iTotalFileNum;
	}

	/* Read Data Fail - Restore from Backup if Backup Exists */
	if (iCheckBackup == DB_ACTION_SUCCESS)
	{
		if (DB_FileMaintain(dbMedia, DB_MAINTAIN_DATA_ACTION_RESTORE) == DB_ACTION_SUCCESS)
		{
			iTotalFileNum = DB_DoRead(dbMedia, dbReadAct, lStartTime, lEndTime);		/* DB Maintain - Dean - 20130110 */

			/* Read Data if Success then Return */
			if (iTotalFileNum != DB_ACTION_ERROR)
				return iTotalFileNum;
		}
	}

	/* Fail to Read Data - Do Init */
	//fprintf(stderr,"DB_GetList - sizeof(DBItemInfo) = %d Byte\n", sizeof(DBItemInfo));
	//fprintf(stderr,"DB_GetList - sizeof(DBListInfo) = %d kByte\n", sizeof(DBListInfo)/1024);
	//fprintf(stderr,"DB_GetList - sizeof(DBList)      = %d kByte\n", sizeof(DBList)/1024);
	//fprintf(stderr,"DB_GetList - sizeof(DBDir)       = %d  Byte\n", sizeof(DBDir));
	//fprintf(stderr,"DB_GetList - sizeof(DBOpShift) = %d  Byte\n", sizeof(DBOpShift));
	//fprintf(stderr,"DB_GetList - sizeof(DBCount)   = %d  Byte\n", sizeof(DBCount));
	DB_InitList(dbMedia);
	//fprintf(stderr,"DB_GetList - DB_SaveList\n");
	iTotalFileNum = DB_SaveList(dbMedia);

	return iTotalFileNum;
}

int DB_SaveList(DB_MEDIA dbMedia)
{
	FILE *pfile;
	int RtnValue = DB_ACTION_SUCCESS;

	DB_FileUnlink(dbMedia);													/* DB File - Dean - 20130530 */

	//fprintf(stderr,"DB_SaveList - DB_FileOpen\n");
	if (DB_FileOpen(&pfile, dbMedia, DB_FILE_WRITE) != DB_ACTION_SUCCESS)	/* DB File Maintain - Dean - 20121219 */
		return DB_ACTION_ERROR;											/* DB File Maintain - Dean - 20121219 */

	//fprintf(stderr,"DB_SaveList - DB_FileWrite\n");
	RtnValue = DB_FileWrite(pfile, dbMedia); 								/* DB File Maintain - Dean - 20121219 */
	DB_FileClose(pfile);

	/* DB Search - Dean - 20140214 - Begin */
//	DB_FileMaintain(dbMedia, DB_MAINTAIN_DATA_ACTION_BACKUP);				/* DB File Maintain - Dean - 20121219 */
	DB_FileMaintain(dbMedia, DB_MAINTAIN_DATA_ACTION_SEARCH);			/* DB File Maintain - Dean - 20140224 */
	/* Save Search File */
	#if 0																/* DB File Maintain - Dean - 20140224 */
	if (DB_SearchFileOpen(&pfile, dbMedia, DB_FILE_WRITE) != DB_ACTION_SUCCESS)	/* DB File Maintain - Dean - 20121219 */
	{
		DB_DoSearchFlagUnlink(dbMedia);
		return DB_ACTION_ERROR;											/* DB File Maintain - Dean - 20121219 */
	}
	RtnValue = DB_FileWrite(pfile, dbMedia); 								/* DB File Maintain - Dean - 20121219 */
	DB_FileClose(pfile);
	DB_DoSearchFlagUnlink(dbMedia);
	#endif																/* DB File Maintain - Dean - 20140224 */
	/* DB Search - Dean - 20140214 - End */
	if (GetfreememSize() < 0)												/* DB File Maintain - Dean - 20130530 */
		return RtnValue;													/* DB File Maintain - Dean - 20130530 */

	return RtnValue;
}

int DB_FlagSet(DB_MEDIA dbMedia, DB_FLAG dbFlag, DB_FLAG_OP dbFlagOp)
{
	DBList[dbMedia].flag[dbFlag] = dbFlagOp;
	return DBList[dbMedia].flag[dbFlag];
}

int DB_FlagCheck(DB_MEDIA dbMedia, DB_FLAG dbFlag)
{
	return DBList[dbMedia].flag[dbFlag];
}

int DB_CheckAvailable(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCheck)
{
	if (DBCount[dbCheck])			/* CheckAvailable - 20130611 */
		return DB_ACTION_BUSY;		/* CheckAvailable - 20130611 */
	DBCount[dbCheck] = 0;
	while (DB_FlagCheck(dbMedia, DB_FLAG_BUSY) == DB_FLAG_OP_ON)
	{
		DBCount[dbCheck] ++;
		if (DBCount[dbCheck] > DB_RETRY_COUNT_MAX)
		{
			DBCount[dbCheck] = 0;	/* CheckAvailable - 20130611 */
			return DB_ACTION_BUSY;	/* DB File Maintain - Dean - 20121219 */
		}
		usleep(DB_RETRY_COUNT_LOOP_TIME);
	}
	DBCount[dbCheck] = 0;			/* CheckAvailable - 20130611 */

	return DB_ACTION_SUCCESS;
}

int DB_RecycleCheck(DB_MEDIA dbMedia, DB_RECYCLE_OP dbRecycleOp, int iValue)
{
	if (dbRecycleOp == DB_RECYCLE_OP_AMOUNT)
	{
		if (DBList[dbMedia].head.total >= iValue)		/*DB Header - Dean - 20130117*/
		{
			if (DB_ItemRemove(dbMedia, 0) == DB_ACTION_ERROR)
				return DB_ACTION_ERROR;					/* DB File Maintain - Dean - 20121219 */
			return DB_ACTION_UPDATE;					/* DB Maintain - Dean - 20130110 */
		}
		return DB_ACTION_SUCCESS;
	}

	/* DB Maintain - Check Duplicate File - Dean - Begin - 20130114 */
	if (dbRecycleOp == DB_RECYCLE_OP_RECORD)
	{
		if (DB_ItemRemove(dbMedia, iValue) == DB_ACTION_ERROR)
			return DB_ACTION_ERROR;
		return DB_ACTION_UPDATE;
	}
	/* DB Maintain - Check Duplicate File - Dean - End - 20130114 */

	if (dbRecycleOp == DB_RECYCLE_OP_FREESPACE)
	{
		int iFreeSpace = 0;
		int iFileSpace = 0;
		//int iIdx = 0;									/* DB File Maintain - Dean - 20121219 */
		int loopi, loopMax = DBList[dbMedia].head.total;/*DB Header - Dean - 20130117*/
		if (loopMax == 0)	return DB_ACTION_FAIL;		/* DB Maintain - Record - Dean - 20130117-2 - Begin */

		//iIdx = 0;										/* DB File Maintain - Dean - 20121219 */
		for (loopi =0; loopi < loopMax; loopi++)
		{
			iFileSpace = DB_ItemRemove(dbMedia, loopi);	/* DB File Maintain - Dean - 20121219 */
			//fprintf(stderr,"DB_RecycleCheck - iFreeSpace = %d, iFileSpace = %d\n", iFreeSpace, iFileSpace);
			if (iFileSpace < 0)
				continue;								/* DB File Maintain - Dean - 20121219 */

			iFreeSpace = iFreeSpace + iFileSpace;
			//fprintf(stderr,"DB_RecycleCheck - iFreeSpace = %d, iValue = %d\n", iFreeSpace, iValue);
			if (iFreeSpace >= iValue)
				return DB_ACTION_UPDATE;				/* DB Maintain - Dean - 20130110 */
		}
		return DB_ACTION_UPDATE;						/* DB Maintain - Dean - 20130110 */
	}

	return DB_ACTION_FAIL;
}

/* DB Maintain - Dean - 20130110 - Begin */
int DB_DoRecycle(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, int iCheckSize)
{
	int idbTotal;
	int RtnValue = DB_ACTION_SUCCESS; 						/* DB File Maintain - Dean - 20121219 */
	int iResultSize = DB_ACTION_SUCCESS;//, iResultAmount = DB_ACTION_SUCCESS;
	//int iResultRecord = DB_ACTION_SUCCESS;					/* DB Maintain - Check Duplicate File - Dean - 20130114 */
	int FlagGet = DB_ACTION_ERROR;

	if (DB_CheckAvailable(dbMedia, dbCountCheck) != DB_ACTION_SUCCESS)
		return DB_ACTION_BUSY;								/* DB File Maintain - Dean - 20121219 */

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_ON);

	idbTotal = DB_GetList(dbMedia, DB_READ_DATA_ACTION_NUMBER, DB_FILE_TIME_VALUE_MIN, DB_FILE_TIME_VALUE_MAX);
	if (idbTotal == DB_ACTION_ERROR)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}

	/* Check Size */
	//fprintf(stderr,"DB_DoRecycle - 2 - idbTotal = %d, iCheckSize = %d\n", idbTotal, iCheckSize);
	if (iCheckSize > 0)
	{
		fprintf(stderr,"DB_DoRecycle - 2a - idbTotal = %d\n", idbTotal);
		if (FlagGet == DB_ACTION_ERROR)
		{
			FlagGet = DB_GetList(dbMedia, DB_READ_DATA_ACTION_DATA_ALL, DB_FILE_TIME_VALUE_MIN, DB_FILE_TIME_VALUE_MAX);
			if (FlagGet == DB_ACTION_ERROR)
			{
				DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
				return DB_ACTION_FAIL;
			}
		}
		fprintf(stderr,"DB_DoRecycle - 2b - idbTotal = %d\n", idbTotal);

		iResultSize = DB_RecycleCheck(dbMedia, DB_RECYCLE_OP_FREESPACE, iCheckSize);
		if ((iResultSize  != DB_ACTION_SUCCESS) && (iResultSize  != DB_ACTION_UPDATE))
		{
			DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
			return DB_ACTION_FAIL;
		}
	}

	/* Check if need to save list */
	//fprintf(stderr,"DB_DoRecycle - 3 - DB_ACTION_UPDATE = %d - iResultSize = %d, iResultAmount = %d, iResultRecord = %d\n", DB_ACTION_UPDATE, iResultSize, iResultAmount, iResultRecord);
	//if ((iResultSize  == DB_ACTION_UPDATE) || (iResultAmount  == DB_ACTION_UPDATE) || (iResultRecord == DB_ACTION_UPDATE))	/* DB Maintain - Check Duplicate File - Dean - 20130114 */
	if (iResultSize == DB_ACTION_UPDATE)	/* DB Maintain - Record - Dean - 20130117 */
	{
		//fprintf(stderr,"DB_DoRecycle - DB_SaveList\n");
		/* DB Maintain - Record - Dean - 20130117-2 - Begin */
		RtnValue = DB_SaveList(dbMedia);
		if (RtnValue == DB_ACTION_ERROR)
		{
			DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
			return DB_ACTION_FAIL;
		}
#if 0
		/* Check if Total is 0 */
		if (DB_GetListTotal(dbMedia) == 0)
		{
			DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
			return DB_ACTION_FAIL;
		}
#endif
		/* DB Maintain - Record - Dean - 20130117-2 - End */
	}

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);

	return DB_ACTION_SUCCESS;								/* DB File Maintain - Dean - 20121219 - Remove */
}
/* DB Maintain - Dean - 20130110 - End */

int DB_ItemUpdate(DB_MEDIA dbMedia, int iIdx, char* strFilename)
{
	char strFullname[256] = "";
	struct stat st;
	int RtnValue;

	//fprintf(stderr,"DB_ItemUpdate - %s\n", DBDir[dbMedia]);
	//fprintf(stderr,"DB_ItemUpdate - %s\n", strFilename);
	//sprintf(strFullname,"%s/%s", DBDir[dbMedia], strFilename);	/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, strFullname, strFilename);			/* Samba Record Modification - 20131004 */
	//fprintf(stderr,"DB_ItemUpdate - %s\n", strFullname);

	if (stat(strFullname, &st) != 0)
		return DB_ACTION_ERROR;

	RtnValue = DB_ItemSetInfo(dbMedia, iIdx, DB_KEEP_LIST_OP_ADD, st.st_size/1024, st.st_mtime, strFilename);

	return RtnValue;
}

int DB_DoItemAdd(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, char* strFilename, long lFiletime, char cRecord)	/* DB - File Time - Dean - 20130702 *//* DB Maintain - Check Duplicate File - Dean - 20130114 */
{
	int idbTotal;
	int idbItemIdx;												/* DB Maintain - Record - Dean - 20130117 */

	if (DB_CheckAvailable(dbMedia, dbCountCheck) != DB_ACTION_SUCCESS)
		return DB_ACTION_BUSY;									/* DB File Maintain - Dean - 20121219 */

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_ON);

	/* DB Search - Dean - 20140214 - Begin */
	#if 1
	idbTotal = DB_GetList(dbMedia, DB_READ_DATA_ACTION_SEARCH_ALL, DB_FILE_TIME_VALUE_MIN, DB_FILE_TIME_VALUE_MAX);	/* DB Maintain - Dean - 20130110 */
	if (idbTotal == DB_ACTION_ERROR)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}
	#else
	idbTotal = DB_GetListTotal(dbMedia);
	#endif
	fprintf(stderr,"DB_DoItemAdd - %s\n", &DBList[dbMedia].DBPath);
	/* DB Search - Dean - 20140214 - End */

	/* DB Maintain - Record - Dean - 20130117 - Begin */
	idbItemIdx = idbTotal;
	if (cRecord == 0)
	{
		int iRecord = DB_GetListRecord(dbMedia);
		if (iRecord == 0xFFFF)
		{
			DB_SetListRecord(dbMedia, idbTotal);
			idbItemIdx = idbTotal;
		}
		else
		{
			//fprintf(stderr, "DB_DoItemAdd : Remove iRecord = %d, idbTotal = %d\n", iRecord, idbTotal);
			DB_ItemSetInfo(dbMedia, iRecord, DB_KEEP_LIST_OP_REMOVE, 0, 0, NULL);
			DB_SetListRecord(dbMedia, idbTotal - 1);
			idbItemIdx = idbTotal;
		}
	}

	//fprintf(stderr, "DB_DoItemAdd : idbItemIdx = %d, strFilename = %s\n", idbItemIdx, strFilename);
	if (DB_ItemAdd(dbMedia, idbItemIdx, strFilename, lFiletime) != DB_ACTION_SUCCESS)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}

	/* Check if Total > DB_FILE_NUM_MAX */
	if (DB_GetListTotal(dbMedia) > DB_FILE_NUM_MAX)
	{
		if (DB_ItemRemove(dbMedia, 0) == DB_ACTION_ERROR)
			return DB_ACTION_ERROR;
	}
	/* DB Maintain - Record - Dean - 20130117 - End */

	//fprintf(stderr, "DB_DoItemAdd : idbItemIdx = %d, strFilename = %s\n", idbItemIdx, DBList[dbMedia].InfoList[idbItemIdx].fname);

	DB_SaveList(dbMedia);					/* DB Maintain - Dean - 20130110 */

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);

	return DB_ACTION_SUCCESS;
}

int DB_DoItemRemove(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, char* strFilename)
{
	int idbTotal;

	if (DB_CheckAvailable(dbMedia, dbCountCheck) != DB_ACTION_SUCCESS)
		return DB_ACTION_BUSY;								/* DB File Maintain - Dean - 20121219 */

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_ON);

	idbTotal = DB_GetList(dbMedia, DB_READ_DATA_ACTION_SEARCH, DB_FILE_TIME_VALUE_MIN, DB_FILE_TIME_VALUE_MAX);	/* DB Maintain - Dean - 20130110 */
	if (idbTotal == DB_ACTION_ERROR)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}

	int iIdx = DB_ItemSearch(dbMedia,DB_INFO_LIST_OP_SEARCH_NAME, strFilename);
	if (iIdx == DB_ACTION_ERROR)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}

	if (DB_ItemRemove(dbMedia, iIdx) == DB_ACTION_ERROR)
	{
		DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);
		return DB_ACTION_FAIL;
	}

	/* DB Maintain - Record - Dean - 20130117-2 - Begin */
	int iRecord = DB_GetListRecord(dbMedia);
	if ((iRecord != 0xFFFF) && (iRecord < idbTotal) && (iRecord >= 0))
	{
		if (iIdx == iRecord)
			DB_SetListRecord(dbMedia, 0xFFFF);
		else if (iIdx < iRecord)
			DB_SetListRecord(dbMedia, iRecord - 1);
	}
	/* DB Maintain - Record - Dean - 20130117-2 - End */

	fprintf(stderr,"DB_DoItemRemove - DB_SaveList\n");
	DB_SaveList(dbMedia);					/* DB Maintain - Dean - 20130110 */

	DB_FlagSet(dbMedia, DB_FLAG_BUSY, DB_FLAG_OP_OFF);

	return DB_ACTION_SUCCESS;
}

int DB_GetListTotal(DB_MEDIA dbMedia)
{
	return DBList[dbMedia].head.total;					/*DB Header - Dean - 20130117*/
}

int DB_SetListTotal(DB_MEDIA dbMedia, int iTotal)
{
	DBList[dbMedia].head.total = iTotal;				/*DB Header - Dean - 20130117*/

	return DBList[dbMedia].head.total;					/*DB Header - Dean - 20130117*/
}

/* DB Maintain - Check Duplicate File - Dean - Begin - 20130114 */
int DB_GetListRecord(DB_MEDIA dbMedia)
{
	return DBList[dbMedia].head.record;					/*DB Header - Dean - 20130117*/
}

int DB_SetListRecord(DB_MEDIA dbMedia, int iIdx)
{
	DBList[dbMedia].head.record = iIdx;					/*DB Header - Dean - 20130117*/

	return DBList[dbMedia].head.record;					/*DB Header - Dean - 20130117*/
}
/* DB Maintain - Check Duplicate File - Dean - End - 20130114 */

/* DB Maintain - Add Total Size - Dean - 20130530 - Begin*/
long DB_GetListTotalSize(DB_MEDIA dbMedia)		/* DB Total Size Maintain - Dean - 20131112 */
{
	return DBList[dbMedia].head.totalsize;				/*DB Header - Dean - 20130117*/
}

long DB_SetListTotalSize(DB_MEDIA dbMedia, long iSize)	/* DB Total Size Maintain - Dean - 20131112 */
{
	DBList[dbMedia].head.totalsize = iSize;				/*DB Header - Dean - 20130117*/

	return DBList[dbMedia].head.totalsize;				/*DB Header - Dean - 20130117*/
}
/* DB Maintain - Add Total Size - Dean - 20130530 - End*/

char* DB_GetInfoListPtr(DB_MEDIA dbMedia)
{
	return (char*)&DBList[dbMedia].InfoList;
}

/* Record Media - Dean - 20130530 - Begin */
int DB_GetPath(DB_MEDIA dbMedia, char* strDir)
{
	/* DB Search - Dean - 20140214 - Begin */
	#if 1
	sprintf(strDir,"%s", &DBList[dbMedia].DBPath);
	#else
	sprintf(strDir,"%s", &DBDir[dbMedia]);
	#endif
	/* DB Search - Dean - 20140214 - End */

	return DB_ACTION_SUCCESS;
}

int DB_SetPath(DB_MEDIA dbMedia, char* strDir)
{
	/* DB Search - Dean - 20140214 - Begin */
#if 1
	sprintf(&DBList[dbMedia].DBPath,"%s", strDir);
#else
	sprintf(&DBDir[dbMedia],"%s", strDir);
#endif
	/* DB Search - Dean - 20140214 - End */

	return DB_ACTION_SUCCESS;
}

int DB_SetSambaPath(DB_MEDIA dbMedia, char* strDir)
{
	char strPath[DB_MEDIA_DIR_LEN] = "";

	if (dbMedia == DB_MEDIA_SAMBA_EVENT)
		sprintf(strPath,"%s%s", strDir, "");//"Event");	/* Samba Record Modification - 20131004 */
	else if (dbMedia == DB_MEDIA_SAMBA_RECORD)
		sprintf(strPath,"%s%s", strDir, "");//"Backup");	/* Samba Record Modification - 20131004 */
	else
		return DB_ACTION_ERROR;

	DB_SetPath(dbMedia, strPath);

	return DB_ACTION_SUCCESS;
}
/* Record Media - Dean - 20130530 - End */

int DB_GetPathLen(DB_MEDIA dbMedia)
{
	/* DB Search - Dean - 20140214 - Begin */
	#if 1
	return strlen(&DBList[dbMedia].DBPath);
	#else
	return strlen(&DBDir[dbMedia]);
	#endif
	/* DB Search - Dean - 20140214 - End */
}

/* Samba Record Modification - 20131004 - Begin */
void DB_GenerateFullname(DB_MEDIA dbMedia, char* strFullname, char* strFilename)
{
	/* DB Search - Dean - 20140214 - Begin */
	#if 1
	//fprintf(stderr,"DB_GenerateFullname - DBList[dbMedia].DBPath = %s\n", &DBList[dbMedia].DBPath);
	if (dbMedia == DB_MEDIA_SAMBA_RECORD)
		sprintf(strFullname,"%s%s", &DBList[dbMedia].DBPath, strFilename);
	else
		sprintf(strFullname,"%s/%s", &DBList[dbMedia].DBPath, strFilename);
	#else
	if (dbMedia == DB_MEDIA_SAMBA_RECORD)
		sprintf(strFullname,"%s%s", DBDir[dbMedia], strFilename);
	else
		sprintf(strFullname,"%s/%s", DBDir[dbMedia], strFilename);
	#endif
	/* DB Search - Dean - 20140214 - End */
}
/* Samba Record Modification - 20131004 - End */

int DB_ItemSearch(DB_MEDIA dbMedia, DB_INFO_LIST_OP dbOp, char* strSearch)
{
	int RtnValue = DB_ACTION_ERROR;
	int loopi, loopMax;
	int iShift = DBOpShift[dbOp];
	char* pItem;

	if (strSearch == NULL)
		return DB_ACTION_ERROR;

	loopMax = DBList[dbMedia].head.total;	/*DB Header - Dean - 20130117*/
	for (loopi = 0; loopi < loopMax; loopi++)
	{
		pItem = (char*)&DBList[dbMedia].InfoList[loopi];
		if (strcmp(strSearch, pItem + iShift) == 0)
		{
			RtnValue = loopi;
			break;
		}
	}

	return RtnValue;
}

int DB_ItemAdd(DB_MEDIA dbMedia, int iIdx, char* strFilename, long lFiletime)			/* DB - File Time - Dean - 20130702 *//* DB Maintain - Record - Dean - 20130117 */
{
	char strFullname[256] = "";
	struct stat st;
	int RtnValue;

	//fprintf(stderr,"DB_ItemAdd - %s\n", DBDir[dbMedia]);
	fprintf(stderr,"DB_ItemAdd - %s/%s\n", &DBList[dbMedia].DBPath, strFilename);
//	fprintf(stderr,"DB_ItemAdd - %s\n", strFilename);
	//sprintf(strFullname,"%s/%s", DBDir[dbMedia], strFilename);	/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, strFullname, strFilename);			/* Samba Record Modification - 20131004 */
	//fprintf(stderr,"DB_ItemAdd - %s\n", strFullname);

	if (stat(strFullname, &st) != 0)
		return DB_ACTION_ERROR;

	#if 0
	/* DB Maintain - Check Duplicate File - Dean - Begin - 20130114 */
	int dbIdx = DBList[dbMedia].total;
	if (dbIdx > 0)
		dbIdx = DB_ItemCheckFilename(dbMedia, strFilename, DB_CHECK_FNAME_SINGLE, DBList[dbMedia].total - 1);

	if (dbIdx == DB_ACTION_ERROR)
		dbIdx = DBList[dbMedia].total;
	/* DB Maintain - Check Duplicate File - Dean - End - 20130114 */
	#endif

	//fprintf(stderr,"DB_ItemAdd - (%d)%d\n", DBList[dbMedia].total, st.st_size/1024);
	//fprintf(stderr,"DB_ItemAdd - (%d)%ld\n", DBList[dbMedia].total, st.st_mtime);
	//fprintf(stderr,"DB_ItemAdd - (%d)%s\n", DBList[dbMedia].total, strFilename);
	RtnValue = DB_ItemSetInfo(dbMedia, iIdx, DB_KEEP_LIST_OP_ADD, st.st_size/1024, lFiletime/*st.st_mtime*/, strFilename);	/* DB - File Time - Dean - 20130702 *//*DB Header - Dean - 20130117*/

	return RtnValue;
}

#if 0
/* DB Maintain - Check Duplicate File - Dean - Begin - 20130114 */
int DB_ItemCheckFilename(DB_MEDIA dbMedia, char* strFilename, DB_CHECK_FNAME dbCheck, int dbIdx)
{
	char strFullname[256] = "";
	struct stat st;
	int RtnValue;
	int loopi, loopMin, loopMax;

	if (dbCheck == DB_CHECK_FNAME_SINGLE)
	{
		loopMin = dbIdx;
		loopMax = dbIdx + 1;
	}
	else if (dbCheck == DB_CHECK_FNAME_ALL)
	{
		loopMin = 0;
		loopMax = DBList[dbMedia].total;
	}
	else
	{
		loopMin = 0;
		loopMax = 0;
	}

	for (loopi = loopMin; loopi < loopMax; loopi ++)
	{
		fprintf(stderr,"DB_ItemCheckFilename - 1 - (%d)%s\n", loopi, DBList[dbMedia].InfoList[loopi].fname);
		fprintf(stderr,"DB_ItemCheckFilename - 1 - %s\n", strFilename);
		if (strncmp(DBList[dbMedia].InfoList[loopi].fname, strFilename, DB_FILE_INFO_NAME_LEN) == 0)
			break;
	}
	fprintf(stderr,"DB_ItemCheckFilename - 2 - (%d)%s\n", dbIdx, strFilename);

	if (loopi >= loopMax)
		return DB_ACTION_ERROR;

	return loopi;
}
/* DB Maintain - Check Duplicate File - Dean - End - 20130114 */
#endif

int DB_ItemRemove(DB_MEDIA dbMedia, int iIdx)
{
	char Keep;
	int iFileSpace;
	long lTime;
	char strFilename[128] = "";
	char strFullname[256] = "";
	int RtnValue;

	RtnValue = DB_ItemGetInfo(dbMedia, iIdx, &Keep, &iFileSpace, &lTime, strFilename);
	//fprintf(stderr,"DB_ItemRemove - Enter - %d\n", RtnValue);
	if (RtnValue != DB_ACTION_SUCCESS)
		return RtnValue;

	//fprintf(stderr,"DB_ItemRemove - (%d)%d\n", iIdx, iFileSpace);
	//fprintf(stderr,"DB_ItemRemove - (%d)%ld\n", iIdx, lTime);
	//fprintf(stderr,"DB_ItemRemove - (%d)%s\n", iIdx, strFilename);
	//sprintf(strFullname,"%s/%s", DBDir[dbMedia], strFilename);	/* Samba Record Modification - 20131004 */
	DB_GenerateFullname(dbMedia, strFullname, strFilename);			/* Samba Record Modification - 20131004 */
	//fprintf(stderr,"DB_ItemRemove - strFullname = %s\n", strFullname);

	DB_SetListTotalSize(dbMedia, DB_GetListTotalSize(dbMedia) - iFileSpace);			/* DB Total Size Maintain - Dean - 20131112 */
	iFileSpace = FileUnlink(strFullname);
	RtnValue = DB_ItemSetInfo(dbMedia, iIdx, DB_KEEP_LIST_OP_REMOVE, 0, 0, NULL);


	//fprintf(stderr,"DB_ItemRemove - RtnValue = %d\n", RtnValue);
	if (RtnValue != DB_ACTION_SUCCESS)
		return RtnValue;

	//fprintf(stderr,"DB_ItemRemove - End - iFileSpace = %d\n", iFileSpace);
	return iFileSpace;
}

int DB_ItemGetInfo(DB_MEDIA dbMedia, int iIdx, char* pKeep, int* pSize, long* pTime, char* strFilename)
{
	if (iIdx >= DB_FILE_NUM_MAX)
		return DB_ACTION_ERROR;

	*pKeep = DBList[dbMedia].KeepList[iIdx];
	sscanf(&DBList[dbMedia].InfoList[iIdx], DB_STR_FILE_INFO_GET_FORMAT, pSize, pTime, strFilename);
	//fprintf(stderr,"DB_ItemGetInfo - (%d)%d\n", iIdx, *pSize);
	//fprintf(stderr,"DB_ItemGetInfo - (%d)%ld\n", iIdx, *pTime);
	//fprintf(stderr,"DB_ItemGetInfo - (%d)%s\n", iIdx, strFilename);

	return DB_ACTION_SUCCESS;
}

int DB_ItemSetInfo(DB_MEDIA dbMedia, int iIdx, char cKeep, int iSize, long lTime, char* strFilename)
{
	//fprintf(stderr,"DB_ItemSetInfo - (%d)%d\n", iIdx, cKeep);
	if (iIdx >= DB_FILE_NUM_MAX)
		return DB_ACTION_ERROR;

	DBList[dbMedia].KeepList[iIdx] = cKeep;
	//fprintf(stderr,"DB_ItemSetInfo - (%d)%d\n", iIdx, DBList[dbMedia].KeepList[iIdx]);
	if (cKeep == DB_KEEP_LIST_OP_REMOVE)
		return DB_ACTION_SUCCESS;

	//fprintf(stderr,"DB_ItemSetInfo - (%d)%d\n", iIdx, iSize);
	//fprintf(stderr,"DB_ItemSetInfo - (%d)%ld\n", iIdx, lTime);
	//fprintf(stderr,"DB_ItemSetInfo - (%d)%s\n", iIdx, strFilename);
	sprintf(&DBList[dbMedia].InfoList[iIdx], DB_STR_FILE_INFO_GET_FORMAT, iSize, lTime, strFilename);
	//fprintf(stderr,"DB_ItemSetInfo - (%d)%s\n", iIdx, (char*)&DBList[dbMedia].InfoList[iIdx]);
	//if (iIdx == DBList[dbMedia].total)		/* DB Maintain - Check Duplicate File - Dean - 20130114 */
	DBList[dbMedia].head.total = DBList[dbMedia].head.total + 1;	/*DB Header - Dean - 20130117*/
	DBList[dbMedia].head.totalsize = DBList[dbMedia].head.totalsize + iSize;	/* Samba Record Modification - 20131004 */

	return DB_ACTION_SUCCESS;
}

/* DB File Maintain - Dean - 20121219 - Begin - Remove */
//#define FILE_ACTION_ERROR		-1
//#define FILE_ACTION_SUCCESS	1
//#define FILE_ACTION_FAIL		0
/* DB File Maintain - Dean - 20121219 - End - Remove */

int FileTouch(char* strFilename)
{
	FILE* pfile;

	pfile = fopen(strFilename, "wb");
	if (pfile == NULL)
		return FILE_ACTION_ERROR;

	fclose(pfile);

	return FILE_ACTION_SUCCESS;
}

int FileCheck(char* strFilename)
{
	struct stat st;

	if (stat(strFilename, &st) == 0)
		return FILE_ACTION_SUCCESS;

	return FILE_ACTION_ERROR;
}
/* Duplicate DB File for Search - Dean - 20130613 - Begin */
int FileCheckAvailable(char* strFilename)
{
	if (FileCheckCount)
		return FILE_ACTION_BUSY;

	FileCheckCount = 0;
	while (FileCheck(strFilename) == FILE_ACTION_ERROR)
	{
		FileCheckCount ++;
		if (FileCheckCount > FILE_RETRY_COUNT_MAX)
		{
			FileCheckCount = 0;
			return FILE_ACTION_BUSY;
		}
		usleep(FILE_RETRY_COUNT_LOOP_TIME);
	}
	FileCheckCount = 0;

	return FILE_ACTION_SUCCESS;
}

/* PORT_USEDLIST - Dean - 20131007 - Begin */
int FileOpen(FILE** ppfile, char* strFilename, char* type)
{
	*ppfile = fopen(strFilename, type);

	if (*ppfile == NULL)
		return FILE_ACTION_ERROR;

	//fprintf(stderr,"FileOpen - *ppfile = %p\n", *ppfile);
	return FILE_ACTION_SUCCESS;
}

int FileClose(FILE** ppfile)
{
	//fprintf(stderr,"FileClose - *ppfile = %p\n", *ppfile);
	fclose(*ppfile);

	return FILE_ACTION_SUCCESS;
}
/* PORT_USEDLIST - Dean - 20131007 - End */
/* Duplicate DB File for Search - Dean - 20130613 - End */
int FileUnlink(char* strFilename)
{
	struct stat st;

	if (stat(strFilename, &st) != 0)
		return FILE_ACTION_FAIL;				/* DB - Dean - 20121219 -End */

	if (unlink(strFilename) != 0)
		return FILE_ACTION_ERROR;

	return st.st_size/1024;
}
/* DB - End - Dean - 20121213 -End */

/* AVI Record Msg - Dean - 20130530 - Begin */
int FileSave(char* strFilename, char* Data, int iSize)
{
	FILE* pfile;

	pfile = fopen(strFilename, "wb");
	if (pfile == NULL)
		return FILE_ACTION_ERROR;

	fwrite(Data, iSize, 1, pfile);

	fclose(pfile);

	return FILE_ACTION_SUCCESS;
}

int FileRead(char* strFilename, char* Data, int iSize)
{
	FILE* pfile;

	pfile = fopen(strFilename, "rb");
	if (pfile == NULL)
		return FILE_ACTION_ERROR;

	fread(Data, iSize, 1, pfile);

	fclose(pfile);

	return FILE_ACTION_SUCCESS;
}

int FileDoAccess(FILE_OP FileOp, char* strFilename, char* strFlag, char* pData, int iSize)
{
	int RtnValue = FILE_ACTION_SUCCESS;

	/* Check Available */
	//fprintf(stderr,"FileDoAccess\n");
	int AccessCount = 0;
	while ((strFlag != NULL) && (FileCheck(strFlag) == FILE_ACTION_SUCCESS))
	{
		AccessCount ++;
		if (AccessCount > FILE_OP_RETRY_COUNT_MAX)
			return FILE_ACTION_ERROR;
		usleep(FILE_OP_RETRY_COUNT_LOOP_TIME);
	}

	/* Touch Busy Flag */
	if ((strFlag != NULL)  && (FileTouch(strFlag) == FILE_ACTION_ERROR))
		return FILE_ACTION_ERROR;

	/* Read Data*//* Write Data */
	if (FileOp == FILE_OP_READ)
	{
		//fprintf(stderr,"FileDoAccess - Read - 1 - strFilename(%d)(%d) = %s\n", FILE_ACTION_ERROR, FileCheck(strFilename), strFilename);
		if (FileCheck(strFilename) == FILE_ACTION_ERROR)
		{
			if (strFlag != NULL) RtnValue = FileUnlink(strFlag);
			return FILE_ACTION_ERROR;
		}
		//fprintf(stderr,"FileDoAccess - Read - 2\n");
		RtnValue = FileRead(strFilename, pData, iSize);
	}
	else if (FileOp == FILE_OP_WRITE)
		RtnValue = FileSave(strFilename, pData, iSize);

	/* Remove Busy Flag */
	if (strFlag != NULL)
	{
		if (FileUnlink(strFlag) == FILE_ACTION_ERROR)
			return FILE_ACTION_ERROR;
	}

	/* Return Result */
	return RtnValue;
}

int AviRecord_SaveFlag(int iFlag)
{
	return FileDoAccess(FILE_OP_WRITE, AVI_RECORD_STATUS_FLAG, AVI_RECORD_STATUS_FLAG_BUSY, &iFlag, sizeof(int));
}

int AviRecord_AccessParam(FILE_OP FileOp, AviRecordInfo* pAviRecInfo)
{
	//fprintf(stderr,"AviRecord_AccessParam\n");
	return FileDoAccess(FileOp, AVI_RECORD_PARAM, AVI_RECORD_PARAM_BUSY, pAviRecInfo, sizeof(AviRecordInfo));
}

int AviRecord_AccessParam2(FILE_OP FileOp, AviRecordInfo* pAviRecInfo)
{
	//fprintf(stderr,"AviRecord_AccessParam\n");
	return FileDoAccess(FileOp, "/tmp/avi_record_param_test", "/tmp/avi_record_param_test_busy", pAviRecInfo, sizeof(AviRecordInfo));
}

int AviRecord_AccessParam3(FILE_OP FileOp, AviRecordInfo* pAviRecInfo)
{
	//fprintf(stderr,"AviRecord_AccessParam\n");
	return FileDoAccess(FileOp, "/tmp/avi_record_param_test3", "/tmp/avi_record_param_test_busy3", pAviRecInfo, sizeof(AviRecordInfo));
}

/* AVI Record Msg - Dean - 20130530 - End */

/* VIDEO_AUTO_RESIZE - Change Resolution - Dean - 20130709 - Begin */
int Image_Resize(int iPreCaptureID, int iNewCaptureID, int iDir, int iValue)
{
	int RtnValue;
	int image_resolution[IMAGE_RESOLUTION_ITEM_MAX][IMAGE_DIRECTION_TOTAL] = IMAGE_RESOLUTION_2;
	int CurSize = image_resolution[iPreCaptureID][iDir];
	int NewSize = image_resolution[iNewCaptureID][iDir];
	float fMultiplier;

	fMultiplier = (float)NewSize/CurSize;
	RtnValue = iValue * fMultiplier;

	if (RtnValue >= NewSize) RtnValue = NewSize -1;

	return RtnValue;
}

/* Privacy Mask Alignment - Dean - 20131015 - Begin */
void Image_Align(int CaptureID, int iDir, int* iValue0, int* iValue1)
{
	int image_resolution[IMAGE_RESOLUTION_ITEM_MAX][IMAGE_DIRECTION_TOTAL] = IMAGE_RESOLUTION_2;
	int iSize = image_resolution[CaptureID][iDir];
	int iAlign = (iDir == IMAGE_DIRECTION_X) ? OSD_ALIGN_X: OSD_ALIGN_Y;
	int iAlignLength = (iDir == IMAGE_DIRECTION_X) ? OSD_ALIGN_WIDTH : OSD_ALIGN_HEIGHT;

	*iValue0 = OSD_ALIGN_FLOOR(*iValue0, iAlign);
	*iValue1 = OSD_ALIGN_FLOOR(*iValue1, iAlign);

	int iLength = *iValue1 - *iValue0;
	iLength = OSD_ALIGN_CEIL(iLength, iAlignLength);

	*iValue1 = *iValue0 + iLength;

	if (*iValue1 > iSize)
	{
		*iValue1 = iSize;
		*iValue0 = iSize - iLength;
	}
}
/* Privacy Mask Alignment - Dean - 20131015 - End */


/* PORT_USEDLIST - Dean - 20131007 - Begin */
int Port_UsedList_Update_Flag_Set(void)
{
	return FileTouch(PORT_USEDLIST_UPDATE_FLAG);
}

int Port_UsedList_Update_Flag_Remove(void)
{
	return FileUnlink(PORT_USEDLIST_UPDATE_FLAG);
}

int Port_UsedList_Update_Flag_Check(void)
{
	return FileCheck(PORT_USEDLIST_UPDATE_FLAG);
}

int Port_UsedList_Generate(void)
{
	/* Dean - 20140317 - Begin */
	Port_UsedList_Delete();

	//fprintf(stderr,"Port_UsedList_Generate\n");

	//if (FileCheck(PORT_USEDLIST_FILENAME) == FILE_ACTION_ERROR)
//	SendSystemCallCmd(0, SYSTEM_CALL_EVENT_RUN, PORT_USED_LIST_CMD_GENERATE, strlen(PORT_USED_LIST_CMD_GENERATE));
	/* Dean - 20140317 - End */

	return 0;
}

int Port_UsedList_Read(char* PortList)
{
	FILE* pListfile;

	int iCount = 0;
	while (FileCheck(PORT_USEDLIST_FILENAME) == FILE_ACTION_ERROR)
	{
		//fprintf(stderr,"Port_UsedList_Read - iCount = %d\n", iCount);
		if (iCount > PORT_USEDLIST_TIMEOUT)
			return -1;

		sleep(1);
		iCount = iCount + 1;
	};

	/* Wait for PortUsedList File Close */
	usleep(500000);

	//fprintf(stderr,"Port_UsedList_Read\n");
	if (FileOpen(&pListfile, PORT_USEDLIST_FILENAME, "r") == FILE_ACTION_ERROR)
		return 0;
	//fprintf(stderr,"Port_UsedList_Read - pListfile = %p\n", pListfile);

	unsigned int iPort = 0;
	char buf[128];
	int loopi;
	NETSTAT_INFO sNetstatInfo;

	memset(&sNetstatInfo, 0, sizeof(sNetstatInfo));
	while(fgets(buf, sizeof(buf), pListfile) )
	{
		iPort = 0;
		memset(sNetstatInfo.Stat, 0, sizeof(sNetstatInfo.Stat));
		//fprintf(stderr,"Port_UsedList_Read - buf = %s\n", buf);
		sscanf(buf,"%s %d %d %s %s %s",
					sNetstatInfo.Proto,
					&sNetstatInfo.Recv_Q,
					&sNetstatInfo.Send_Q,
					sNetstatInfo.LocalAddress,
					sNetstatInfo.ForeignAddress,
					sNetstatInfo.Stat
					);

//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.Proto[%d] = %s\n", strlen(sNetstatInfo.Proto), sNetstatInfo.Proto);
//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.Recv_Q[%d] = %d\n", sizeof(sNetstatInfo.Recv_Q), sNetstatInfo.Recv_Q);
//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.Send_Q[%d] = %d\n", sizeof(sNetstatInfo.Send_Q), sNetstatInfo.Send_Q);
//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.LocalAddress[%d] = %s\n", strlen(sNetstatInfo.LocalAddress), sNetstatInfo.LocalAddress);
//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.ForeignAddress[%d] = %s\n", strlen(sNetstatInfo.ForeignAddress), sNetstatInfo.ForeignAddress);
//		fprintf(stderr,"Port_UsedList_Read - sNetstatInfo.Stat[%d] = %s\n", strlen(sNetstatInfo.Stat), sNetstatInfo.Stat);
		//if ((!strstr(sNetstatInfo.Proto, "tcp")) && (!strstr(sNetstatInfo.Proto, "udp")))
		//	continue;
		//if (strstr(sNetstatInfo.Stat, "ESTABLISHED") || strstr(sNetstatInfo.Stat, "TIME_WAIT"))
		//	continue;
		if ((strstr(sNetstatInfo.Proto, "tcp") && strstr(sNetstatInfo.Stat, "LISTEN")) ||
			strstr(sNetstatInfo.Proto, "udp"))
		{
			for (loopi = strlen(sNetstatInfo.LocalAddress)-1; loopi > 0; loopi --)
				if (sNetstatInfo.LocalAddress[loopi] == ':')	break;
			if (loopi) sscanf(sNetstatInfo.LocalAddress + loopi + 1,"%d", &iPort);
			if (iPort)
				sprintf(PortList,"%s,%d", PortList, iPort);
		}
	}
	//fprintf(stderr,"Port_UsedList_Read - data = %s\n", PortList);

	FileClose(&pListfile);
	return 0;
}

int Port_UsedList_Delete(void)
{
	unlink(PORT_USEDLIST_FILENAME);

	return 0;
}

/* Port Used List CGI - Dean - 20131021 - Begin */
int Port_SettingList(char* PortList)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;

	sprintf(PortList,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		pSysInfo->lan_config.net.http_port
		,pSysInfo->lan_config.net.https_port
		,pSysInfo->extrainfo.general_net_mode.rtspport
		,pSysInfo->extrainfo.general_net_mode.rtpport
		,pSysInfo->extrainfo.general_net_mode.rtpdataport
		,pSysInfo->extrainfo.general_net_mode.videoport
		,pSysInfo->extrainfo.general_net_mode.videoport+1
		,pSysInfo->extrainfo.general_net_mode.audioport
		,pSysInfo->extrainfo.general_net_mode.audioport+1
		,pSysInfo->ftp_config.port
		,pSysInfo->smtp_config.server_port
		);

	return 0;
}
/* Port Used List CGI - Dean - 20131021 - End */

/* Dean - 20140317 - Begin */
/* Dean - 20140317 - End */

/* PORT_USEDLIST - Dean - 20131007 - End */
int fSetAudioBroadcastPort(unsigned short port)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.audioport, &port, sizeof(port));

	fprintf(stderr, "fSetAudioBroadcastPort !!\n");
	return SetSysInfo(0);
}

int fSetEWdrMode(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	//memcpy(&pSysInfo->lan_config.eWdrMode, &value, sizeof(value));
	pSysInfo->lan_config.eWdrMode = value;

	fprintf(stderr, "fSetEWdrMode %d!!\n", pSysInfo->lan_config.eWdrMode);
	return SetSysInfo(0);
}
int fSetIpMode(unsigned char value) //Add by Joseph 20140408
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->extrainfo.general_net_mode.ipmode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetAppVer(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	strcpy(&pSysInfo->extrainfo.app_version, SYS_DEF_VERSION);
	return SetSysInfo(0);
}

int fSetHttpsMod(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	memcpy(&pSysInfo->lan_config.net.https_mode, &value, sizeof(value));
	return SetSysInfo(0);
}

int fSetEdimaxP2PValue(unsigned char value)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	pSysInfo->lan_config.edimax_cloud = value;
	return SetSysInfo(0);
}
int fSetPtzmode(int nvalue)
{
	SysInfo *pSysInfo = (SysInfo *)pShareMem;
	if (pSysInfo == NULL)
		return -1;
	pSysInfo->ptz_config.eptzmdoe = nvalue;
	return SetSysInfo(0);	
}
