/*------------------------------------------------------------------------------
| File:
|   xlCANdemo.C
| Project:
|   Sample for XL - Driver Library
|   Example application using 'vxlapi.dll'
|-------------------------------------------------------------------------------
| $Author: visjb $    $Locker: $   $Revision: 101442 $
|-------------------------------------------------------------------------------
| Copyright (c) 2014 by Vector Informatik GmbH.  All rights reserved.
 -----------------------------------------------------------------------------*/

#if defined(_Windows) || defined(_MSC_VER) || defined (__GNUC__)
#define  STRICT
#include <windows.h>
#endif
#include<commctrl.h>
#define UNUSED_PARAM(a) { a=a; }

#define RECEIVE_EVENT_SIZE         1        // DO NOT EDIT! Currently 1 is supported only
#define RX_QUEUE_SIZE              4096     // internal driver queue size in CAN events
#define RX_QUEUE_SIZE_FD           16384    // driver queue size for CAN-FD Rx events
#define ENABLE_CAN_FD_MODE_NO_ISO  0        // switch to activate no iso mode on a CAN FD channel

#include "vxlapi.h"
#include<tchar.h>
 /////////////////////////////////////////////////////////////////////////////
 // globals

char            g_AppName[XL_MAX_APPNAME + 1] = "xlCANdemo";               //!< Application name which is displayed in VHWconf
XLportHandle    g_xlPortHandle = XL_INVALID_PORTHANDLE;      //!< Global porthandle (we use only one!)
XLdriverConfig  g_xlDrvConfig;                                            //!< Contains the actual hardware configuration
XLaccess        g_xlChannelMask = 0;                          //!< Global channelmask (includes all founded channels)
XLaccess        g_xlPermissionMask = 0;                          //!< Global permissionmask (includes all founded channels)
unsigned int    g_BaudRate = 500000;                     //!< Default baudrate
int             g_silent = 0;                          //!< flag to visualize the message events (on/off)
unsigned int    g_TimerRate = 0;                          //!< Global timerrate (to toggel)
unsigned int    g_canFdSupport = 0;                          //!< Global CAN FD support flag
unsigned int    g_canFdModeNoIso = ENABLE_CAN_FD_MODE_NO_ISO;  //!< Global CAN FD ISO (default) / no ISO mode flag

// tread variables
XLhandle        g_hMsgEvent;                                          //!< notification handle for the receive queue
HANDLE          g_hRXThread;                                          //!< thread handle (RX)
HANDLE          g_hTXThread;                                          //!< thread handle (TX)
int             g_RXThreadRun;                                        //!< flag to start/stop the RX thread
int             g_TXThreadRun;                                        //!< flag to start/stop the TX thread (for the transmission burst)
int             g_RXCANThreadRun;                                     //!< flag to start/stop the RX thread
unsigned int    g_TXThreadCanId;                                     //!< CAN-ID the TX thread transmits under
XLaccess        g_TXThreadTxMask;                                     //!< channel mask the TX thread uses for transmitting


unsigned int    g_TXThreadCanId_3E;                                     //!< CAN-ID the TX thread transmits under
XLaccess        g_TXThreadTxMask_3E;                                     //!< channel mask the TX thread uses for transmitting
int             g_TXThreadRun_3E;                                        //!< flag to start/stop the TX thread (for the transmission burst)
HANDLE          g_hTXThread_3E;                                          //!< thread handle (TX)

////////////////////////////////////////////////////////////////////////////
// functions (Threads)

DWORD     WINAPI RxCanFdThread(PVOID par);
DWORD     WINAPI RxThread(PVOID par);
DWORD     WINAPI TxThread(LPVOID par);
DWORD     WINAPI TxThread_3E(LPVOID par);

////////////////////////////////////////////////////////////////////////////
// functions (prototypes)
void     demoHelp(void);
void     demoPrintConfig(void);
XLstatus demoCreateRxThread(void);

#ifdef __GNUC__
static void strncpy_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count)
{
	UNUSED_PARAM(numberOfElements);
	strncpy(strDest, strSource, count);
}

static void sscanf_s(const char *buffer, const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	sscanf(buffer, format, argList);
}
#endif





#include "UDS.h"
#include <stdio.h>

//GUI
#include <tchar.h>
#include <locale.h>
#include<Richedit.h>//富文本编辑控件的头文件
#include"ini.h"
HWND GUI, my_edit, my_button1, my_button2,t1,t2;
HWND Text_out;
HWND Text_in;
HWND Info_out;
HWND debug_info;


HWND button_2001;
HWND button_2002;
HWND button_2003;
HWND button_2004;
HWND button_2005;
HWND button_2006;
HWND button_2007;
HWND button_2008;
HWND button_2009;
HWND button_2010;
HWND button_2011;
HWND button_2012;
HWND button_2013;


HWND ListBox_3000;
HWND ListBox_3001;
HWND ListBox_3002;
HWND ListBox_3003;
HWND ListBox_3004;
HWND ListBox_3005;

HWND ComboBox_4000;
HWND ComboBox_4001;
HWND ID4001_text;
HWND ComboBox_4002;
HWND ComboBox_4003;
HWND ComboBox_4004;
HWND ComboBox_4005;
HWND ComboBox_4006;
HWND ComboBox_4007;

int gSendMsgByCANFD_flag;
int gCANFDBus_flag=1;
int channel_choose = 0;


uint8_t g_channel_choose=0;
uint8_t g_channel_choose_cm = 0;

XLstatus      xlStatus;
XLaccess      xlChanMaskTx = 0;

int           stop = 0;
int           activated = 0;
int           c;
unsigned int  xlChanIndex = 0;
unsigned int  txID = 0x01;
int           outputMode = XL_OUTPUT_MODE_NORMAL;

