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


//���� ��֡��ǰ�����ֽڼ��㴫���ֽ�������������Ҫ�ֶε�����
void CalculateFramesSenquence(uint8_t payload[], long * DataLength, long * FrameNum)
{
	long divisor;
	long remainder;


	// ��֡��byte(0)�ĵ�4bit��byte(1) �Ƕ�֡��own�����ݳ���
	*DataLength = ((payload[0] << 8) + payload[1]) & 0x0FFF;
	divisor = (*DataLength + 1) / (can_datalenth - 1); //+1����Ϊ��֡��byte(0)��byte(1)�����ݣ�-1����Ϊ����֡��byte(0)������
	remainder = (*DataLength + 1) % (can_datalenth - 1);
	FrameNum = remainder ? (divisor + 1) : divisor;// FrameNum:��֡����������������֡�����������Ͷ�һ������
}




//ԭʼ����ת��ΪUDS����(��֡)�� 22 F1 95ת��Ϊ 03 22 F1 95
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