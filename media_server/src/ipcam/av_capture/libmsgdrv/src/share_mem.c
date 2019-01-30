/* ===========================================================================
* @file share_mem.c
*
* @path $(IPNCPATH)/util/
*
* @desc Share memory functions.
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <string.h>
#include "share_mem.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include <sys_env_type.h>
#include <ipnc_ver.h>
#include <Msg_Def.h>
#define DEBUG
#ifdef DEBUG
#define __D(fmt, args...) fprintf(stderr, "Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

static int mid, mExtraIds, mExtraIdc, mMD, mSambaInfo;
static int samba_info_size = 0, md_buf_size = 0, extra_buf_size = 0, sysinfo_buf_size = 0;
/**
* @brief Initialize shared memory driver.

* Initialize shared momory driver.
* @note Once initialed, the memory ID is saved to global variable.
* @return Memory ID
* @retval -1 Fail to initialize shared memory.
*/
int ShareMemInit(int key)
{
	sysinfo_buf_size = sizeof(SysInfo);
#if 0	
	mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | 0660);
#else
	if (sysinfo_buf_size == 0)
		mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | S_IRUSR | S_IWUSR);
	else
	 	mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | S_IRUSR | S_IWUSR);
#endif	

#if 1
	if (mid < 0)
		mid = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mid);
#endif
	if (mid < 0)
		return -1;
	return mid;
}