WCHAR about[128] = L"xlCANdemo - Test Application for XL Family Driver\nAuthor：王志晨\nVersion:V1.2\nData:2023.9.21";
WCHAR ReleaseLog[256] = L"V1.0:初版\nV1.1:优化发送密钥时间,优化dll字符串大小\nV1.2:增加通道选择，优化输出显示,输入支持空格";
//WCHAR dllname[128] = L"SeednKeyExOpt.dll";
WCHAR dllname[128] = L"./SecurityAccessDLL/SeednKeyML.dll";
typedef int(*DLL_FUNCTION_GenerateKeyEx) (const unsigned char*, unsigned int, const unsigned int, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。
typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (const unsigned char*, unsigned int, const unsigned int, const char*, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。

void setHEXDatatocontrol(HWND hwnd, uint8_t hex[], int length, int type);
void settexttocontrol(HWND hwnd, char text[], int type);
void setHEXtocontrol(HWND hwnd, uint16_t hex, int type);








////////////////////////////////////////////////////////////////////////////

//! demoHelp()

//! shows the program functionality
//!
////////////////////////////////////////////////////////////////////////////

void demoHelp(void) {

	printf("\n----------------------------------------------------------\n");
	printf("-                   xlCANdemo - HELP                     -\n");
	printf("----------------------------------------------------------\n");
	printf("- Keyboard commands:                                     -\n");
	printf("- 't'      Transmit a message                            -\n");
	printf("- 'b'      Transmit a message burst (toggle)             -\n");
	printf("- 'm'      Transmit a remote message                     -\n");
	printf("- 'g'      Request chipstate                             -\n");
	printf("- 's'      Start/Stop                                    -\n");
	printf("- 'r'      Reset clock                                   -\n");
	printf("- '+'      Select channel      (up)                      -\n");
	printf("- '-'      Select channel      (down)                    -\n");
	printf("- 'i'      Select transmit Id  (up)                      -\n");
	printf("- 'I'      Select transmit Id  (down)                    -\n");
	printf("- 'x'      Toggle extended/standard Id                   -\n");
	printf("- 'o'      Toggle output mode                            -\n");
	printf("- 'a'      Toggle timer                                  -\n");
	printf("- 'v'      Toggle logging to screen                      -\n");
	printf("- 'p'      Show hardware configuration                   -\n");
	printf("- 'y'      Trigger HW-Sync pulse                         -\n");
	printf("- 'h'      Help                                          -\n");
	printf("- 'ESC'    Exit                                          -\n");
	printf("----------------------------------------------------------\n");
	printf("- 'PH'->PortHandle; 'CM'->ChannelMask; 'PM'->Permission  -\n");
	printf("----------------------------------------------------------\n\n");

}

////////////////////////////////////////////////////////////////////////////

//! demoPrintConfig()

//! shows the actual hardware configuration
//!
////////////////////////////////////////////////////////////////////////////

void demoPrintConfig(void) {

	unsigned int i;
	char         str[100];

	printf("----------------------------------------------------------\n");
	printf("- %02d channels       Hardware Configuration               -\n", g_xlDrvConfig.channelCount);
	printf("----------------------------------------------------------\n");

	for (i = 0; i < g_xlDrvConfig.channelCount; i++) {

		printf("- Ch:%02d, CM:0x%03I64x,",
			g_xlDrvConfig.channel[i].channelIndex, g_xlDrvConfig.channel[i].channelMask);

		strncpy_s(str, 100, g_xlDrvConfig.channel[i].name, 23);
		printf(" %23s,", str);

		memset(str, 0, sizeof(str));

		if (g_xlDrvConfig.channel[i].transceiverType != XL_TRANSCEIVER_TYPE_NONE) {
			strncpy_s(str, 100, g_xlDrvConfig.channel[i].transceiverName, 13);
			printf("%13s -\n", str);
		}
		else printf("    no Cab!   -\n");


	}

	printf("----------------------------------------------------------\n\n");

}

////////////////////////////////////////////////////////////////////////////

//! demoTransmit

//! transmit a CAN message (depending on an ID, channel)
//!
////////////////////////////////////////////////////////////////////////////
XLcanTxEvent canTxEvt_win32;
XLevent       xlEvent_win32;
XLstatus demoTransmit(unsigned int txID, XLaccess xlChanMaskTx)
{
	XLstatus             xlStatus;
	unsigned int         messageCount = 1;
	static int           cnt = 0;

	if (g_canFdSupport) {
	//if (0) {
		unsigned int  fl[3] = {

		  0 , // CAN (no FD)
		  XL_CAN_TXMSG_FLAG_EDL,
		  XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS,
		};

		//XLcanTxEvent canTxEvt;
		unsigned int cntSent;
		unsigned int i;

		//memset(&canTxEvt_win32, 0, sizeof(canTxEvt_win32));
		//canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;

		canTxEvt_win32.tagData.canMsg.canId = txID;
		////canTxEvt_win32.tagData.canMsg.msgFlags = fl[cnt % (sizeof(fl) / sizeof(fl[0]))];
		//canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
		//canTxEvt_win32.tagData.canMsg.dlc = 8;
		//canTxEvt_win32.tagData.canMsg.data[0] = 0x02;
		//canTxEvt_win32.tagData.canMsg.data[1] = 0x10;
		////canTxEvt_win32.tagData.canMsg.data[2] = 0x01;


		



		// if EDL is set, demonstrate transmit with DLC=15 (64 bytes)
		/*if (canTxEvt.tagData.canMsg.msgFlags & XL_CAN_TXMSG_FLAG_EDL) {
			canTxEvt.tagData.canMsg.dlc = 15;
		}

		++cnt;

		for (i = 1; i < XL_CAN_MAX_DATA_LEN; ++i) {
			canTxEvt.tagData.canMsg.data[i] = (unsigned char)i - 1;
		}
		canTxEvt.tagData.canMsg.data[0] = (unsigned char)cnt;*/
		//setHEXtocontrol(debug_info, xlChanMaskTx, 1);
		xlStatus = xlCanTransmitEx(g_xlPortHandle, xlChanMaskTx, messageCount, &cntSent, &canTxEvt_win32);
		//xlStatus = xlCanTransmitEx(g_xlPortHandle, 1, messageCount, &cntSent, &canTxEvt_win32);
	}
	else {
		//static XLevent       xlEvent;

		memset(&xlEvent_win32, 0, sizeof(xlEvent_win32));

		xlEvent_win32.tag = XL_TRANSMIT_MSG;
		xlEvent_win32.tagData.msg.id = txID;
		xlEvent_win32.tagData.msg.dlc = 8;
		xlEvent_win32.tagData.msg.flags = 0;
		++xlEvent_win32.tagData.msg.data[0];
		xlEvent_win32.tagData.msg.data[1] = 2;
		xlEvent_win32.tagData.msg.data[2] = 3;
		xlEvent_win32.tagData.msg.data[3] = 4;
		xlEvent_win32.tagData.msg.data[4] = 5;
		xlEvent_win32.tagData.msg.data[5] = 6;
		xlEvent_win32.tagData.msg.data[6] = 7;
		xlEvent_win32.tagData.msg.data[7] = 8;

		xlStatus = xlCanTransmit(g_xlPortHandle, xlChanMaskTx, &messageCount, &xlEvent_win32);

	}

	printf("- Transmit         : CM(0x%I64x), %s\n", xlChanMaskTx, xlGetErrorString(xlStatus));

	return xlStatus;
}

////////////////////////////////////////////////////////////////////////////

//! demoStopTransmitBurst

//! Stop the TX thread if it is running.
//!
////////////////////////////////////////////////////////////////////////////

void demoStopTransmitBurst()
{
	if (g_hTXThread) {
		g_TXThreadRun = 0;
		WaitForSingleObject(g_hTXThread, 10);
		g_hTXThread = 0;
	}
}

void demoStopTransmitBurst_3E()
{
	if (g_hTXThread_3E) {
		g_TXThreadRun_3E = 0;
		WaitForSingleObject(g_hTXThread_3E, 10);
		g_hTXThread_3E = 0;
	}
}

////////////////////////////////////////////////////////////////////////////

//! demoTransmitBurst

//! transmit a message burst (also depending on an IC, channel).
//!
////////////////////////////////////////////////////////////////////////////

void demoTransmitBurst(unsigned int txID, XLaccess xlChanMaskTx)
{
	// first collect old TX-Thread
	demoStopTransmitBurst();

	printf("- print txID: %d\n", txID);
	g_TXThreadCanId = txID;
	g_TXThreadTxMask = xlChanMaskTx;
	g_TXThreadRun = 1;
	g_hTXThread = CreateThread(0, 0x1000, TxThread, NULL, 0, NULL);
}

void demoTransmitBurst_3E(unsigned int txID, XLaccess xlChanMaskTx)
{
	// first collect old TX-Thread
	demoStopTransmitBurst_3E();

	g_TXThreadCanId_3E = txID;
	g_TXThreadTxMask_3E = xlChanMaskTx;
	g_TXThreadRun_3E = 1;
	g_hTXThread_3E = CreateThread(0, 0x1000, TxThread_3E, NULL, 0, NULL);
}

////////////////////////////////////////////////////////////////////////////

//! demoTransmitRemote

//! transmit a remote frame
//!
////////////////////////////////////////////////////////////////////////////

XLstatus demoTransmitRemote(unsigned int txID, XLaccess xlChanMaskTx)
{
	XLstatus      xlStatus;
	unsigned int  messageCount = 1;

	if (g_canFdSupport) {
		XLcanTxEvent canTxEvt;
		unsigned int cntSent;

		memset(&canTxEvt, 0, sizeof(canTxEvt));

		canTxEvt.tag = XL_CAN_EV_TAG_TX_MSG;

		canTxEvt.tagData.canMsg.canId = txID;
		canTxEvt.tagData.canMsg.msgFlags = XL_CAN_TXMSG_FLAG_RTR;
		canTxEvt.tagData.canMsg.dlc = 8;

		xlStatus = xlCanTransmitEx(g_xlPortHandle, xlChanMaskTx, messageCount, &cntSent, &canTxEvt);
	}
	else {
		XLevent       xlEvent;

		memset(&xlEvent, 0, sizeof(xlEvent));

		xlEvent.tag = XL_TRANSMIT_MSG;
		xlEvent.tagData.msg.id = txID;
		xlEvent.tagData.msg.flags = XL_CAN_MSG_FLAG_REMOTE_FRAME;
		xlEvent.tagData.msg.dlc = 8;

		xlStatus = xlCanTransmit(g_xlPortHandle, xlChanMaskTx, &messageCount, &xlEvent);
	}

	printf("- Transmit REMOTE  : CM(0x%I64x), %s\n", g_xlChannelMask, xlGetErrorString(xlStatus));

	return XL_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

//! demoStartStop

//! toggle the channel activate/deactivate
//!
////////////////////////////////////////////////////////////////////////////

XLstatus demoStartStop(int activated)
{
	XLstatus xlStatus;

	if (activated) {
		xlStatus = xlActivateChannel(g_xlPortHandle, g_xlChannelMask, XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
		printf("- ActivateChannel : CM(0x%I64x), %s\n", g_xlChannelMask, xlGetErrorString(xlStatus));
	}
	else {
		demoStopTransmitBurst();
		xlStatus = xlDeactivateChannel(g_xlPortHandle, g_xlChannelMask);
		printf("- DeativateChannel: CM(0x%I64x), %s\n", g_xlChannelMask, xlGetErrorString(xlStatus));
	}

	return XL_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

//! demoSetOutput

//! toggle NORMAL/SILENT mode of a CAN channel
//!
////////////////////////////////////////////////////////////////////////////

XLstatus demoSetOutput(int outputMode, const char *sMode, XLaccess xlChanMaskTx) {

	XLstatus xlStatus;

	// to get an effect we deactivate the channel first.
	xlStatus = xlDeactivateChannel(g_xlPortHandle, g_xlChannelMask);

	xlStatus = xlCanSetChannelOutput(g_xlPortHandle, xlChanMaskTx, outputMode);
	printf("- SetChannelOutput: CM(0x%I64x), %s, %s, %d\n", xlChanMaskTx, sMode, xlGetErrorString(xlStatus), outputMode);

	// and activate the channel again.
	xlStatus = xlActivateChannel(g_xlPortHandle, g_xlChannelMask, XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);

	return xlStatus;

}

////////////////////////////////////////////////////////////////////////////

//! demoCreateRxThread

//! set the notification and creates the thread.
//!
////////////////////////////////////////////////////////////////////////////

XLstatus demoCreateRxThread(void) {
	XLstatus      xlStatus = XL_ERROR;
	DWORD         ThreadId = 0;

	if (g_xlPortHandle != XL_INVALID_PORTHANDLE) {

		// Send a event for each Msg!!!
		xlStatus = xlSetNotification(g_xlPortHandle, &g_hMsgEvent, 1);

		if (g_canFdSupport) {
			g_hRXThread = CreateThread(0, 0x1000, RxCanFdThread, (LPVOID)0, 0, &ThreadId);
		}
		else {
			g_hRXThread = CreateThread(0, 0x1000, RxThread, (LPVOID)0, 0, &ThreadId);
		}

	}
	return xlStatus;
}

////////////////////////////////////////////////////////////////////////////

//! demoInitDriver

//! initializes the driver with one port and all founded channels which
//! have a connected CAN cab/piggy.
//!
////////////////////////////////////////////////////////////////////////////

XLstatus demoInitDriver(XLaccess *pxlChannelMaskTx, unsigned int *pxlChannelIndex) {

	XLstatus          xlStatus;
	unsigned int      i;
	XLaccess          xlChannelMaskFd = 0;
	XLaccess          xlChannelMaskFdNoIso = 0;

	// ------------------------------------
	// open the driver
	// ------------------------------------
	xlStatus = xlOpenDriver();

	// ------------------------------------
	// get/print the hardware configuration
	// ------------------------------------
	if (XL_SUCCESS == xlStatus) {
		xlStatus = xlGetDriverConfig(&g_xlDrvConfig);
	}
	if (gCANFDBus_flag == 1)
	{

		if (XL_SUCCESS == xlStatus) {
			demoPrintConfig();

			//printf("Usage: xlCANdemo <BaudRate> <ApplicationName> <Identifier>\n\n");

			// ------------------------------------
			// select the wanted channels
			// ------------------------------------
			g_xlChannelMask = 0;
			for (i = 0; i < g_xlDrvConfig.channelCount; i++) {
				//setHEXtocontrol(debug_info, g_xlDrvConfig.channelCount, 1);

				// we take all hardware we found and supports CAN
				if (g_xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN) {

					if (!*pxlChannelMaskTx) {
						*pxlChannelMaskTx = g_xlDrvConfig.channel[i].channelMask;
						*pxlChannelIndex = g_xlDrvConfig.channel[i].channelIndex;
					}

					// check if we can use CAN FD - the virtual CAN driver supports CAN-FD, but we don't use it
					if ((g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT)
						/*&& (g_xlDrvConfig.channel[i].hwType != XL_HWTYPE_VIRTUAL)*/) {
						xlChannelMaskFd |= g_xlDrvConfig.channel[i].channelMask;
						//setHEXtocontrol(debug_info, xlChannelMaskFd, 1);
						//setHEXtocontrol(debug_info, g_xlDrvConfig.channel[i].channelMask, 1);
						// check CAN FD NO ISO support
						if (g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_BOSCH_SUPPORT) {
							xlChannelMaskFdNoIso |= g_xlDrvConfig.channel[i].channelMask;
						}
					}
					else {
						g_xlChannelMask |= g_xlDrvConfig.channel[i].channelMask;
					}

				}
			}

			// if we found a CAN FD supported channel - we use it.
			if (xlChannelMaskFd && !g_canFdModeNoIso) {

				g_xlChannelMask = xlChannelMaskFd;
				printf("- Use CAN-FD for   : CM=0x%I64x\n", g_xlChannelMask);
				g_canFdSupport = 1;


			}

			if (xlChannelMaskFdNoIso && g_canFdModeNoIso) {

				g_xlChannelMask = xlChannelMaskFdNoIso;
				printf("- Use CAN-FD NO ISO for   : CM=0x%I64x\n", g_xlChannelMask);
				g_canFdSupport = 1;

			}

			if (!g_xlChannelMask) {
				printf("ERROR: no available channels found! (e.g. no CANcabs...)\n\n");
				xlStatus = XL_ERROR;
			}
		}

		//if (g_xlChannelMask&g_channel_choose_cm == 0)//所选通道不支持
		//{
		//	//MessageBox(hWnd, L"不支持CANFD！", L"提示", MB_OK);
		//	xlStatus = XL_ERROR;
		//	return xlStatus;
		//}
		//else
		//{
		//	g_xlChannelMask = g_channel_choose_cm;
		//}

		g_xlPermissionMask = g_xlChannelMask;

		// ------------------------------------
		// open ONE port including all channels
		// ------------------------------------
		if (XL_SUCCESS == xlStatus) {

			// check if we can use CAN FD
			if (g_canFdSupport) {
				xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE_FD, XL_INTERFACE_VERSION_V4, XL_BUS_TYPE_CAN);
			}
			// if not, we make 'normal' CAN
			else {
				xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE, XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);

			}
			printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n",
				g_xlChannelMask, g_xlPortHandle, g_xlPermissionMask, xlGetErrorString(xlStatus));

		}

		if ((XL_SUCCESS == xlStatus) && (XL_INVALID_PORTHANDLE != g_xlPortHandle)) {

			// ------------------------------------
			// if we have permission we set the
			// bus parameters (baudrate)
			// ------------------------------------
			if (g_xlChannelMask == g_xlPermissionMask) {

				if (g_canFdSupport ) {
					XLcanFdConf fdParams;

					memset(&fdParams, 0, sizeof(fdParams));

					// arbitration bitrate
					fdParams.arbitrationBitRate = 500000;
					fdParams.tseg1Abr = 63;
					fdParams.tseg2Abr = 16;
					fdParams.sjwAbr = 2;

					// data bitrate
					fdParams.dataBitRate = fdParams.arbitrationBitRate * 4;
					fdParams.tseg1Dbr = 15;
					fdParams.tseg2Dbr = 4;
					fdParams.sjwDbr = 2;

					if (g_canFdModeNoIso) {
						fdParams.options = CANFD_CONFOPT_NO_ISO;
					}

					xlStatus = xlCanFdSetConfiguration(g_xlPortHandle, g_xlChannelMask, &fdParams);
					printf("- SetFdConfig.     : ABaudr.=%u, DBaudr.=%u, %s\n", fdParams.arbitrationBitRate, fdParams.dataBitRate, xlGetErrorString(xlStatus));

				}
				else {
					xlStatus = xlCanSetChannelBitrate(g_xlPortHandle, g_xlChannelMask, g_BaudRate);
					printf("- SetChannelBitrate: baudr.=%u, %s\n", g_BaudRate, xlGetErrorString(xlStatus));
				}
			}
			else {
				printf("-                  : we have NO init access!\n");
			}
		}
		else {

			xlClosePort(g_xlPortHandle);
			g_xlPortHandle = XL_INVALID_PORTHANDLE;
			xlStatus = XL_ERROR;
		}
	}
	else
	{
		if (XL_SUCCESS == xlStatus) {
			demoPrintConfig();

			//printf("Usage: xlCANdemo <BaudRate> <ApplicationName> <Identifier>\n\n");

			// ------------------------------------
			// select the wanted channels
			// ------------------------------------
			g_xlChannelMask = 0;
			for (i = 0; i < g_xlDrvConfig.channelCount; i++) {

				// we take all hardware we found and supports CAN
				if (g_xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN) {

					if (!*pxlChannelMaskTx) {
						*pxlChannelMaskTx = g_xlDrvConfig.channel[i].channelMask;
						*pxlChannelIndex = g_xlDrvConfig.channel[i].channelIndex;
					}

					//// check if we can use CAN FD - the virtual CAN driver supports CAN-FD, but we don't use it
					//if ((g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT)
					//	&& (g_xlDrvConfig.channel[i].hwType != XL_HWTYPE_VIRTUAL)) {
					//	xlChannelMaskFd |= g_xlDrvConfig.channel[i].channelMask;

					//	// check CAN FD NO ISO support
					//	if (g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_BOSCH_SUPPORT) {
					//		xlChannelMaskFdNoIso |= g_xlDrvConfig.channel[i].channelMask;
					//	}
					//}
					//else {
					//	g_xlChannelMask |= g_xlDrvConfig.channel[i].channelMask;
					//}
					g_xlChannelMask |= g_xlDrvConfig.channel[i].channelMask;
					//g_xlChannelMask = 0;

				}
			}

			// if we found a CAN FD supported channel - we use it.
			if (xlChannelMaskFd && !g_canFdModeNoIso) {

				g_xlChannelMask = xlChannelMaskFd;
				printf("- Use CAN-FD for   : CM=0x%I64x\n", g_xlChannelMask);
				g_canFdSupport = 1;


			}

			if (xlChannelMaskFdNoIso && g_canFdModeNoIso) {

				g_xlChannelMask = xlChannelMaskFdNoIso;
				printf("- Use CAN-FD NO ISO for   : CM=0x%I64x\n", g_xlChannelMask);
				g_canFdSupport = 1;

			}

			if (!g_xlChannelMask) {
				printf("ERROR: no available channels found! (e.g. no CANcabs...)\n\n");
				xlStatus = XL_ERROR;
			}
		}

		g_xlPermissionMask = g_xlChannelMask;

		// ------------------------------------
		// open ONE port including all channels
		// ------------------------------------
		if (XL_SUCCESS == xlStatus) {

			// check if we can use CAN FD
			if (g_canFdSupport) {
				xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE_FD, XL_INTERFACE_VERSION_V4, XL_BUS_TYPE_CAN);
			}
			// if not, we make 'normal' CAN
			else {
				xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE, XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);

			}
			printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n",
				g_xlChannelMask, g_xlPortHandle, g_xlPermissionMask, xlGetErrorString(xlStatus));

		}

		if ((XL_SUCCESS == xlStatus) && (XL_INVALID_PORTHANDLE != g_xlPortHandle)) {

			// ------------------------------------
			// if we have permission we set the
			// bus parameters (baudrate)
			// ------------------------------------
			if (g_xlChannelMask == g_xlPermissionMask) {

				if (g_canFdSupport ) {
					XLcanFdConf fdParams;

					memset(&fdParams, 0, sizeof(fdParams));

					// arbitration bitrate
					fdParams.arbitrationBitRate = 500000;
					fdParams.tseg1Abr = 63;
					fdParams.tseg2Abr = 16;
					fdParams.sjwAbr = 2;

					// data bitrate
					fdParams.dataBitRate = fdParams.arbitrationBitRate * 4;
					fdParams.tseg1Dbr = 15;
					fdParams.tseg2Dbr = 4;
					fdParams.sjwDbr = 2;

					if (g_canFdModeNoIso) {
						fdParams.options = CANFD_CONFOPT_NO_ISO;
					}

					xlStatus = xlCanFdSetConfiguration(g_xlPortHandle, g_xlChannelMask, &fdParams);
					printf("- SetFdConfig.     : ABaudr.=%u, DBaudr.=%u, %s\n", fdParams.arbitrationBitRate, fdParams.dataBitRate, xlGetErrorString(xlStatus));

				}
				else {
					xlStatus = xlCanSetChannelBitrate(g_xlPortHandle, g_xlChannelMask, g_BaudRate);
					printf("- SetChannelBitrate: baudr.=%u, %s\n", g_BaudRate, xlGetErrorString(xlStatus));
				}
			}
			else {
				printf("-                  : we have NO init access!\n");
			}
		}
		else {

			xlClosePort(g_xlPortHandle);
			g_xlPortHandle = XL_INVALID_PORTHANDLE;
			xlStatus = XL_ERROR;
		}
	}
	return xlStatus;

}

