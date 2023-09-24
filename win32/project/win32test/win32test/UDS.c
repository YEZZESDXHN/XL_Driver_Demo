#include <stdio.h>
//#include <stdint.h>
//#include "UDS.h"


int gReq_Type;
//const uint8_t SF_N_PDU = 0;
//const uint8_t FF_N_PDU = 1;
//const uint8_t CF_N_PDU = 2;
//const uint8_t FC_N_PDU = 3;



void ResetByteArrary(uint8_t data[])
{
	int i;
	for (i = 0; i <(sizeof(data) / sizeof(data[0])); i++)
		data[i] = 0x55;
}


//根据 首帧的前两个字节计算传输字节数，并计算需要分段的数量
void CalculateFramesSenquence(uint8_t payload[], long * DataLength, long * FrameNum)
{
	long divisor;
	long remainder;


	// 首帧的byte(0)的低4bit和byte(1) 是多帧吧own的数据长度
	*DataLength = ((payload[0] << 8) + payload[1]) & 0x0FFF;
	divisor = (*DataLength + 1) / (can_datalenth - 1); //+1是因为首帧的byte(0)和byte(1)非数据，-1是因为连续帧的byte(0)非数据
	remainder = (*DataLength + 1) % (can_datalenth - 1);
	FrameNum = remainder ? (divisor + 1) : divisor;// FrameNum:多帧报文数量（包含首帧），有余数就多一个报文
}




//原始数据转换为UDS数据(单帧)， 22 F1 95转换为 03 22 F1 95
void DataToUDSData(uint8_t data[], long copiedBytes, uint8_t UDSData[])
{
	//long copiedBytes;
	int i;
	//byte data[4095];
	uint8_t N_PCItype;
	//sysGetVariableData(sysvar::DIAG::SendData,data,copiedBytes);
	if (copiedBytes < 8)
	{
		UDSData[0] = copiedBytes;
		for (i = 0; i < copiedBytes; i++)
		{
			UDSData[i + 1] = data[i];
		}
	}
	else
	{
		N_PCItype = 1;
		UDSData[0] = ((N_PCItype << 4) & 0xF0) + ((copiedBytes >> 8) & 0xF);
		UDSData[1] = copiedBytes & 0xff;

		for (i = 0; i < copiedBytes; i++)
		{
			UDSData[i + 2] = data[i];
		}

	}

}