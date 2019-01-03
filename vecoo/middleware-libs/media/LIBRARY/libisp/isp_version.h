/*
* Copyright (c) 2008-2018 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : isp_version.h
* Description :
* History :
* Author  : zhaowei <zhaowei@allwinnertech.com>
* Date    : 2018/02/08
*
*/

#ifndef ISP_VERSION_H
#define ISP_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define ISP_VERSION "V1.00"
#define REPO_TAG "v5-sdv-v1.0-1.0.1"
#define REPO_BRANCH "libisp-dev"
#define REPO_COMMIT "cff39f7b867953099ccc23c6f011ea9e8508926e"
#define REPO_DATE "Thu Feb 8 18:39:56 2018 +0800"
#define RELEASE_AUTHOR "zhaowei"

static inline void isp_version_info(void)
{
	printf("\n>>>>>>>>>>>>>>>>>>>> ISP VERSION INFO <<<<<<<<<<<<<<<<<<<\n"
		"version:%s\n"
		"tag   : %s\n"
		"branch: %s\n"
		"commit: %s\n"
		"date  : %s\n"
		"author: %s\n"
		"--------------------------------------------------------\n\n",
		ISP_VERSION, REPO_TAG, REPO_BRANCH, REPO_COMMIT, REPO_DATE, RELEASE_AUTHOR);
}

#ifdef __cplusplus
}
#endif

#endif