////////////////////////////////////////////////////////////////////////////

//! demoCleanUp()

//! close the port and the driver
//!
////////////////////////////////////////////////////////////////////////////

static XLstatus demoCleanUp(void)
{
	XLstatus xlStatus;

	if (g_xlPortHandle != XL_INVALID_PORTHANDLE) {
		xlStatus = xlClosePort(g_xlPortHandle);
		printf("- ClosePort        : PH(0x%x), %s\n", g_xlPortHandle, xlGetErrorString(xlStatus));
	}

	g_xlPortHandle = XL_INVALID_PORTHANDLE;
	xlCloseDriver();

	return XL_SUCCESS;    // No error handling
}


wchar_t *chartowchar(char CStr[])
{
	size_t len = strlen(CStr) + 1;

	size_t converted = 0;

	wchar_t *WStr;

	WStr = (wchar_t*)malloc(len * sizeof(wchar_t));

	mbstowcs_s(&converted, WStr, len, CStr, _TRUNCATE);
	return WStr;
}


char *WCHARtoCHAR(WCHAR text[])
{
	char output[256];
	//WCHAR* wc = L"Hellow World";
	sprintf_s(output, 128,"%ws", text);
	return output;
}

WCHAR *CHARtoWCHAR(CHAR text[])
{
	wchar_t  ws[100];
	swprintf(ws, 100, L"%hs", text);
	return ws;
}


//转换函数
int Wchar2Char(char* charStr, const wchar_t* wcharStr) {
	int len = WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wcharStr, wcslen(wcharStr), charStr, len, NULL, NULL);
	charStr[len] = '\0';
	return len;
}

int Char2Wchar(wchar_t* wcharStr, const char* charStr) {
	int len = MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), NULL, 0);

	MultiByteToWideChar(CP_ACP, 0, charStr, strlen(charStr), wcharStr, len);
	wcharStr[len] = '\0';
	return len;
}

byte ChartoHex(char asciiChar)
{
	byte finalHexByte;

	finalHexByte = 0x00;
	if (asciiChar >= 0x30 && asciiChar <= 0x39)
	{
		finalHexByte = asciiChar - 0x30;
	}
	else if (asciiChar >= 0x41 && asciiChar <= 0x46)
	{
		finalHexByte = asciiChar - 0x37;
	}
	else if (asciiChar >= 0x61 && asciiChar <= 0x66)
	{
		finalHexByte = asciiChar - 0x57;
	}

	return finalHexByte;
}

void char_to_hex(char src[], int len, uint8_t des[])
{
	/*char char_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	uint8_t hex_table[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	while (len--)
	{
		*(des++) = char_table[(*src) >> 4];
		*(des++) = char_table[*(src++) & 0x0f];
	}*/
	for (int i = 0; i < len/2; i++)
	{
		des[i] = ((ChartoHex(src[i*2]))<<4)+ (ChartoHex(src[i * 2+1])&0x0F);
	}
	

}


//void AppendEditText(HWND hWndEdit, LPCSTR pszFormat, ...)
//{
//	CHAR szText[4096] = { 0 };
//	va_list vlArgs;
//	va_start(vlArgs, pszFormat);
//	_vsnprintf(szText, sizeof(szText) - 1, pszFormat, vlArgs);
//	va_end(vlArgs);
//	lstrcatA(szText, "\r\n");
//	//以下两条语句为在edit中追加字符串
//	SendMessageA(hWndEdit, EM_SETSEL, -2, -1);
//	SendMessageA(hWndEdit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szText);
//	//设置滚轮到末尾，这样就可以看到最新信息
//	SendMessageA(hWndEdit, WM_VSCROLL, SB_BOTTOM, 0);
//}
//void settexttocontrol(HWND hwnd,char text[],int type)
//{
//	
//	if (type == 1)
//	{
//		int len;
//
//
//
//		size_t textlen;
//		textlen = strlen(text);
//		WCHAR * Wstrtext = (WCHAR*)malloc(sizeof(WCHAR) * textlen);
//		Wstrtext = chartowchar(text);
//
//
//
//		len = GetWindowTextLengthA(hwnd);
//		if (len != 0)
//		{
//			WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (len + textlen + 9));
//			GetWindowText(hwnd, str, len);
//			//chartowchar(text);
//			wcscat_s(str, len + textlen + 3, L"\r\n ");
//			wcscat_s(str, len + textlen + 7, Wstrtext);
//			//wcscat_s(str, len + textlen + 9, L"\0");
//			//SetWindowText(hwnd, str);
//			SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str);
//			free(str);
//		}
//		else
//		{
//			//WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (len + textlen + 5));
//			//GetWindowText(hwnd, str, len);
//			//chartowchar(text);
//			//wcscat_s(str, len + textlen + 1, L"\r\n");
//			//wcscat_s(str, len + textlen + 5, Wstrtext);
//			//SetWindowText(hwnd, Wstrtext);
//			SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)Wstrtext);
//		}
//		
//	}
//	else
//	{
//		SetWindowText(hwnd, chartowchar(text));
//	}
//}
void settexttocontrol(HWND hwnd, char text[], int type)
{
	int textlen=GetWindowTextLengthA(hwnd);
	SendMessageA(hwnd, EM_SETSEL, (WPARAM)textlen, (LPARAM)textlen);
	if (type == 1)//换行
	{
		int len = GetWindowTextLengthA(hwnd);
		if (len > 0)
		{
			SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"\r\n");
		}
		//wcscat_s(text, strlen(text) + 3, "\r\n");
		
		SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)text);
	}
	else if(type == 0)//不换行
	{
		SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)text);
	}

	
}
void setHEXtocontrol(HWND hwnd, uint16_t hex, int type)
{
	int textlen = GetWindowTextLengthA(hwnd);
	SendMessageA(hwnd, EM_SETSEL, (WPARAM)textlen, (LPARAM)textlen);

	char text[128];
	snprintf(text, 5, "%3.2X", hex);
	if (type == 1)//换行
	{
		//wcscat_s(text, strlen(text) + 3, "\r\n");
		int len = GetWindowTextLengthA(hwnd);
		if (len > 0)
		{
			SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"\r\n");
		}
		SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)text);
		
	}
	else if (type == 0)//不换行
	{
		SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)text);
	}
}
void setHEXDatatocontrol(HWND hwnd, uint8_t hex[], int length,int type)
{
	int textlen = GetWindowTextLengthA(hwnd);
	SendMessageA(hwnd, EM_SETSEL, (WPARAM)textlen, (LPARAM)textlen);

	char text[128];
	int i;
	if (type == 1)//换行
	{
		int len = GetWindowTextLengthA(hwnd);
		if (len > 0)
		{
			SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"\r\n");
		}
		for (i = 0; i < length; i++)
		{
			setHEXtocontrol(hwnd, hex[i], 0);
		}
		

	}
	else if (type == 0)//不换行
	{
		for (i = 0; i < length; i++)
		{
			//snprintf(text, 5, "%3.2X", hex[i]);
			//wcscat_s(text, strlen(text) + 3, "\r\n");
			//SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"t_t\r\n");

			//SendMessageA(hwnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)text);
			setHEXtocontrol(hwnd, hex[i], 0);
		}
	}
}


void gettextwithoutspace(HWND hwnd, char *text)
{
	WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (128));
	//char temp[128];
	int len = GetWindowTextLengthA(Text_in);

	//SendMessageA(Text_in, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)" ");
	//MessageBox(hWnd, L"空格", L"提示", MB_OK);

	GetWindowText(Text_in, str, len + 1);
	Wchar2Char(text, str);

	int i = 0, j = 0;
	for (i = 0; i < strlen(text); i++) {
		if (text[i] != ' ') {
			text[j++] = text[i];
		}
		else if (text[i] == ' ') {
			continue;
		}
	}
	text[j] = '\0';


}








//void setHEXtocontrol(HWND hwnd, uint16_t hex, int type)
//{
//	char hexchar[5];
//	snprintf(hexchar, 5, "%3.2X", hex);
//	if (type == 1)
//	{
//		int len;
//		
//		WCHAR * Wstrtext = (WCHAR*)malloc(sizeof(WCHAR) * 5);
//		Wstrtext = chartowchar(hexchar);
//
//
//
//		len = GetWindowTextLengthA(hwnd);
//		if (len != 0)
//		{
//			WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (len + 5 + 7));
//			GetWindowText(hwnd, str, len);
//			wcscat_s(str, len + 2, L" ");
//			//chartowchar(text);
//			//wcscat_s(str, len + 5 + 2, L"\r");
//			wcscat_s(str, len + 5 + 7, Wstrtext);
//			SetWindowText(hwnd, str);
//			free(str);
//		}
//		else
//		{
//			//WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (len + textlen + 5));
//			//GetWindowText(hwnd, str, len);
//			//chartowchar(text);
//			//wcscat_s(str, len + textlen + 1, L"\r\n");
//			//wcscat_s(str, len + textlen + 5, Wstrtext);
//			SetWindowText(hwnd, Wstrtext);
//		}
//
//	}
//	else
//	{
//		SetWindowText(hwnd, chartowchar(hexchar));
//	}
//}


