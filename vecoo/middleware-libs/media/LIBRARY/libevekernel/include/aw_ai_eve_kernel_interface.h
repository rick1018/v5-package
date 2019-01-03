#ifndef _AW_AI_EVE_KERNEL_INTERFACE_H
#define _AW_AI_EVE_KERNEL_INTERFACE_H

/*
	EVE api functions.
	Do not modify!

	Writen by limianhe<limianhe@allwinnertech.com>
*/

#include "aw_ai_eve_type.h"

//----------------------------------------------
// EXPORTS and IMPORTS definition
#ifdef WIN32 
#define AW_EXPORTS __declspec(dllexport)
#else
#define AW_EXPORTS 
#include <pthread.h>
#endif


//! \brief ����EVEģ�������Դ���ַ
//! \pParam pEVECtrl [in] ����Դ��ַ
//! \return             ����״̬���ο�\ref AW_STATUS_E
AW_EXPORTS  AW_STATUS_E  AW_AI_EVE_Kernel_SetSrcAddr(AW_U32 addrPtr);

//! \brief ����EVEģ���DMA�������������ִ��ģʽ��0-�첽��1-ͬ��
//! \dmaMode pEVECtrl [in] DMA��������ִ��ģʽ
//! \return             ����״̬���ο�\ref AW_STATUS_E
AW_EXPORTS  AW_STATUS_E  AW_AI_EVE_Kernel_SetDMAMode(AW_S32 dmaMode);

//! \brief ����EVEģ�鲢��ʼ��������DMA����
//! \pParam pEVECtrl [in] EVE���ò���
//! \return             ����״̬���ο�\ref AW_STATUS_E
AW_EXPORTS  AW_STATUS_E  AW_AI_EVE_Kernel_Init(AW_AI_EVE_CTRL_S* pEVECtrl);

//! \brief ��ֹEVEģ�鲢�ͷ��ڴ�
//! \return             ����״̬���ο�\ref AW_STATUS_E
AW_EXPORTS AW_STATUS_E  AW_AI_EVE_Kernel_UnInit();

//! \brief ����������
//! \param userFunc     [in] �ص����������ڻ�ȡ�����
//! \return             ����״̬���ο�\ref AW_STATUS_E
AW_EXPORTS AW_STATUS_E  AW_AI_EVE_Kernel_Detect(void(*userFunc)(AW_AI_EVE_RESULTS* r, AW_AI_EVE_MIDRESULTS* m, void* p ), void* pUsr);

AW_EXPORTS  AW_STATUS_E  AW_AI_EVE_Kernel_SetDMAParam(AW_AI_EVE_DMA_CTRL_S* pDmaCtrl);

#endif