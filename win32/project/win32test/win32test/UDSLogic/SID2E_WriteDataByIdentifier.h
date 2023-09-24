/******************************************************************************
* 文件名称: SID2E_WriteDataByIdentifier.h
* 内容摘要: 根据标识符写入数据头文件
* 创建者の: 孔佳伟
* 个人主页: https://gitee.com/thin-wind/jump
* 修改记录: 
******************************************************************************/

#ifndef _SID2E_WRITE_DATA_BY_IDENTIFIER_H_
#define _SID2E_WRITE_DATA_BY_IDENTIFIER_H_

#include <stdint.h>
#include "uds_type.h"

/******************************************************************************
* 函数名称: void service_2E_WriteDataByIdentifier(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 2E 服务 - 根据标识符写入数据
* 输入参数: uint8_t*    msg_buf         --数据首地址
    　　　　uint8_t     msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: 无
* 其它说明: 无
******************************************************************************/
void service_2E_WriteDataByIdentifier(const uint8_t* msg_buf, uint16_t msg_dlc);


/******************************************************************************
* 函数名称: bool_t service_2E_check_len(const uint8_t* msg_buf, uint16_t msg_dlc)
* 功能说明: 检查 2E 服务数据长度是否合法
* 输入参数: uint16_t msg_dlc         --数据长度
* 输出参数: 无
* 函数返回: TRUE: 合法; FALSE: 非法
* 其它说明: 无
******************************************************************************/
bool_t service_2E_check_len(const uint8_t* msg_buf, uint16_t msg_dlc);


#endif
/****************EOF****************/