//自定义的窗口过程
LRESULT  CALLBACK    MyWindowProc(HWND hWnd, UINT Msg,
	WPARAM wParam, LPARAM lParam)
{
	char temp_text[1024];
	HDC hdc;
	switch (Msg)
	{

	case WM_DESTROY:
		PostQuitMessage(0);//可以使GetMessage返回0    
		return 0;

	case WM_MOUSEWHEEL:

	case WM_VSCROLL:
		
		switch (LOWORD(wParam))
		{
		case SB_LEFT:

		default:
			break;
		}
		

	case  WM_CREATE:
	{
		
		


		//创建顶层菜单
		HMENU  hTop = CreateMenu();//空菜单
		
		InsertMenu(hTop, 0, MF_STRING | MF_BYPOSITION, 200, L"工具"); //添加第二项

																	//创建弹出菜单(子菜单)
		HMENU  hPopUp = CreatePopupMenu();//空菜单 
		AppendMenu(hPopUp, MF_SEPARATOR, NULL, NULL);//分隔条
		AppendMenu(hPopUp, MF_STRING, 301, L"打开");
		InsertMenu(hPopUp, 0, MF_STRING | MF_BYPOSITION, 302, L"新建");

		//添加第三项
		AppendMenu(hTop, MF_POPUP, (UINT_PTR)hPopUp, L"打开2");

		AppendMenu(hTop, MF_STRING, 400, L"关于"); //添加第一项
		AppendMenu(hTop, MF_STRING, 100, L"ReleaseLog"); //添加第一项
		//InsertMenu(hTop, 0, MF_STRING | MF_BYPOSITION, 400, L"关于"); //添加第二项

		//设置窗口菜单
		SetMenu(hWnd, hTop);

		//释放菜单资源
		DestroyMenu(hTop);
		DestroyMenu(hPopUp);






		


	}break;

	case WM_COMMAND:
	{
		//菜单ID
		WORD  menuId = LOWORD(wParam);
		switch (menuId)
		{
		case 100:
			MessageBox(hWnd, ReleaseLog, L"提示", MB_OK);
			break;
		case 200:
			MessageBox(hWnd, L"工具菜单点击", L"提示", MB_OK);
			break;
		case 301:
			MessageBox(hWnd, L"打开菜单点击", L"提示", MB_OK);
			break;
		case 302:
			MessageBox(hWnd, L"新建菜单点击", L"提示", MB_OK);
			break;
		case 400:
		{
			//hdc = GetDC(Text_out);
			//TextOut(hdc, 0, 0, TEXT("向上"), 9);
			MessageBox(hWnd, about, L"关于", MB_OK);
			break;
		}
			
		
		}
		

		switch (LOWORD(wParam))
		{
		case 2001:
		{
			xlStatus = demoInitDriver(&xlChanMaskTx, &xlChanIndex);
			//printf("- Init             : %s\n", xlGetErrorString(xlStatus));
			
			


			if (XL_SUCCESS == xlStatus) {
				// ------------------------------------
				// create the RX thread to read the
				// messages
				// ------------------------------------
				xlStatus = demoCreateRxThread();

				while (SendMessage(ComboBox_4001, CB_DELETESTRING, 0, 0) > 0)
				{
					//g_xlDrvConfig.channel[xlChanIndex].channelIndex, g_xlDrvConfig.channel[xlChanIndex].name, xlChanMaskTx);
				}




				for (int i = 0; i < g_xlDrvConfig.channelCount; i++) {
					snprintf(temp_text,128,"- Ch:%02d, CM:0x%03I64x,",
						g_xlDrvConfig.channel[i].channelIndex, g_xlDrvConfig.channel[i].channelMask);
					//settexttocontrol(Info_out, temp_text, 1);
					strncat_s(temp_text,100, g_xlDrvConfig.channel[i].name, 26);
					strncat_s(temp_text, 100, " ", 2);
					//printf(" %23s,", str);
					//settexttocontrol(Info_out, temp_text, 1);
					//memset(str, 0, sizeof(str));

					if (g_xlDrvConfig.channel[i].transceiverType != XL_TRANSCEIVER_TYPE_NONE) {
						//strncat_s(temp_text, 300, g_xlDrvConfig.channel[i].transceiverName, 30);
						//strncat_s(temp_text, 300, g_xlDrvConfig.channel[i].hwType, 30);
						//printf("%13s -\n", str);
						settexttocontrol(Info_out, temp_text, 1);
					}
					//else printf("    no Cab!   -\n");



					WCHAR test[128];
					char channel_name[128];
					snprintf(channel_name, 128, g_xlDrvConfig.channel[i].name);
					strncat_s(channel_name, 128, g_xlDrvConfig.channel[i].transceiverName, 26);
					//settexttocontrol(debug_info, g_xlDrvConfig.channel[i].name, 1);
					Char2Wchar(test, channel_name);
					SendMessage(ComboBox_4001, CB_ADDSTRING, 0, (LPARAM)test);
					
					



				}




				//printf("- Create RX thread : %s\n", xlGetErrorString(xlStatus));
			}
		
			if (XL_SUCCESS == xlStatus) {
				// ------------------------------------
				// go with all selected channels on bus
				// ------------------------------------
				xlStatus = xlActivateChannel(g_xlPortHandle, g_xlChannelMask, XL_BUS_TYPE_CAN, XL_ACTIVATE_RESET_CLOCK);
				//printf("- ActivateChannel  : CM=0x%I64x, %s\n", g_xlChannelMask, xlGetErrorString(xlStatus));
				if (xlStatus == XL_SUCCESS) activated = 1;
				
				settexttocontrol(Info_out, "Init success",1);
				memset(temp_text, 0, sizeof(temp_text));
				snprintf(temp_text, 128, "ActivateChannel  : CM = 0x%I64x", g_xlChannelMask);
				settexttocontrol(Info_out, temp_text, 1);

			}



			if (initDIAG("./Config/ECUlist.ini", &gDiag_info) == -1)
			{
				MessageBox(hWnd, L"未添加配置文件", L"提示", MB_OK);
			}
			while (SendMessage(ComboBox_4000, CB_DELETESTRING, 0, 0) > 0)
			{

			}
			for (int i = 0; i < gDiag_info.ECU_num; i++)
			{
				WCHAR test[128];
				Char2Wchar(test, gDiag_info.ECU_list[i].ECU_name);
				SendMessage(ComboBox_4000, CB_ADDSTRING, 0, (LPARAM)test);
			}
			


			//for (int i = 0; i < gDiag_info.ECU_num; i++)
			//{
			//	//printf("----------------\n");
			//	//printf("Name:%s\n0x%s\n0x%s\n0x%s\n", Diag_info.ECU_list[i].ECU_name, Diag_info.ECU_list[i].DIAG_REQ_PHY_ID, Diag_info.ECU_list[i].DIAG_RESP_ID, Diag_info.ECU_list[i].DIAG_REQ_FUNC_ID);
			//	settexttocontrol(Text_out, "-------------------", 1);
			//	settexttocontrol(Text_out, "Name:", 1);
			//	settexttocontrol(Text_out, gDiag_info.ECU_list[i].ECU_name, 0);
			//	settexttocontrol(Text_out, gDiag_info.ECU_list[i].DIAG_REQ_PHY_ID, 1);
			//	settexttocontrol(Text_out, gDiag_info.ECU_list[i].DIAG_RESP_ID, 1);
			//	settexttocontrol(Text_out, gDiag_info.ECU_list[i].DIAG_REQ_FUNC_ID, 1);
			//	settexttocontrol(Text_out, "DID_num:", 1);
			//	
			//	setHEXtocontrol(Text_out, gDiag_info.ECU_DID[i].DID_num, 0);
			//	uint8_t data[128];
			//	for (int n = 0; n < gDiag_info.ECU_DID[i].DID_num; n++)
			//	{
			//		//printf("DID_num=%d\n%s:%s\n", Diag_info.ECU_DID[i].DID_num, Diag_info.ECU_DID[i].DID_list[n].did_name, Diag_info.ECU_DID[i].DID_list[n].did);
			//		
			//		settexttocontrol(Text_out, gDiag_info.ECU_DID[i].DID_list[n].did_name, 1);
			//		settexttocontrol(Text_out, ": ", 0);
			//		settexttocontrol(Text_out, gDiag_info.ECU_DID[i].DID_list[n].did, 0);

			//		char_to_hex(gDiag_info.ECU_DID[i].DID_list[n].did, 6, data);
			//		settexttocontrol(Text_out, ": ", 0);
			//		setHEXDatatocontrol(Text_out, data, 3, 0);


			//	}
			//}
		
			break;
		
		}
		
		case 2003:
		{
			if (SendMessage(button_2003, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				//MessageBox(hWnd, L"被选中！", L"提示", MB_OK);
				//if(g_canFdSupport==1)
				
				if (g_canFdSupport == 1 && gCANFDBus_flag==1)
				{
					gSendMsgByCANFD_flag = 1;
				}
				else
				{
					MessageBox(hWnd, L"不支持CANFD！", L"提示", MB_OK);
					SendMessage(button_2003, BM_SETCHECK, BST_UNCHECKED, 0);

				}



			}
			else  if (SendMessage(button_2003, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			{
				//MessageBox(hWnd, L"没选中！", L"提示", MB_OK);
				gSendMsgByCANFD_flag = 0;
				
			}

			break;
		}
		case 2004:
		{
			if (SendMessage(button_2004, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				MessageBox(hWnd, L"被选中！", L"提示", MB_OK);
				gCANFDBus_flag = 1;

				



			}
			else  if (SendMessage(button_2004, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			{
				//MessageBox(hWnd, L"没选中！", L"提示", MB_OK);
				gCANFDBus_flag = 0;

			}

			break;
		}
		case 2005:
		{
			
			demoCleanUp();
			MessageBox(hWnd, L"Stop！", L"提示", MB_OK);
			break;
		}

		


		case 2007:
		{
			//WCHAR dllname[128] = L"SeednKeyExOpt.dll";
			//HINSTANCE handle = LoadLibrary(_T("SeednKeyExOpt.dll"));//LoadLibrary填入ddl文件名赋值给句柄
			//HINSTANCE handle = LoadLibrary(dllname);//LoadLibrary填入ddl文件名赋值给句柄
			//typedef int(*DLL_FUNCTION_GenerateKeyEx) (const unsigned char*, unsigned int, const unsigned int, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。
			//DLL_FUNCTION_GenerateKeyEx dll_GenerateKeyEx = (DLL_FUNCTION_GenerateKeyEx)GetProcAddress(handle, "GenerateKeyEx"); //使用GetProcAddress得到函数，参数是句柄名和函数名
			//printf("Loading...\n");
			//if (dll_GenerateKeyEx) //还是判断一下函数指针是否有效
			//{
			//	const unsigned char ipSeedArray[4] = { 0x1,0x2,0x3,0x4 };
			//	unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
			//	const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
			//	const char iVariant = 1;   /* Name of the active variant [in] */
			////const char ipOptions = 1;
			//	unsigned char ioKeyArray[4];     /* Array for the key [in, out] */
			//	unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
			//	unsigned int oSize = 0;

			//	//printf("SeednKeyEx\n");
			//	dll_GenerateKeyEx(ipSeedArray,                   iSeedArraySize,                   iSecurityLevel,                 &iVariant, ioKeyArray,                         iKeyArraySize, &oSize);
			//dll_GenerateKeyEx(  SecurityInfo[ECU_Choose].Seed, SecurityInfo[ECU_Choose].SeedLen, SecurityInfo[ECU_Choose].level, &iVariant, SecurityInfo[ECU_Choose].outputkey, iKeyArraySize, &oSize);
			//	//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
			//	//printf("iSeedArraySize=%d\n", iSeedArraySize);
			//	//printf("iSecurityLevel=%d\n", iSecurityLevel);
			//	//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

			//	//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
			//	//printf("oSize=%d\n", oSize);
			//	FreeLibrary(handle); //卸载句柄，，
			//	setHEXDatatocontrol(debug_info, ioKeyArray, oSize, 1);
			//	settexttocontrol(debug_info, "SeednKeyEx", 1);
			//}
			//else
			//{
			//	//HINSTANCE handle1 = LoadLibrary(_T("SeednKeyEx.dll"));//LoadLibrary填入ddl文件名赋值给句柄
			//	typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (const unsigned char*, unsigned int, const unsigned int, const char*, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。

			//	DLL_FUNCTION_GenerateKeyExOpt dll_GenerateKeyExOpt = (DLL_FUNCTION_GenerateKeyExOpt)GetProcAddress(handle, "GenerateKeyExOpt"); //使用GetProcAddress得到函数，参数是句柄名和函数名

			//	if (dll_GenerateKeyExOpt) //还是判断一下函数指针是否有效
			//	{
			//		const unsigned char  ipSeedArray[4] = { 0x1,0x2,0x3,0x4 };
			//		unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
			//		const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
			//		const char           iVariant = 1;   /* Name of the active variant [in] */
			//		const char ipOptions = 1;
			//		unsigned char        ioKeyArray[4];     /* Array for the key [in, out] */
			//		unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
			//		unsigned int         oSize = 0;

			//		//printf("SeednKeyExOpt\n");
			//		dll_GenerateKeyExOpt(ipSeedArray, iSeedArraySize, iSecurityLevel, &iVariant, &ipOptions, ioKeyArray, iKeyArraySize, &oSize);
			//		//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
			//		//printf("iSeedArraySize=%d\n", iSeedArraySize);
			//		//printf("iSecurityLevel=%d\n", iSecurityLevel);
			//		//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

			//		//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
			//		//printf("oSize=%d\n", oSize);
			//		FreeLibrary(handle); //卸载句柄，，
			//		setHEXDatatocontrol(debug_info, ioKeyArray, oSize, 1);
			//		settexttocontrol(debug_info, "SeednKeyExOpt", 1);
			//	}
			//}


			char dllname_char[128];
			snprintf(dllname_char, 22, "./SecurityAccessDLL/");
			strncat_s(dllname_char, 128, gDiag_info.ECU_list[ECU_Choose].SecurityAccessDLL, 128);
			Char2Wchar(dllname, dllname_char);
			settexttocontrol(debug_info, dllname_char, 1);
			HINSTANCE handle = LoadLibrary(dllname);//LoadLibrary填入ddl文件名赋值给句柄
			//typedef int(*DLL_FUNCTION_GenerateKeyEx) (const unsigned char*, unsigned int, const unsigned int, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。
			DLL_FUNCTION_GenerateKeyEx dll_GenerateKeyEx = (DLL_FUNCTION_GenerateKeyEx)GetProcAddress(handle, "GenerateKeyEx"); //使用GetProcAddress得到函数，参数是句柄名和函数名
			//printf("Loading...\n");
			if (dll_GenerateKeyEx) //还是判断一下函数指针是否有效
			{
				const unsigned char ipSeedArray[4];
				unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
				const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
				const char iVariant = 1;   /* Name of the active variant [in] */
			//const char ipOptions = 1;
				unsigned char ioKeyArray[4];     /* Array for the key [in, out] */
				unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
				unsigned int oSize = 0;
				ECU_Choose = 0;
				//printf("SeednKeyEx\n");
				SecurityInfo[ECU_Choose].SeedLen = 4;
				SecurityInfo[ECU_Choose].Seed[0] = 1;
				SecurityInfo[ECU_Choose].Seed[0] = 2;
				SecurityInfo[ECU_Choose].Seed[0] = 3;
				SecurityInfo[ECU_Choose].Seed[0] = 4;
				dll_GenerateKeyEx(SecurityInfo[ECU_Choose].Seed, SecurityInfo[ECU_Choose].SeedLen, SecurityInfo[ECU_Choose].level, &iVariant, SecurityInfo[ECU_Choose].outputkey, iKeyArraySize, &SecurityInfo[ECU_Choose].outputkeylen);
				//dll_GenerateKeyEx(ipSeedArray, iSeedArraySize, iSecurityLevel, &iVariant, ioKeyArray, iKeyArraySize, &oSize);
				//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
				//printf("iSeedArraySize=%d\n", iSeedArraySize);
				//printf("iSecurityLevel=%d\n", iSecurityLevel);
				//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

				//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
				//printf("oSize=%d\n", oSize);
				FreeLibrary(handle); //卸载句柄，，
				//setHEXDatatocontrol(debug_info, ioKeyArray, oSize, 1);
				setHEXDatatocontrol(debug_info, SecurityInfo[ECU_Choose].outputkey, SecurityInfo[ECU_Choose].outputkeylen, 1);
				settexttocontrol(debug_info, "SeednKeyEx", 1);
			}
			else
			{
				//HINSTANCE handle1 = LoadLibrary(_T("SeednKeyEx.dll"));//LoadLibrary填入ddl文件名赋值给句柄
				//typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (const unsigned char*, unsigned int, const unsigned int, const char*, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。

				DLL_FUNCTION_GenerateKeyExOpt dll_GenerateKeyExOpt = (DLL_FUNCTION_GenerateKeyExOpt)GetProcAddress(handle, "GenerateKeyExOpt"); //使用GetProcAddress得到函数，参数是句柄名和函数名

				if (dll_GenerateKeyExOpt) //还是判断一下函数指针是否有效
				{
					const unsigned char  ipSeedArray[4] = { 0x1,0x2,0x3,0x4 };
					unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
					const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
					const char           iVariant = 1;   /* Name of the active variant [in] */
					const char ipOptions = 1;
					unsigned char        ioKeyArray[4];     /* Array for the key [in, out] */
					unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
					unsigned int         oSize = 0;

					//printf("SeednKeyExOpt\n");
					dll_GenerateKeyExOpt(SecurityInfo[ECU_Choose].Seed, SecurityInfo[ECU_Choose].SeedLen, SecurityInfo[ECU_Choose].level, &iVariant, &ipOptions, SecurityInfo[ECU_Choose].outputkey, iKeyArraySize, &(SecurityInfo[ECU_Choose].outputkeylen));
					//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
					//printf("iSeedArraySize=%d\n", iSeedArraySize);
					//printf("iSecurityLevel=%d\n", iSecurityLevel);
					//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

					//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
					//printf("oSize=%d\n", oSize);
					FreeLibrary(handle); //卸载句柄，，
					setHEXDatatocontrol(debug_info, SecurityInfo[ECU_Choose].outputkey, SecurityInfo[ECU_Choose].outputkeylen, 1);
					settexttocontrol(debug_info, "SeednKeyExOpt", 1);
				}
			}




			
			break;
		}
		case 2008:
		{

			const unsigned char  ipSeedArray[4] = { 0x1,0x2,0x3,0x4 };
			unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
			const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
			const char           iVariant = 1;   /* Name of the active variant [in] */
			const char ipOptions = 1;
			unsigned char        ioKeyArray[4];     /* Array for the key [in, out] */
			unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
			unsigned int         oSize = 0;

			//printf("SeednKeyExOpt\n");
			//dll_GenerateKeyExOpt(ipSeedArray, iSeedArraySize, iSecurityLevel, &iVariant, &ipOptions, ioKeyArray, iKeyArraySize, &oSize);
			//setHEXDatatocontrol(debug_info, ioKeyArray, oSize, 1);
			break;
		}
		case 2006:
		{
			//xlChanMaskTx = g_xlDrvConfig.channel[2].channelMask;
			if (g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID == 0)
			{
				MessageBox(hWnd, L"请选择ECU", L"提示", MB_OK);
				return;
			}
			if (g_diaginfo[ECU_Choose].DIAG_RESP_ID == 0)
			{
				MessageBox(hWnd, L"请选择ECU", L"提示", MB_OK);
				return;
			}
			

			//WCHAR * str = (WCHAR*)malloc(sizeof(WCHAR) * (128));
			//int len = GetWindowTextLengthA(Text_in);
			char text[128];
			gettextwithoutspace(Text_in, text);
			int len = strlen(text);
			if ((len % 2) == 0)
			{
				//SendMessageA(Text_in, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)" ");
				//MessageBox(hWnd, L"空格", L"提示", MB_OK);

				//GetWindowText(Text_in, str, len + 1);
				
				//Wchar2Char(text, str);
				//uint8_t data[128];
				//uint8_t udsdata[128];
				char_to_hex(text, len, g_diaginfo[ECU_Choose].TX_Original_Data);
				g_diaginfo[ECU_Choose].TX_DataLength = len / 2;
				DataToUDSData(g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, g_diaginfo[ECU_Choose].TX_Data);
				//setHEXDatatocontrol(debug_info, g_diaginfo[ECU_Choose].TX_Data, g_diaginfo[ECU_Choose].TX_DataLength + 2, 1);
				g_diaginfo[ECU_Choose].TX_N_PCItype = (g_diaginfo[ECU_Choose].TX_Data[0] >> 4) & 0x0F;
				if (g_diaginfo[ECU_Choose].TX_N_PCItype == FF_N_PDU)
				{
					g_diaginfo[ECU_Choose].TX_flag = 1;
					CalculateFramesSenquence(g_diaginfo[ECU_Choose].TX_Data, &g_diaginfo[ECU_Choose].TX_DataLength, &g_diaginfo[ECU_Choose].TX_FrameNum);
					//setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].TX_FrameNum, 1);
				}


				if (1)
				{
					//OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, g_diaginfo[ECU_Choose].TX_Data);
					canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;
					canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
					canTxEvt_win32.tagData.canMsg.dlc = 8;
					for (int n = 0; n < 8; n++)
					{
						canTxEvt_win32.tagData.canMsg.data[n] = g_diaginfo[ECU_Choose].TX_Data[n];
					}
					//setHEXDatatocontrol(debug_info, canTxEvt_win32.tagData.canMsg.data, g_diaginfo[ECU_Choose].TX_DataLength + 2, 1);
					demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);
					//setHEXtocontrol(debug_info, xlChanMaskTx, 1);



				}
				else
				{
					canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;
					canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
					canTxEvt_win32.tagData.canMsg.dlc = 8;
					for (int n = 0; n < 8; n++)
					{
						canTxEvt_win32.tagData.canMsg.data[n] = g_diaginfo[ECU_Choose].TX_Data[n];
					}

					demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_FUNC_ID, xlChanMaskTx);
				}
				//OutFrame(0x725,g_diaginfo[ECU_Choose].TX_Data);
				//write("g_diaginfo[%d].DIAG_REQ_PHY_ID=%x",ChooseDIAG,g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID);

				//write("g_diaginfo[ECU_Choose].TX_DataLength=%d data %x %x",g_diaginfo[ECU_Choose].TX_DataLength,g_diaginfo[ECU_Choose].TX_Data[0],g_diaginfo[ECU_Choose].TX_Data[1]);

				if (g_diaginfo[ECU_Choose].TX_N_PCItype == SF_N_PDU)
				{
					//PrintHexToControl("TX：", g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, "DIAG", "DiagDisaply");
				}




				if (len / 2 < 8)
				{
					//setHEXDatatocontrol(debug_info, g_diaginfo[ECU_Choose].TX_Data, len / 2 + 1, 1);

					settexttocontrol(Text_out, "TX:", 1);
					setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].TX_Original_Data, len / 2, 0);

				}
				

				SendMessageA(Text_out, WM_VSCROLL, SB_BOTTOM, 0);//设置滚轮到末尾，这样就可以看到最新信息

			}
			else
			{
				MessageBox(hWnd, L"数据长度错误", L"提示", MB_OK);
			}

		}

		case 1056:
		{
			//MessageBox(hWnd, L"ttt", L"提示", MB_OK);
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
				{
					int len = GetWindowTextLengthA(Text_in);
					if ((len % 2)==0)
					{
						//SendMessageA(Text_in, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)" ");
						//MessageBox(hWnd, L"空格", L"提示", MB_OK);
					}
					else
					{
						//MessageBox(hWnd, L"数据长度错误", L"提示", MB_OK);
					}

				}
				default:
					break;
			}
		}

		case 3000:
		{
			//MessageBox(hWnd, L"列表---", L"提示", MB_OK);
			//MessageBox(hWnd, L"ttt", L"提示", MB_OK);
			int index=0;
			switch (HIWORD(wParam))
			{
			
			case LBN_SELCHANGE:
			{
				uint8_t getdid[128];
				// 获取当前选择的索引
				index = SendMessage(ListBox_3000, LB_GETCURSEL, 0, 0);

				//WCHAR temp[128];
				//Char2Wchar(temp, gDiag_info.ECU_DID[ECU_Choose].DID_list[index].did);
				//SendMessage(ListBox_3000, LB_ADDSTRING, 0, (LPARAM)temp);
				//setHEXtocontrol(debug_info, index, 1);

				g_diaginfo[ECU_Choose].TX_DataLength = strlen(gDiag_info.ECU_DID[ECU_Choose].DID_list[index].did) / 2;
				char_to_hex(gDiag_info.ECU_DID[ECU_Choose].DID_list[index].did, g_diaginfo[ECU_Choose].TX_DataLength*2, g_diaginfo[ECU_Choose].TX_Original_Data);


				if (g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID == 0)
				{
					MessageBox(hWnd, L"请选择ECU", L"提示", MB_OK);
					return;
				}
				if (g_diaginfo[ECU_Choose].DIAG_RESP_ID == 0)
				{
					MessageBox(hWnd, L"请选择ECU", L"提示", MB_OK);
					return;
				}


				
				DataToUDSData(g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, g_diaginfo[ECU_Choose].TX_Data);

				g_diaginfo[ECU_Choose].TX_N_PCItype = (g_diaginfo[ECU_Choose].TX_Data[0] >> 4) & 0x0F;
				if (g_diaginfo[ECU_Choose].TX_N_PCItype == FF_N_PDU)
				{
					g_diaginfo[ECU_Choose].TX_flag = 1;
					CalculateFramesSenquence(g_diaginfo[ECU_Choose].TX_Data, &g_diaginfo[ECU_Choose].TX_DataLength, &g_diaginfo[ECU_Choose].TX_FrameNum);
					//setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].TX_FrameNum, 1);
				}


				if (1)
				{
					//OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, g_diaginfo[ECU_Choose].TX_Data);
					canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;
					canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
					canTxEvt_win32.tagData.canMsg.dlc = 8;
					for (int n = 0; n < 8; n++)
					{
						canTxEvt_win32.tagData.canMsg.data[n] = g_diaginfo[ECU_Choose].TX_Data[n];
					}

					demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);




				}
				else
				{
					canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;
					canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
					canTxEvt_win32.tagData.canMsg.dlc = 8;
					for (int n = 0; n < 8; n++)
					{
						canTxEvt_win32.tagData.canMsg.data[n] = g_diaginfo[ECU_Choose].TX_Data[n];
					}

					demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_FUNC_ID, xlChanMaskTx);
				}
				//OutFrame(0x725,g_diaginfo[ECU_Choose].TX_Data);
				//write("g_diaginfo[%d].DIAG_REQ_PHY_ID=%x",ChooseDIAG,g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID);

				//write("g_diaginfo[ECU_Choose].TX_DataLength=%d data %x %x",g_diaginfo[ECU_Choose].TX_DataLength,g_diaginfo[ECU_Choose].TX_Data[0],g_diaginfo[ECU_Choose].TX_Data[1]);

				if (g_diaginfo[ECU_Choose].TX_N_PCItype == SF_N_PDU)
				{
					//PrintHexToControl("TX：", g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, "DIAG", "DiagDisaply");
				}




				if (g_diaginfo[ECU_Choose].TX_DataLength < 8)
				{
					//setHEXDatatocontrol(debug_info, g_diaginfo[ECU_Choose].TX_Data, len / 2 + 1, 1);

					settexttocontrol(Text_out, "TX:", 1);
					setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, 0);

				}


				SendMessageA(Text_out, WM_VSCROLL, SB_BOTTOM, 0);//设置滚轮到末尾，这样就可以看到最新信息

				
				




				break;
			}
			
			default:
				break;
			}
		}

		case 4000:
		{
			//MessageBox(hWnd, L"列表---", L"提示", MB_OK);
			//MessageBox(hWnd, L"ttt", L"提示", MB_OK);
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
			{
				//HWND hCom = (HWND)lParam;
				int Sel = SendMessage(ComboBox_4000, CB_GETCURSEL, 0, 0);
				//demoStopTransmitBurst_3E();

				ECU_Choose = Sel;
				
				uint8_t DIAG_REQ_FUNC_ID[2], DIAG_REQ_PHY_ID[2],DIAG_RESP_ID[2];

				char_to_hex(gDiag_info.ECU_list[Sel].DIAG_REQ_FUNC_ID, 4, DIAG_REQ_FUNC_ID);
				char_to_hex(gDiag_info.ECU_list[Sel].DIAG_REQ_PHY_ID, 4, DIAG_REQ_PHY_ID);
				char_to_hex(gDiag_info.ECU_list[Sel].DIAG_RESP_ID, 4, DIAG_RESP_ID);


				g_diaginfo[Sel].DIAG_REQ_FUNC_ID = (DIAG_REQ_FUNC_ID[0] << 8) + DIAG_REQ_FUNC_ID[1];
				g_diaginfo[Sel].DIAG_REQ_PHY_ID = (DIAG_REQ_PHY_ID[0] << 8) + DIAG_REQ_PHY_ID[1];
				g_diaginfo[Sel].DIAG_RESP_ID = (DIAG_RESP_ID[0] << 8) + DIAG_RESP_ID[1];


				while (SendMessage(ListBox_3000, LB_DELETESTRING, 0, 0) > 0)
				{

				}

				for (int i = 0; i < gDiag_info.ECU_DID[Sel].DID_num; i++)

				{
					WCHAR temp[128];
					Char2Wchar(temp, gDiag_info.ECU_DID[Sel].DID_list[i].did_name);
					SendMessage(ListBox_3000, LB_ADDSTRING, 0, (LPARAM)temp);

				}

				break;


			}
			default:
				break;
			}
		}

		case 4001:
		{
			//MessageBox(hWnd, L"列表---", L"提示", MB_OK);
			//MessageBox(hWnd, L"ttt", L"提示", MB_OK);
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
			{
				//HWND hCom = (HWND)lParam;
				channel_choose = SendMessage(ComboBox_4001, CB_GETCURSEL, 0, 0);
				xlChanMaskTx = g_xlDrvConfig.channel[channel_choose].channelMask;
				//setHEXtocontrol(debug_info, channel_choose, 1);

				break;


			}
			default:
				break;
			}
		}
		
		default:
			break;
		}

	}  break;

	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}


