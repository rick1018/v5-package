/*! \file aw_ai_cve_hcnt_interface.h
\brief aw_ai_cve_hcnt_interface.h document.
*/

#ifndef _AW_AI_CVE_HCNT_INTERFACE_H_
#define _AW_AI_CVE_HCNT_INTERFACE_H_

#include "aw_ai_cve_base_type.h"
#include "aw_ai_cve_hcnt_config.h"
#include "aw_ai_cve_hcnt_result.h"
#include "aw_ai_cve_clbr_config.h"


//! \defgroup group_hcnt 行人计数
//!  行人计数数据类型与函数
//! \{
//! \}


#ifdef __cplusplus
extern "C"{
#endif


//! \ingroup group_hcnt
//! \{

//! \brief 查询算法版本
//! \param pVersion     [out] 版本字符串，字符串缓冲区长度至少是32
//! \return             操作状态，参考\ref AW_STATUS_E
AW_CVE_EXPORTS AW_STATUS_E AW_AI_CVE_HCNT_GetAlgoVersion( char *pVersion );

//! \brief 创建算法模块并初始化。
//! \param pstParamHCNT [in] 目标识别算法参数，参考\ref AW_AI_CVE_HCNT_PARAM
//! \param pstParamCLBR [in] 景深标定算法参数，参考\ref AW_AI_CVE_CLBR_PARAM
//! \return             模块句柄
AW_CVE_EXPORTS AW_HANDLE AW_AI_CVE_HCNT_Init( AW_AI_CVE_HCNT_PARAM *pstParamHCNT, AW_AI_CVE_CLBR_PARAM *pstParamCLBR );


//! \brief 终止算法模块并释放内存
//! \param hModule      [in] 模块句柄
//! \param pstResult    [out] 处理结果，参考\ref AW_AI_CVE_HCNT_RESULT_S
//! \return             操作状态，参考\ref AW_STATUS_E
AW_CVE_EXPORTS AW_STATUS_E AW_AI_CVE_HCNT_UnInit( AW_HANDLE hModule, AW_AI_CVE_HCNT_RESULT_S *pstResult );

//! \brief 处理一幅图像，并获取处理结果
//! \param hModule      [in] 模块句柄
//! \param pstImage     [in] 待处理的输入图像，参考\ref AW_IMAGE_S
//! \param u32TimeStamp [in] 输入图像的时间戳，以图像帧为单位
//! \param pstResult    [out] 处理结果，参考\ref AW_AI_CVE_HCNT_RESULT_S
//! \return             操作状态，参考\ref AW_STATUS_E
AW_CVE_EXPORTS AW_STATUS_E AW_AI_CVE_HCNT_Process( AW_HANDLE hModule, AW_IMAGE_S *pstImage,
                                          AW_U32 u32TimeStamp, AW_AI_CVE_HCNT_RESULT_S *pstResult );

//! \brief 获取最后的错误码结果
//! \param hModule      [in] 模块句柄
//! \return             算法错误码，参考\ref AW_AI_CVE_ERROR_CODE_E
AW_CVE_EXPORTS AW_AI_CVE_ERROR_CODE_E AW_AI_CVE_HCNT_GetLastError( AW_HANDLE hModule );


//! \}


#ifdef __cplusplus
}
#endif


#endif // _AW_AI_CVE_HCNT_INTERFACE_H_