int ShareMemServerInit(int key)  //by Larry
{
	extra_buf_size = sizeof(Extra_Info);
	if (extra_buf_size == 0)
	{
	  //mExtraIds = shmget(key, PROC_MEM_SIZE, IPC_CREAT | 0660);
		mExtraIds = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
	else
	{
		//mExtraIds = shmget(key, extra_buf_size, IPC_CREAT | 0660);
		mExtraIds = shmget(key, extra_buf_size, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
#if 0	
	if (mExtraIds < 0)
		mExtraIds = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mExtraIds);
#endif
	if (mExtraIds < 0)
		return -1;
	return mExtraIds;
}

void ExtraShareMemServerRead(void *buf, int length)  //by Larry
{
	int seg_id = shmget(EXTRA_KEY, sizeof (Extra_Info), 0666);
	Extra_Info *pSrc = (Extra_Info *)shmat(seg_id, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	memcpy((Extra_Info *)buf, pSrc, length);
	shmdt(pSrc);
}

void ExtraShareMemServerWrite(void *buf, int length)  //by Larry
{
	int seg_id = shmget(EXTRA_KEY, sizeof (Extra_Info), 0666);
	Extra_Info *pDst = (Extra_Info *)shmat(seg_id, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	memcpy(pDst, (Extra_Info *)buf, length);
	shmdt(pDst);
}

int ShareMemClientInit(int key)  //by Larry
{
	extra_buf_size = sizeof(Extra_Info);
	if (extra_buf_size == 0)
	{
	   //mExtraIdc = shmget(key, PROC_MEM_SIZE, 0660);
		mExtraIdc = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, S_IRUSR | S_IWUSR);
	}
	else
	{
		 //mExtraIdc = shmget(key, extra_buf_size, 0660);
		 mExtraIdc = shmget(key, extra_buf_size, S_IRUSR | S_IWUSR);
	}
#if 0
	if (mExtraIdc < 0)
		mExtraIdc = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mExtraIdc);
#endif
	if (mExtraIdc < 0)
		return -1;
	return mExtraIdc;
}

void ExtraShareMemClientRead(void *buf, int length)  //by Larry
{
#if 0
	char *pSrc = shmat(mExtraIdc, 0, 0);
	__D("%s\n",__func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	memcpy(buf, pSrc, length);
	shmdt(pSrc);
#else
	int seg_id = shmget(EXTRA_KEY, sizeof (Extra_Info), 0666);
	Extra_Info *pSrc = (Extra_Info *)shmat(seg_id, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	memcpy((Extra_Info *)buf, pSrc, length);
	shmdt(pSrc);
#endif
}

void ExtraShareMemClientWrite(void *buf, int length)  //by Larry
{
#if 0	
	char *pDst = shmat(mExtraIdc, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	memcpy(pDst, buf, length);
	shmdt(pDst);
#else
	int seg_id = shmget(EXTRA_KEY, sizeof (Extra_Info), 0666);
	Extra_Info *pDst = (Extra_Info *)shmat(seg_id, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	memcpy(pDst, (Extra_Info *)buf, length);
	shmdt(pDst);
#endif
}

int ShareMemMDServerInit(int key)  //by Larry
{
	md_buf_size = sizeof(Motion_Data) * MAX_MD_NUM;
	if (md_buf_size == 0)
	{
	  //mMD = shmget(key, PROC_MEM_SIZE, IPC_CREAT | 0660);
		mMD = shmget(key, PROC_MEM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
	else
	{
		//mMD = shmget(key, md_buf_size, IPC_CREAT | 0660);
		mMD = shmget(key, md_buf_size, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
#if 0	
	if (mMD < 0)
		mMD = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mMD);
#endif
	if (mMD < 0)
		return -1;
	return mMD;
}

int ShareMemMDInit(int key)  //by Larry
{
	md_buf_size = sizeof(Motion_Data) * MAX_MD_NUM;
	if (md_buf_size == 0)
	{
	  //mMD = shmget(key, PROC_MEM_SIZE, 0660);
		mMD = shmget(key, PROC_MEM_SIZE, S_IRUSR | S_IWUSR);
	}
	else
	{
		//mMD = shmget(key, md_buf_size, 0660);
		mMD = shmget(key, md_buf_size, S_IRUSR | S_IWUSR);
	}
#if 0
	if (mMD < 0)
		mMD = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mMD);
#endif
	if (mMD < 0)
		return -1;
	return mMD;
}

void ExtraShareMemMDRead(void *buf, int length)  //by Larry
{
	char *pSrc = (char *)shmat(mMD, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	memcpy(buf, pSrc, length);
	shmdt(pSrc);
}

void ExtraShareMemMDWrite(void *buf, int length)  //by Larry
{
	char *pDst = (char *)shmat(mMD, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	memcpy(pDst, buf, length);
	shmdt(pDst);
}

int ShareMemSambaInfoServerInit(int key)  //by Larry
{
	samba_info_size = sizeof(SAMBA_INFO);
	if (samba_info_size == 0)
	{
	  //mSambaInfo = shmget(key, PROC_MEM_SIZE, IPC_CREAT | 0660);
		mSambaInfo = shmget(key, PROC_MEM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
	else
	{
		//mMD = shmget(key, md_buf_size, IPC_CREAT | 0660);
		mSambaInfo = shmget(key, samba_info_size, IPC_CREAT | S_IRUSR | S_IWUSR);
	}
#if 0
	if (mSambaInfo < 0)
		mSambaInfo = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mSambaInfo);
#endif
	if (mSambaInfo < 0)
		return -1;
	return mSambaInfo;
}

int ShareMemSambaInfoInit(int key)  //by Larry
{
	samba_info_size = sizeof(SAMBA_INFO);
	if (samba_info_size == 0)
	{
	  //mSambaInfo = shmget(key, PROC_MEM_SIZE, 0660);
		mSambaInfo = shmget(key, PROC_MEM_SIZE, S_IRUSR | S_IWUSR);
	}
	else
	{
		//mSambaInfo = shmget(key, md_buf_size, 0660);
		mSambaInfo = shmget(key, samba_info_size, S_IRUSR | S_IWUSR);
	}
#if 0
	if (mSambaInfo < 0)
		mSambaInfo = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mSambaInfo);
#endif
	if (mSambaInfo < 0)
		return -1;
	return mSambaInfo;
}

void ExtraShareMemSambaInfoRead(void *buf, int length)  //by Larry
{
	char *pSrc = (char *)shmat(mSambaInfo, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	memcpy(buf, pSrc, length);
	shmdt(pSrc);
}

void ExtraShareMemSambaInfoWrite(void *buf, int length)  //by Larry
{
	char *pDst = (char *)shmat(mSambaInfo, 0, 0);
	__D("%s\n", __func__);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	memcpy(pDst, buf, length);
	shmdt(pDst);
}

/**
* @brief Initialize pshared memory driver.

* Initialize pshared momory driver.
* @note The memory ID isn't saved to global variable.
* @return Memory ID
* @retval -1 Fail to initialize shared memory.
*/
int pShareMemInit(int key)
{
	int mid;
	sysinfo_buf_size = sizeof(SysInfo);
#if 0	
	mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | 0660);
#else
	if (sysinfo_buf_size == 0)
		mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC*3 , IPC_CREAT | S_IRUSR | S_IWUSR);
	else
		mid = shmget(key, sysinfo_buf_size*3, IPC_CREAT | S_IRUSR | S_IWUSR);
#endif
	__D("shared memory size %d\n", PROC_MEM_SIZE * MAX_SHARE_PROC);
#if 0
	if (mid < 0)
		mid = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mid);
#endif
	if (mid < 0)
		return -1;
	return mid;
}

int pShareMemInit_key(int key)
{
	int mid;
	sysinfo_buf_size = sizeof(SysInfo);
#if 0	
	mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, IPC_CREAT | 0660);
#else
	if (sysinfo_buf_size == 0)
		mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, S_IRUSR | S_IWUSR);
	else
		mid = shmget(key, PROC_MEM_SIZE * MAX_SHARE_PROC, S_IRUSR | S_IWUSR);
#endif
	__D("shared memory size %d\n", PROC_MEM_SIZE * MAX_SHARE_PROC);
#if 0	
	if (mid < 0)
		mid = shmget(key, 0, 0);
	__D("shared memory id:%d\n", mid);
#endif
	if(mid < 0)
		return -1;
	return mid;
}
/**
* @brief Read shared memory driver.

* Read shared momory.
* @param offset [i]  memory offset
* @param *buf [i]  pointer to memory buffer
* @param length [i]  data length to read
*/
void ShareMemRead(int offset,void *buf, int length)
{
	char *pSrc = (char *)shmat(mid, 0, 0);
	__D("%s\n",__func__);
	__D("offset: %d\n", offset);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	if(length>=PROC_MEM_SIZE * MAX_SHARE_PROC)
		fprintf(stderr, "Error for ShareMemRead size > PROC_MEM_SIZE * MAX_SHARE_PROC \n");
	else
		memcpy(buf, pSrc + offset, length);
	shmdt(pSrc);
}
/**
* @brief Write shared memory driver.

* Write shared momory.
* @param offset [i]  memory offset
* @param *buf [i]  pointer to memory buffer
* @param length [i]  data length to read
*/
void ShareMemWrite(int offset,void *buf, int length)
{
	char *pDst = (char *)shmat(mid, 0, 0);
	__D("%s\n",__func__);
	__D("offset: %d\n", offset);
	__D("buf: %x\n", (unsigned int) buf);
	__D("length: %d\n", length);
	__D("pDst: %p\n", pDst);
	__D("pDst + offset: %p\n", pDst + offset);
	if(length>=PROC_MEM_SIZE * MAX_SHARE_PROC)
		fprintf(stderr, "Error for ShareMemWrite size > PROC_MEM_SIZE * MAX_SHARE_PROC \n");
	else		
		memcpy(pDst + offset, buf, length);
	shmdt(pDst);
}