int  WINAPI   WinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance,
	LPSTR lpCmdLine, int  nShowCmd)
{
	setlocale(LC_ALL, "");


	g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID = 0;
	g_diaginfo[ECU_Choose].DIAG_REQ_FUNC_ID = 0;
	g_diaginfo[ECU_Choose].DIAG_RESP_ID = 0;
	//DIAG_info Diag_info1;
	//struct DIAG_info Diag_info;
	





	//1.注册窗口类
	WNDCLASS  wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)(GetStockObject(DEFAULT_PALETTE));//背景色
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);//光标
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);//图标
												//wnd.lpfnWndProc = DefWindowProc;//默认窗口过程函数，用于处理消息
	wnd.lpfnWndProc = MyWindowProc;//自定义的窗口过程函数
	wnd.lpszClassName = L"MrHuang";//窗口类名
	wnd.lpszMenuName = NULL;//菜单资源名称
	wnd.style = CS_HREDRAW | CS_VREDRAW;//窗口类、样式
	wnd.hInstance = hInstance;//实例句柄
	
	RegisterClass(&wnd);

	//WNDCLASS  wnd1;
	//wnd1.cbClsExtra = 0;
	//wnd1.cbWndExtra = 0;
	//wnd1.hbrBackground = (HBRUSH)(GetStockObject(GRAY_BRUSH));//背景色
	//wnd1.hCursor = LoadCursor(NULL, IDC_ARROW);//光标
	//wnd1.hIcon = LoadIcon(NULL, IDI_APPLICATION);//图标
	//											//wnd.lpfnWndProc = DefWindowProc;//默认窗口过程函数，用于处理消息
	//wnd1.lpfnWndProc = MyWindowProc;//自定义的窗口过程函数
	//wnd1.lpszClassName = L"te";//窗口类名
	//wnd1.lpszMenuName = NULL;//菜单资源名称
	//wnd1.style = NULL;//窗口类、样式
	//wnd1.hInstance = hInstance;//实例句柄

	//RegisterClass(&wnd1);


	//创建窗口(返回之前发送WM_CREATE)
	GUI = CreateWindow(L"MrHuang", L"CANoe Demo",
		WS_OVERLAPPEDWINDOW, 100, 100, 1300, 800, NULL, NULL, hInstance, NULL);

	
	/*Text_out = CreateWindowExW(
		0, L"Edit", L"t", WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
		ES_MULTILINE  , 10, 150,
		800, 300, GUI, (HMENU)1010, GetModuleHandle(NULL), NULL
	);*/

	LoadLibrary(L"Msftedit.dll");
	//setHEXtocontrol(debug_info, LoadLibrary(L"Msftedit.dll"), 1);
	
	Text_out = CreateWindow(MSFTEDIT_CLASS, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY| /*WS_DISABLED|*/
		ES_UPPERCASE|ES_AUTOVSCROLL, 10, 150, 800, 300, GUI, (HMENU)1010, hInstance, NULL);
	Text_in = CreateWindow(MSFTEDIT_CLASS, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL| ES_MULTILINE |
		ES_UPPERCASE | ES_AUTOVSCROLL, 70, 65, 200, 80, GUI, (HMENU)1056, hInstance, NULL);
	SendMessage(Text_in, EM_SETEVENTMASK, 0, ENM_CHANGE);
	debug_info = CreateWindow(MSFTEDIT_CLASS, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE |
		ES_UPPERCASE | ES_AUTOVSCROLL, 820, 150, 400, 300, GUI, (HMENU)1012, hInstance, NULL);

	Info_out = CreateWindow(MSFTEDIT_CLASS, NULL,
		WS_CHILD | ES_READONLY | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_UPPERCASE , 300, 10, 500, 130, GUI, (HMENU)1011, hInstance, NULL);
	
	HWND hTabCtel= CreateWindowExW(
		0, L"SysTabControl32", NULL, WS_CHILDWINDOW | WS_VISIBLE, 820, 10,
		300, 130, GUI, (HMENU)1012, GetModuleHandle(NULL), NULL
	);

	TCITEM tcItem = { 0 };
	tcItem.mask = TCIF_IMAGE | TCIF_TEXT;
	tcItem.pszText = (LPTSTR)TEXT("item1");
	tcItem.iImage = 0;
	SendMessage(hTabCtel, TCM_INSERTITEM, 1, (LPARAM)&tcItem);
	tcItem.pszText = (LPTSTR)TEXT("item2");
	tcItem.iImage = 0;
	SendMessage(hTabCtel, TCM_INSERTITEM, 2, (LPARAM)&tcItem);
	tcItem.pszText = (LPTSTR)TEXT("item3");
	tcItem.iImage = 0;
	SendMessage(hTabCtel, TCM_INSERTITEM, 3, (LPARAM)&tcItem);

	ListBox_3000= CreateWindowEx(0, TEXT("ListBox"), NULL, WS_VISIBLE | WS_VSCROLL| WS_CHILD | WS_BORDER | LBS_NOTIFY,

		10, 500, 200, 200, GUI, (HMENU)3000, hInstance, 0);
	
	
	ComboBox_4000 = CreateWindowEx(0, TEXT("comboBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL| CBS_DROPDOWNLIST ,

		250, 500, 200, 200, GUI, (HMENU)4000, GetModuleHandle(NULL), 0);

	ComboBox_4001 = CreateWindowEx(0, TEXT("comboBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,

		500, 500, 200, 250, GUI, (HMENU)4001, GetModuleHandle(NULL), 0);

	ID4001_text =CreateWindow(MSFTEDIT_CLASS, NULL,
		WS_CHILD | ES_READONLY | WS_VISIBLE | ES_MULTILINE | ES_UPPERCASE, 500, 470, 200, 30, GUI, (HMENU)14001, hInstance, NULL);

	settexttocontrol(ID4001_text, "channel:", 0);
	
	


	button_2001 = CreateWindowExW(
		0, L"button", L"init", WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON, 10, 10,
		50, 50, GUI, (HMENU)2001, GetModuleHandle(NULL), NULL
	);

	button_2005 = CreateWindowExW(
		0, L"button", L"stop", WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON, 10, 70,
		50, 50, GUI, (HMENU)2005, GetModuleHandle(NULL), NULL
	);

	
	button_2003 = CreateWindowExW(
		0, L"button", L"SendCANFDMsg", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 20,
		100, 10, GUI, (HMENU)2003, GetModuleHandle(NULL), NULL
	);

	button_2004 = CreateWindowExW(
		0, L"button", L"CANFDBus", WS_CHILDWINDOW | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 40,
		100, 10, GUI, (HMENU)2004, GetModuleHandle(NULL), NULL
	);

	


	SendMessage(button_2004, BM_SETCHECK, BST_CHECKED, 0);

	button_2006 = CreateWindowExW(
		0, L"button", L"send", WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON, 240, 10,
		50, 50, GUI, (HMENU)2006, GetModuleHandle(NULL), NULL
	);

	button_2007 = CreateWindowExW(
		0, L"button", L"loaddll", WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON, 800, 10,
		50, 50, GUI, (HMENU)2007, GetModuleHandle(NULL), NULL
	);
	
	button_2008 = CreateWindowExW(
		0, L"button", L"cacle", WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON, 900, 10,
		50, 50, GUI, (HMENU)2008, GetModuleHandle(NULL), NULL
	);
	




	//显示窗口
	ShowWindow(GUI, nShowCmd);
	//更新窗口(发送WM_PAINT)
	UpdateWindow(GUI);

	//消息循环（收到WM_QUIT消息退出）
	MSG  msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//翻译消息
		DispatchMessage(&msg);//分发消息到窗口过程
	}
}

