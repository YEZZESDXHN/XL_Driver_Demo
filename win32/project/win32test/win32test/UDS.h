#pragma once
#include <stdint.h>
#include "ini.h"
//#include "SimpleIni.h"
















struct securityAlgorithmInfo
{
	uint8_t level;
	uint8_t Seed[4];
	uint8_t SeedLen;
	uint8_t Xor_Key[4];
	uint8_t Xor_Key_61[4];
	uint8_t outputkey[4];
	unsigned int outputkeylen;
};
struct securityAlgorithmInfo SecurityInfo[4];


struct DiagInfo
{
	uint16_t DIAG_REQ_PHY_ID;
	uint16_t DIAG_RESP_ID;
	uint16_t DIAG_REQ_FUNC_ID;
	uint8_t FS, BS, STmin;

	uint8_t RX_Data[4095];//
	uint16_t RX_byte_counter;
	uint8_t RX_flag;
	uint16_t RX_DataLength;
	uint16_t RX_FrameNum;
	uint8_t RX_N_PCItype;

	uint8_t TX_Original_Data[4095];
	uint8_t TX_Data[4095];
	uint8_t TX_flag;
	uint16_t TX_FrameNum;
	uint8_t TX_SenquenceCount;
	uint16_t TX_Count;
	uint16_t TX_DataLength;
	uint8_t TX_N_PCItype;
};


struct DiagInfo g_diaginfo[128];
uint8_t ECU_Choose = 0;
int gReq_Type;
const uint8_t SF_N_PDU = 0;
const uint8_t FF_N_PDU = 1;
const uint8_t CF_N_PDU = 2;
const uint8_t FC_N_PDU = 3;

const uint8_t can_datalenth= 8;
const uint8_t canfd_datalenth= 64;


void ResetByteArrary(uint8_t data[]);

//根据 首帧的前两个字节计算传输字节数，并计算需要分段的数量
void CalculateFramesSenquence(uint8_t payload[], long * DataLength, long * FrameNum);

//原始数据转换为UDS数据， 22 F1 95转换为 03 22 F1 95
void DataToUDSData(uint8_t data[], long copiedBytes, uint8_t UDSData[]);