////////////////////////////////////////////////////////////////////////////

//! TxThread

//! 
//!
////////////////////////////////////////////////////////////////////////////
DWORD WINAPI TxThread(LPVOID par)
{
	XLstatus      xlStatus = XL_SUCCESS;
	unsigned int  n = 1;
	XLcanTxEvent  canTxEvt;
	XLevent       xlEvent;
	unsigned int  cntSent;

	UNREFERENCED_PARAMETER(par);

	if (g_canFdSupport) {

		unsigned int i;

		memset(&canTxEvt, 0, sizeof(canTxEvt));
		canTxEvt.tag = XL_CAN_EV_TAG_TX_MSG;

		canTxEvt.tagData.canMsg.canId = g_TXThreadCanId;
		canTxEvt.tagData.canMsg.msgFlags = XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS;
		canTxEvt.tagData.canMsg.dlc = 15;

		for (i = 1; i < XL_CAN_MAX_DATA_LEN; ++i) {
			canTxEvt.tagData.canMsg.data[i] = (unsigned char)i - 1;
		}
	}
	else {

		memset(&xlEvent, 0, sizeof(xlEvent));

		xlEvent.tag = XL_TRANSMIT_MSG;
		xlEvent.tagData.msg.id = g_TXThreadCanId;
		xlEvent.tagData.msg.dlc = 8;
		xlEvent.tagData.msg.flags = 0;
		xlEvent.tagData.msg.data[0] = 1;
		xlEvent.tagData.msg.data[1] = 2;
		xlEvent.tagData.msg.data[2] = 3;
		xlEvent.tagData.msg.data[3] = 4;
		xlEvent.tagData.msg.data[4] = 5;
		xlEvent.tagData.msg.data[5] = 6;
		xlEvent.tagData.msg.data[6] = 7;
		xlEvent.tagData.msg.data[7] = 8;

	}

	while (g_TXThreadRun && XL_SUCCESS == xlStatus) {

		if (g_canFdSupport) {
			++canTxEvt.tagData.canMsg.data[0];
			xlStatus = xlCanTransmitEx(g_xlPortHandle, g_TXThreadTxMask, n, &cntSent, &canTxEvt);
		}
		else {
			++xlEvent.tagData.msg.data[0];
			xlStatus = xlCanTransmit(g_xlPortHandle, g_TXThreadTxMask, &n, &xlEvent);
		}

		Sleep(10);

	}

	if (XL_SUCCESS != xlStatus) {
		printf("Error xlCanTransmit:%s\n", xlGetErrorString(xlStatus));
	}

	g_TXThreadRun = 0;
	return NO_ERROR;
}



DWORD WINAPI TxThread_3E(LPVOID par)
{
	XLstatus      xlStatus = XL_SUCCESS;
	unsigned int  n = 1;
	XLcanTxEvent  canTxEvt;
	XLevent       xlEvent;
	unsigned int  cntSent;

	UNREFERENCED_PARAMETER(par);

	if (g_canFdSupport) {

		unsigned int i;

		memset(&canTxEvt, 0, sizeof(canTxEvt));
		canTxEvt.tag = XL_CAN_EV_TAG_TX_MSG;

		canTxEvt.tagData.canMsg.canId = g_TXThreadCanId_3E;
		canTxEvt.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
		canTxEvt.tagData.canMsg.dlc = 8;
		canTxEvt.tagData.canMsg.data[0] = 0x02;
		canTxEvt.tagData.canMsg.data[1] = 0x3e;
		canTxEvt.tagData.canMsg.data[2] = 0x80;

		//for (i = 1; i < XL_CAN_MAX_DATA_LEN; ++i) {
		//	canTxEvt.tagData.canMsg.data[i] = (unsigned char)i - 1;
		//}
	}
	else {

		memset(&xlEvent, 0, sizeof(xlEvent));

		xlEvent.tag = XL_TRANSMIT_MSG;
		xlEvent.tagData.msg.id = g_TXThreadCanId_3E;
		xlEvent.tagData.msg.dlc = 8;
		xlEvent.tagData.msg.flags = 0;
		xlEvent.tagData.msg.data[0] = 0x02;
		xlEvent.tagData.msg.data[1] = 0x3e;
		xlEvent.tagData.msg.data[2] = 0x80;
		//xlEvent.tagData.msg.data[3] = 4;
		//xlEvent.tagData.msg.data[4] = 5;
		//xlEvent.tagData.msg.data[5] = 6;
		//xlEvent.tagData.msg.data[6] = 7;
		//xlEvent.tagData.msg.data[7] = 8;

	}

	while (g_TXThreadRun_3E && XL_SUCCESS == xlStatus) {

		if (g_canFdSupport) {
			//++canTxEvt.tagData.canMsg.data[0];
			xlStatus = xlCanTransmitEx(g_xlPortHandle, g_TXThreadTxMask_3E, n, &cntSent, &canTxEvt);
		}
		else {
			//++xlEvent.tagData.msg.data[0];
			xlStatus = xlCanTransmit(g_xlPortHandle, g_TXThreadTxMask_3E, &n, &xlEvent);
		}

		Sleep(3300);

	}

	if (XL_SUCCESS != xlStatus) {
		printf("Error xlCanTransmit:%s\n", xlGetErrorString(xlStatus));
	}

	g_TXThreadRun_3E = 0;
	return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////

//! RxThread

//! thread to readout the message queue and parse the incoming messages
//!
////////////////////////////////////////////////////////////////////////////

DWORD WINAPI RxThread(LPVOID par)
{
	XLstatus        xlStatus;

	unsigned int    msgsrx = RECEIVE_EVENT_SIZE;
	XLevent         xlEvent;

	UNUSED_PARAM(par);

	g_RXThreadRun = 1;

	while (g_RXThreadRun) {

		WaitForSingleObject(g_hMsgEvent, 10);

		xlStatus = XL_SUCCESS;

		while (!xlStatus) {

			msgsrx = RECEIVE_EVENT_SIZE;

			xlStatus = xlReceive(g_xlPortHandle, &msgsrx, &xlEvent);
			if (xlStatus != XL_ERR_QUEUE_IS_EMPTY) {

				if (!g_silent) {
					printf("%s\n", xlGetEventString(&xlEvent));
				}

			}
		}

	}
	return NO_ERROR;
}

int rxflag = 0;

///////////////////////////////////////////////////////////////////////////

//! RxCANThread

//! thread to read the message queue and parse the incoming messages
//!
////////////////////////////////////////////////////////////////////////////
DWORD WINAPI RxCanFdThread(LPVOID par)
{
	XLstatus        xlStatus = XL_SUCCESS;
	DWORD           rc;
	XLcanRxEvent    xlCanRxEvt;

	UNUSED_PARAM(par);
	uint8_t data[128];
	g_RXCANThreadRun = 1;
	while (g_RXCANThreadRun) {
		rc = WaitForSingleObject(g_hMsgEvent, 10);
		if (rc != WAIT_OBJECT_0) continue;

		do {
			xlStatus = xlCanReceive(g_xlPortHandle, &xlCanRxEvt);
			if (xlStatus == XL_ERR_QUEUE_IS_EMPTY) {
				break;
			}
			
			if (!g_silent) 
			{
				
				

					//settexttocontrol(Text_out, "RX_test：", 1);
					//setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 13, 0);//Data


				//settexttocontrol(debug_info, "RX:\t", 1);
				//setHEXtocontrol(debug_info, xlCanRxEvt.tagData.canRxOkMsg.canId, 0);//ID
				//settexttocontrol(debug_info, "\t", 0);
				//for (int m = 0; m < 8; m++)
				//{
				//	data[m] = xlCanRxEvt.tagData.canRxOkMsg.data[m];
				//}
				//setHEXDatatocontrol(debug_info, data, 8, 0);//Data


				rxflag++;
				if (rxflag == 2)
				{
					rxflag = 0;
				}
				
				if (xlCanRxEvt.tagData.canRxOkMsg.canId == g_diaginfo[ECU_Choose].DIAG_RESP_ID && xlCanRxEvt.channelIndex == channel_choose /*&& rxflag==1*/)
				{
					int count, i, j;
					char RX_print_info[4096];
					char RX_canid[32];
					char RX_data[4000];
					//settexttocontrol(debug_info, "channel_choose:", 1);
					//setHEXtocontrol(debug_info, channel_choose, 0);
					//settexttocontrol(debug_info, "channelIndex:", 1);
					//setHEXtocontrol(debug_info, xlCanRxEvt.channelIndex, 0);

					//settexttocontrol(debug_info, "RX:\t", 1);
					//setHEXtocontrol(debug_info, xlCanRxEvt.tagData.canRxOkMsg.canId, 0);//ID
					//settexttocontrol(debug_info, "\t", 0);
					//for (int m = 0; m < 8; m++)
					//{
					//	data[m] = xlCanRxEvt.tagData.canRxOkMsg.data[m];
					//}
					//setHEXDatatocontrol(debug_info, data, 8, 0);//Data



















					g_diaginfo[ECU_Choose].RX_N_PCItype = (xlCanRxEvt.tagData.canRxOkMsg.data[0] >> 4) & 0x0F;//报文第一个字节的前4bit代表 报文类型
					if (g_diaginfo[ECU_Choose].RX_N_PCItype == SF_N_PDU)
					{
						if (g_diaginfo[ECU_Choose].RX_flag == 0 && g_diaginfo[ECU_Choose].TX_flag == 0)
						{
							g_diaginfo[ECU_Choose].RX_DataLength = xlCanRxEvt.tagData.canRxOkMsg.data[0] & 0x0F; //单帧低4 bit 是数据长度
							//setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].RX_DataLength, 1);
							settexttocontrol(Text_out, "RX:\t", 1);
							setHEXtocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.canId, 0);//ID
							settexttocontrol(Text_out, "\t", 0);
							//settexttocontrol(Text_out, "\n", 0);
							for (i = 0; i < g_diaginfo[ECU_Choose].RX_DataLength; i++)
							{
								g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_byte_counter] = xlCanRxEvt.tagData.canRxOkMsg.data[i + 1];
								//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_byte_counter], 0);//Data
								g_diaginfo[ECU_Choose].RX_byte_counter++;
							}
							//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_byte_counter, 1);
							//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data[0], 1);
							//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data[1], 0);
							//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data[2], 0);
							//setHEXtocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data[3], 0);

							//setHEXtocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.data[1], 1);
							//setHEXtocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.data[2], 0);
							//setHEXtocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.data[3], 0);
							//setHEXtocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.data[4], 0);

							//settexttocontrol(Text_out, "\t", 0);
							setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, g_diaginfo[ECU_Choose].RX_DataLength, 0);//Data
							g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_DataLength] = 0;
							
							//setHEXDatatocontrol(Text_out, xlCanRxEvt.tagData.canRxOkMsg.data, 8, 1);//Data
							SendMessageA(Text_out, WM_VSCROLL, SB_BOTTOM, 0);//设置滚轮到末尾，这样就可以看到最新信息
							g_diaginfo[ECU_Choose].RX_byte_counter = 0;

							





							if (g_diaginfo[ECU_Choose].RX_Data[0] == 0x67 && g_diaginfo[ECU_Choose].RX_Data[1] == g_diaginfo[ECU_Choose].TX_Original_Data[1])
							{
								
								if ((g_diaginfo[ECU_Choose].RX_Data[1] % 2) == 1)
								{
									SecurityInfo[ECU_Choose].level = g_diaginfo[ECU_Choose].TX_Original_Data[1];
									SecurityInfo[ECU_Choose].SeedLen = g_diaginfo[ECU_Choose].RX_DataLength - 2;
									for (int i = 0; i < SecurityInfo[ECU_Choose].SeedLen; i++)
									{
										SecurityInfo[ECU_Choose].Seed[i] = g_diaginfo[ECU_Choose].RX_Data[i+2];
									}


									char dllname_char[128];
									snprintf(dllname_char, 128, "./SecurityAccessDLL/");
									strncat_s(dllname_char, 128, gDiag_info.ECU_list[ECU_Choose].SecurityAccessDLL, 128);
									settexttocontrol(debug_info, dllname_char, 1);
									Char2Wchar(dllname, dllname_char);
									HINSTANCE handle = LoadLibrary(dllname);//LoadLibrary填入ddl文件名赋值给句柄
									settexttocontrol(debug_info, dllname_char, 1);
									//HINSTANCE handle = LoadLibrary(_T("./SecurityAccessDLL/BLE_Master_20201104.dll"));
									//settexttocontrol(debug_info, "dllname_char", 1);
									typedef int(*DLL_FUNCTION_GenerateKeyEx) (const unsigned char*, unsigned int, const unsigned int, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。
									DLL_FUNCTION_GenerateKeyEx dll_GenerateKeyEx = (DLL_FUNCTION_GenerateKeyEx)GetProcAddress(handle, "GenerateKeyEx"); //使用GetProcAddress得到函数，参数是句柄名和函数名
									//printf("Loading...\n");
									if (dll_GenerateKeyEx) //还是判断一下函数指针是否有效
									{
										const unsigned char ipSeedArray[4] ;
										unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
										const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
										const char iVariant = 1;   /* Name of the active variant [in] */
									//const char ipOptions = 1;
										unsigned char ioKeyArray[4];     /* Array for the key [in, out] */
										unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
										unsigned int oSize = 0;

										//printf("SeednKeyEx\n");
										dll_GenerateKeyEx(SecurityInfo[ECU_Choose].Seed, SecurityInfo[ECU_Choose].SeedLen, SecurityInfo[ECU_Choose].level, &iVariant, SecurityInfo[ECU_Choose].outputkey, iKeyArraySize, &SecurityInfo[ECU_Choose].outputkeylen);
										//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
										//printf("iSeedArraySize=%d\n", iSeedArraySize);
										//printf("iSecurityLevel=%d\n", iSecurityLevel);
										//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

										//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
										//printf("oSize=%d\n", oSize);
										FreeLibrary(handle); //卸载句柄，，
										
										setHEXDatatocontrol(debug_info, SecurityInfo[ECU_Choose].outputkey, SecurityInfo[ECU_Choose].outputkeylen, 1);
										settexttocontrol(debug_info, "SeednKeyEx", 1);
									}
									else
									{
										//HINSTANCE handle1 = LoadLibrary(_T("SeednKeyEx.dll"));//LoadLibrary填入ddl文件名赋值给句柄
										typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (const unsigned char*, unsigned int, const unsigned int, const char*, const char*, unsigned char*, unsigned int, unsigned int*); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。

										DLL_FUNCTION_GenerateKeyExOpt dll_GenerateKeyExOpt = (DLL_FUNCTION_GenerateKeyExOpt)GetProcAddress(handle, "GenerateKeyExOpt"); //使用GetProcAddress得到函数，参数是句柄名和函数名

										if (dll_GenerateKeyExOpt) //还是判断一下函数指针是否有效
										{
											const unsigned char  ipSeedArray[4] = { 0x1,0x2,0x3,0x4 };
											unsigned int          iSeedArraySize = 4; /* Length of the array for the seed [in] */
											const unsigned int    iSecurityLevel = 1;  /* Security level [in] */
											const char           iVariant = 1;   /* Name of the active variant [in] */
											const char ipOptions = 1;
											unsigned char        ioKeyArray[4];     /* Array for the key [in, out] */
											unsigned int          iKeyArraySize = 4;  /* Maximum length of the array for the key [in] */
											unsigned int         oSize = 0;

											//printf("SeednKeyExOpt\n");
											dll_GenerateKeyExOpt(SecurityInfo[ECU_Choose].Seed, SecurityInfo[ECU_Choose].SeedLen, SecurityInfo[ECU_Choose].level, &iVariant, &ipOptions, SecurityInfo[ECU_Choose].outputkey, iKeyArraySize, &(SecurityInfo[ECU_Choose].outputkeylen));
											//printf("ipSeedArray:%X %X %X %X\n", ipSeedArray[0], ipSeedArray[1], ipSeedArray[2], ipSeedArray[3]);
											//printf("iSeedArraySize=%d\n", iSeedArraySize);
											//printf("iSecurityLevel=%d\n", iSecurityLevel);
											//printf("ioKeyArray:%X %X %X %X\n", ioKeyArray[0], ioKeyArray[1], ioKeyArray[2], ioKeyArray[3]);

											//printf("EXP ioKeyArray:%X %X %X %X\n", ~ipSeedArray[0], ~ipSeedArray[1], ~ipSeedArray[2], ~ipSeedArray[3]);
											//printf("oSize=%d\n", oSize);
											FreeLibrary(handle); //卸载句柄，，
											setHEXDatatocontrol(debug_info, SecurityInfo[ECU_Choose].outputkey, SecurityInfo[ECU_Choose].outputkeylen, 1);
											settexttocontrol(debug_info, "SeednKeyExOpt", 1);
										}
									}

									//SecurityInfo[ECU_Choose].outputkeylen = 4;
									g_diaginfo[ECU_Choose].RX_byte_counter = 0;
									g_diaginfo[ECU_Choose].RX_DataLength = 0;


									Sleep(10);
									g_diaginfo[ECU_Choose].TX_DataLength = 2+ SecurityInfo[ECU_Choose].outputkeylen;
									//char_to_hex(gDiag_info.ECU_DID[ECU_Choose].DID_list[index].did, g_diaginfo[ECU_Choose].TX_DataLength * 2, g_diaginfo[ECU_Choose].TX_Original_Data);
									g_diaginfo[ECU_Choose].TX_Original_Data[0] = 0x27;
									g_diaginfo[ECU_Choose].TX_Original_Data[1] = g_diaginfo[ECU_Choose].TX_Original_Data[1]+1;
									
									for (int i = 0; i < g_diaginfo[ECU_Choose].TX_DataLength; i++)
									{
										g_diaginfo[ECU_Choose].TX_Original_Data[i + 2] = SecurityInfo[ECU_Choose].outputkey[i];
									}
									

									



									DataToUDSData(g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, g_diaginfo[ECU_Choose].TX_Data);

									g_diaginfo[ECU_Choose].TX_N_PCItype = (g_diaginfo[ECU_Choose].TX_Data[0] >> 4) & 0x0F;
									if (g_diaginfo[ECU_Choose].TX_N_PCItype == FF_N_PDU)
									{
										g_diaginfo[ECU_Choose].TX_flag = 1;
										CalculateFramesSenquence(g_diaginfo[ECU_Choose].TX_Data, &g_diaginfo[ECU_Choose].TX_DataLength, &g_diaginfo[ECU_Choose].TX_FrameNum);
										//setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].TX_FrameNum, 1);
									}

									//OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, g_diaginfo[ECU_Choose].TX_Data);
									canTxEvt_win32.tag = XL_CAN_EV_TAG_TX_MSG;
									canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
									canTxEvt_win32.tagData.canMsg.dlc = 8;
									for (int n = 0; n < 8; n++)
									{
										canTxEvt_win32.tagData.canMsg.data[n] = g_diaginfo[ECU_Choose].TX_Data[n];
									}

									demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);



									if (g_diaginfo[ECU_Choose].TX_DataLength < 8)
									{
										//setHEXDatatocontrol(debug_info, g_diaginfo[ECU_Choose].TX_Data, len / 2 + 1, 1);

										settexttocontrol(Text_out, "TX:", 1);
										setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, 0);

									}


									SendMessageA(Text_out, WM_VSCROLL, SB_BOTTOM, 0);//设置滚轮到末尾，这样就可以看到最新信息













									//securityAlgorithm(SecurityInfo[ChooseDIAG].Seed, SecurityInfo[ChooseDIAG].Xor_Key, SecurityInfo[ChooseDIAG].level, SecurityInfo[ChooseDIAG].outputkey);

									//setTimer(send_key_timer, 100);
								}

							}

							if (g_diaginfo[ECU_Choose].RX_Data[0] == 0x50 && g_diaginfo[ECU_Choose].RX_Data[1] == 0x03)
							{
								demoTransmitBurst_3E(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);
							}
							if (g_diaginfo[ECU_Choose].RX_Data[0] == 0x50 && g_diaginfo[ECU_Choose].RX_Data[1] == 0x02)
							{
								demoTransmitBurst_3E(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);
							}

							if (g_diaginfo[ECU_Choose].RX_Data[0] == 0x50 && g_diaginfo[ECU_Choose].RX_Data[1] == 0x01)
							{
								demoStopTransmitBurst_3E();
							}



						}
					}
					else if (g_diaginfo[ECU_Choose].RX_N_PCItype == FF_N_PDU)//首帧
					{
						if (g_diaginfo[ECU_Choose].RX_flag == 0 && g_diaginfo[ECU_Choose].TX_flag == 0)
						{
							g_diaginfo[ECU_Choose].RX_flag = 1;
							CalculateFramesSenquence(xlCanRxEvt.tagData.canRxOkMsg.data, &g_diaginfo[ECU_Choose].RX_DataLength, &g_diaginfo[ECU_Choose].RX_FrameNum);
								
							for (i = 0; i < 6; i++)
							{
								g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_byte_counter] = xlCanRxEvt.tagData.canRxOkMsg.data[i + 2];
								g_diaginfo[ECU_Choose].RX_byte_counter++;
							}
							//settexttocontrol(Text_out, "首帧：", 1);
							//setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 6, 0);//Data

								
							//Teser发送流控帧,只发一次
			//                if(gReq_Type==0)
			//                {
			//                    
			//                }
			//                else
			//                {
			//                    OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_FUNC_ID,TX_FC_Frame); 
			//                }
							//OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, TX_FC_Frame);


							//XLcanTxEvent canTxEvt;
							Sleep(50);
							canTxEvt_win32.tagData.canMsg.msgFlags = gSendMsgByCANFD_flag;
							canTxEvt_win32.tagData.canMsg.dlc = 8;
							canTxEvt_win32.tagData.canMsg.data[0] = 0x30;
							canTxEvt_win32.tagData.canMsg.data[1] = 0x00;
							canTxEvt_win32.tagData.canMsg.data[2] = 0x14;
							canTxEvt_win32.tagData.canMsg.data[3] = 0x00;
							canTxEvt_win32.tagData.canMsg.data[4] = 0x00;
							canTxEvt_win32.tagData.canMsg.data[5] = 0x00;
							canTxEvt_win32.tagData.canMsg.data[6] = 0x00;
							canTxEvt_win32.tagData.canMsg.data[7] = 0x00;


							demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);


							g_diaginfo[ECU_Choose].RX_FrameNum = g_diaginfo[ECU_Choose].RX_FrameNum - 1;

							//return;
						}
					}
					else if (g_diaginfo[ECU_Choose].RX_N_PCItype == FC_N_PDU)//流控帧
					{
						if (g_diaginfo[ECU_Choose].RX_flag == 0 && g_diaginfo[ECU_Choose].TX_flag == 1)
						{
							g_diaginfo[ECU_Choose].TX_flag = 2;
							g_diaginfo[ECU_Choose].FS = xlCanRxEvt.tagData.canRxOkMsg.data[0] & 0x0f;
							g_diaginfo[ECU_Choose].BS = xlCanRxEvt.tagData.canRxOkMsg.data[1];
							g_diaginfo[ECU_Choose].STmin = xlCanRxEvt.tagData.canRxOkMsg.data[2];

							if (g_diaginfo[ECU_Choose].FS == 0)
							{
								g_diaginfo[ECU_Choose].TX_SenquenceCount = (g_diaginfo[ECU_Choose].TX_SenquenceCount + 1) & 0x0f;
								g_diaginfo[ECU_Choose].TX_Count = g_diaginfo[ECU_Choose].TX_Count + 1;
								//setTimer(STmin_timer, g_diaginfo[ECU_Choose].STmin);
								do
								{
									settexttocontrol(debug_info, "流控0：", 1);
									setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].RX_flag, 0);
									setHEXtocontrol(debug_info, g_diaginfo[ECU_Choose].TX_FrameNum, 0);
									Sleep(20);
									if (g_diaginfo[ECU_Choose].RX_flag == 0 && g_diaginfo[ECU_Choose].TX_flag == 2)
									{
										canTxEvt_win32.tagData.canMsg.data[0] = (CF_N_PDU << 4) + g_diaginfo[ECU_Choose].TX_SenquenceCount;

										for (i = 0; i < 7; i++)
										{
											canTxEvt_win32.tagData.canMsg.data[i + 1] = g_diaginfo[ECU_Choose].TX_Data[g_diaginfo[ECU_Choose].TX_Count * 7 + i + 1];
										}
										//OutFrame(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, data);
										demoTransmit(g_diaginfo[ECU_Choose].DIAG_REQ_PHY_ID, xlChanMaskTx);
										g_diaginfo[ECU_Choose].TX_SenquenceCount = (g_diaginfo[ECU_Choose].TX_SenquenceCount + 1) & 0x0f;
										g_diaginfo[ECU_Choose].TX_Count = g_diaginfo[ECU_Choose].TX_Count + 1;

										if (g_diaginfo[ECU_Choose].TX_Count < g_diaginfo[ECU_Choose].TX_FrameNum)
										{
											//setTimer(STmin_timer, g_diaginfo[ECU_Choose].STmin);
										}
										else
										{
											//PrintHexToControl("TX：", g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, "DIAG", "DiagDisaply");
											g_diaginfo[ECU_Choose].TX_Count = 0;
											g_diaginfo[ECU_Choose].TX_flag = 0;
											g_diaginfo[ECU_Choose].TX_FrameNum = 0;
											g_diaginfo[ECU_Choose].TX_N_PCItype = 0xff;
											g_diaginfo[ECU_Choose].TX_SenquenceCount = 0;
											settexttocontrol(Text_out, "TX:", 1);
											setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].TX_Original_Data, g_diaginfo[ECU_Choose].TX_DataLength, 0);

										}
									}
								//}
								} while (g_diaginfo[ECU_Choose].TX_flag!=0);

							}




						}
					}
					else if (g_diaginfo[ECU_Choose].RX_N_PCItype == CF_N_PDU)//连续帧
					{
						if (g_diaginfo[ECU_Choose].RX_flag == 1 && g_diaginfo[ECU_Choose].TX_flag == 0)
						{
							//setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 6, 0);//Data
							for (i = 0; i < can_datalenth - 1; i++)
							{
								g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_byte_counter] = xlCanRxEvt.tagData.canRxOkMsg.data[i + 1];
								g_diaginfo[ECU_Choose].RX_byte_counter++;
								//write("%X",g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_byte_counter]);
							}
								

							g_diaginfo[ECU_Choose].RX_FrameNum = g_diaginfo[ECU_Choose].RX_FrameNum - 1;

							if (g_diaginfo[ECU_Choose].RX_FrameNum == 0)
							{
								g_diaginfo[ECU_Choose].RX_Data[g_diaginfo[ECU_Choose].RX_DataLength] = 0;
								//settexttocontrol(Text_out, "RX:", 1);
								//setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, g_diaginfo[ECU_Choose].RX_DataLength, 0);

								


								if (g_diaginfo[ECU_Choose].RX_Data[1] == 0xF1 && g_diaginfo[ECU_Choose].RX_Data[2] == 0x95)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0xF1 && g_diaginfo[ECU_Choose].RX_Data[2] == 0x89)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0xF1 && g_diaginfo[ECU_Choose].RX_Data[2] == 0x80)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0x06 && g_diaginfo[ECU_Choose].RX_Data[2] == 0x20)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0x0f && g_diaginfo[ECU_Choose].RX_Data[2] == 0x20)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0x0c && g_diaginfo[ECU_Choose].RX_Data[2] == 0x20)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else if (g_diaginfo[ECU_Choose].RX_Data[1] == 0xF1 && g_diaginfo[ECU_Choose].RX_Data[2] == 0x93)
								{
									settexttocontrol(Text_out, "RX_ascil: ", 1);
									settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
								}
								else
								{
									settexttocontrol(Text_out, "RX: ", 1);
									//settexttocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, 0);
									setHEXDatatocontrol(Text_out, g_diaginfo[ECU_Choose].RX_Data, g_diaginfo[ECU_Choose].RX_DataLength, 0);
								}
								SendMessageA(Text_out, WM_VSCROLL, SB_BOTTOM, 0);//设置滚轮到末尾，这样就可以看到最新信息
								g_diaginfo[ECU_Choose].RX_flag = 0;
								g_diaginfo[ECU_Choose].RX_DataLength = 0;
								g_diaginfo[ECU_Choose].RX_byte_counter = 0;

							}
						}
					}

				}
				
			}

		} while (XL_SUCCESS == xlStatus);
	}

	return(NO_ERROR);
} // RxCanFdThread














void ResetByteArrary(uint8_t data[])
{
	int i;
	for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++)
		data[i] = 0x55;
}


//根据 首帧的前两个字节计算传输字节数，并计算需要分段的数量
void CalculateFramesSenquence(uint8_t payload[], long * DataLength, long * FrameNum)
{
	long divisor;
	long remainder;
	long t;
	//int t = 17;
	// 首帧的byte(0)的低4bit和byte(1) 是多帧吧own的数据长度
	*DataLength = (((payload[0] << 8) + payload[1]) & 0x0FFF);
	t= ((payload[0] << 8) + payload[1]) & 0x0FFF;
	*DataLength = t;
	//setHEXtocontrol(debug_info, *DataLength, 1);
	divisor = (*DataLength + 1) / (can_datalenth - 1); //+1是因为首帧的byte(0)和byte(1)非数据，-1是因为连续帧的byte(0)非数据
	remainder = (*DataLength + 1) % (can_datalenth - 1);
	t1 = remainder ? (divisor + 1) : divisor;// FrameNum:多帧报文数量（包含首帧），有余数就多一个报文
	//t1 = 2;
	*FrameNum = t1;
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