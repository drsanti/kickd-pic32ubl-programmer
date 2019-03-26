// PIC32UBLDlg.cpp : implementation file
//

#include "stdafx.h"


#include "PIC32UBLDlg.h"
#include <windows.h>
#include <wtypes.h>
#include <dbt.h>

#pragma optimize( "", off )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BOOTLOADER_ROUTINE_TIMER 1
#define BOOTLOADER_GUI_UPDATE_TIMER 2
#define SWAP(x)   ( ((x<<24)&(0xFF000000)) | ((x<<8)&(0x00FF0000))  | ((x>>24)&(0x000000FF)) | ((x>>8)&(0x0000FF00)) )

#define SAVE 1
#define RESTORE 2

// CPIC32UBLDlg dialog

CPIC32UBLDlg::CPIC32UBLDlg()
{
   CString wcn = ::AfxRegisterWndClass(NULL);
   BOOL created = this->CreateEx(0, wcn, _T("CPic32UblDlg"), 0, 0, 0, 0, 0, HWND_MESSAGE, 0);
   ublApiStatus = UBLAPI_DISCONNECTED; 
   PortSelected = USB;
}

void CPIC32UBLDlg::messagePump() {
	MSG uMsg;
     while (PeekMessage (&uMsg, NULL, 0, 0, PM_REMOVE) > 0) //Or use an if statement
     {
          TranslateMessage (&uMsg);
          DispatchMessage (&uMsg);
     }
}

BEGIN_MESSAGE_MAP(CPIC32UBLDlg, CWnd)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_BOOTLOADER_RESP_OK, OnReceiveResponse)
	ON_MESSAGE(WM_USER_BOOTLOADER_NO_RESP, OnTransmitFailure)
	ON_WM_DEVICECHANGE()
	ON_WM_DEVMODECHANGE()
END_MESSAGE_MAP()


// CPIC32UBLDlg message handlers

BOOL CPIC32UBLDlg::OnInitDialog()
{
	// TODO: Add extra initialization here
	ExtraInitialization();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPIC32UBLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


//extern HDEVINFO SetupDiGetClassDevs(CONST GUID*, PCTSTR, HWND, DWORD);

/****************************************************************************
 * Invoked when connect button is clicked
 *
 *  
 *****************************************************************************/

void  CPIC32UBLDlg::Disconnect() {
	// Already connected. Disconnect now.
	ConnectionEstablished = false;

	// Shut down the rx-tx thread, if already opened.
		mBootLoader.ShutdownThread();
	
	if(mBootLoader.GetPortOpenStatus(PortSelected))
	{
		// com port already opened. close com port
		mBootLoader.ClosePort(PortSelected);			
	}
	ublApiStatus = UBLAPI_DISCONNECTED;
}

void CPIC32UBLDlg::OnBnClickedButtonConnect(UINT portType, INT comport, INT baud, 
	UINT vid, UINT pid, ULONG ip, USHORT skt)

{	
	{
		// Establish new connection.
		if(mBootLoader.GetPortOpenStatus(PortSelected))
		{
			// com port already opened. close com port
			mBootLoader.ClosePort(PortSelected);			
		}
		// Open Communication port freshly.
		PortSelected = portType;
		mBootLoader.OpenPort(PortSelected, comport, baud, vid, pid, skt, ip, GetSafeHwnd());
		
		if(mBootLoader.GetPortOpenStatus(PortSelected))
		{	// COM port opened.
			// Shut down the rx-tx thread, if already opened.
			mBootLoader.ShutdownThread();
			// Create a new thread to do Rx Tx in the back ground.			
			mBootLoader.CreateRxTxThread(GetSafeHwnd()); // Create Rx Tx thread.
			// Trigger Read boot info command
			mBootLoader.SendCommand(READ_BOOT_INFO, 30, 200);					
			
			// Start timer to handle GUI updates.
			SetTimer(BOOTLOADER_GUI_UPDATE_TIMER, 300, NULL);  // Timer elapses every 300 ms.

			// Print a message to user/
			PrintKonsole("Please reset device and invoke bootloader");

			ublApiStatus = UBLAPI_CONNECTING; 
		}
	}
}

/****************************************************************************
 * Invoked just after dialog is created. Do some extra initialization
 * like enabling and disabling buttons and so on.
 *  
 *****************************************************************************/
void CPIC32UBLDlg::ExtraInitialization()
{

}

/****************************************************************************
 * Just do some mundane things here, like getting progress bar status, 
	disabling buttons if connection cuts.
   This function is called everytime the timer elapses.
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnTimer(UINT nIDEvent)
{
	CWnd::OnTimer(nIDEvent);
}

/****************************************************************************
 * Invoked when button erase is clicked.
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonErase()
{
	mBootLoader.SendCommand(ERASE_FLASH, 3, 7000); //7s retry delay, becuse erase takes considerable time.
}


/****************************************************************************
 * Enables all button, if the parameter passed is true.
 * 
 *  
 *****************************************************************************/


/****************************************************************************
 * Invoked when load hex button is clicked.
 * 
 *  
 *****************************************************************************/
bool CPIC32UBLDlg::OnBnClickedButtonLoadhex(const char *hexFilePath)
{
	CString string;
	// Save button status.
	if (mBootLoader.LoadHexFile(hexFilePath))
	{
		PrintKonsole("Hex file loaded successfully");
		ublApiStatus = UBLAPI_READY;
		return true;
	}
	else
	{
		PrintKonsole("Hex file loading failed");
		ublApiStatus = UBLAPI_ERROR;
		return false;
	}	

}


/****************************************************************************
 * This function is invoked when there is a response from the device.
 Process the response based on the command.
 * 
 *  
 *****************************************************************************/
LRESULT CPIC32UBLDlg::OnReceiveResponse(WPARAM cmd, LPARAM RxDataPtrAdrs)
{
	char MajorVer;// = RxData[3];
	char MinorVer ;//= RxData[4];
	char *RxData;
	CString string;
	unsigned short crc;

	RxData = (char *)RxDataPtrAdrs;
	MajorVer = RxData[0];
	MinorVer = RxData[1];

	switch(cmd)
	{
	case READ_BOOT_INFO:
		if(ConnectionEstablished == false)
		{
			// New connection.
			ClearKonsole();
			string.FormatMessage("Device connected"); 
			PrintKonsole(string);
		}
		string.FormatMessage("Bootloader Firmware Version: %1!d!.%2!d!", MajorVer, MinorVer);
		PrintKonsole(string);		

		// Change the connect button to disconnect.
		ConnectionEstablished = true;
		// Disable baud rate and com port combo boxes.
		ublApiStatus = UBLAPI_READY;
		OnBnClickedButtonEraseProgVerify();
		break;

	case ERASE_FLASH:
		PrintKonsole("Flash Erased");
		if(EraseProgVer)// Operation Erase->Program->Verify
		{
			// Erase completed. Next operation is programming.
			mBootLoader.SendCommand(PROGRAM_FLASH, 10, 500); // 500ms delay	
		}
		ublApiStatus = UBLAPI_PROGRAM; 
		break;

	case PROGRAM_FLASH:
		PrintKonsole("Programming completed");
		if(EraseProgVer)// Operation Erase->Program->Verify
		{
			// Programming completed. Next operation is verification.
			mBootLoader.SendCommand(READ_CRC, 5, 7000);// 7 second delay
		}
		ublApiStatus = UBLAPI_VERIFY; 
		break;

	case READ_CRC:
		crc = ((RxData[1] << 8) & 0xFF00) | (RxData[0] & 0x00FF);
		// Reset erase->program-verify operation.
		EraseProgVer = false;	

		if(crc == mBootLoader.CalculateFlashCRC())
		{
			PrintKonsole("Verification successfull");
			ublApiStatus = UBLAPI_READY;
			OnBnClickedButtonRunapplication();
		}
		else
		{
			PrintKonsole("Verification failed");
			ublApiStatus = UBLAPI_ERROR;
		}
		break;
	}
	
	return 1;
}


/****************************************************************************
 *  This function is invoked when there is no resonse from the device, even after
	retries.
 *  
 *****************************************************************************/
LRESULT CPIC32UBLDlg::OnTransmitFailure(WPARAM cmd, LPARAM)
{
	CString string;
	ublApiStatus = UBLAPI_ERROR;
	EraseProgVer = false;
	switch(cmd)
	{
	case READ_BOOT_INFO:		
	case ERASE_FLASH:
	case PROGRAM_FLASH:
	case READ_CRC:
		PrintKonsole("No Response from the device. Operation failed");
		break;
	}

	return 1;
}


/****************************************************************************
 * Invoked when the button program is clicked.
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonProgram()
{
	mBootLoader.SendCommand(PROGRAM_FLASH, 3, 5000); // 500ms delay
	ublApiStatus = UBLAPI_PROGRAM;
}


/****************************************************************************
 * This function is invoked when button verify is clicked
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonVerify()
{
	mBootLoader.SendCommand(READ_CRC, 3, 5000);
	ublApiStatus = UBLAPI_VERIFY;
}


/****************************************************************************
 * This function is invoked when button Erase-Program-Verify is clicked
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonEraseProgVerify()
{
	EraseProgVer = true;
	// Start with erase. Rest is automatically handled by state machine.
	mBootLoader.SendCommand(ERASE_FLASH, 5, 5000); // 5s delay
	ublApiStatus = UBLAPI_ERASE; 
}


/****************************************************************************
 * This function is invoked when button run application is clicked
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonRunapplication()
{
	CString string;

	mBootLoader.SendCommand(JMP_TO_APP, 5, 100); // 100ms delay
	PrintKonsole("\nCommand issued to run application");
	ublApiStatus = UBLAPI_APPSTART; 
}

/****************************************************************************
 * This function is invoked when button Read Version is clicked
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedButtonBootVer()
{
	mBootLoader.SendCommand(READ_BOOT_INFO, 50, 200);
	ublApiStatus = UBLAPI_READVER; 
}


/****************************************************************************
 * Print Konsole
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::PrintKonsole(CString string)
{

	cstringEditConsole += string;
	string.FormatMessage("\n");
	cstringEditConsole += string;
	
}

/****************************************************************************
 * Clears konsole
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::ClearKonsole(void)
{

	cstringEditConsole = "";
	
	
}


/****************************************************************************
 * USB check box button clicked.
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedCheckUsbEnable()
{	
	boolCheckUsbEnable = 1;
	boolCheckComEnable = 0;
	boolCheckEthEnable = 0;
	PortSelected = USB;	
}


/****************************************************************************
 * Ethernet check box button clicked.
 * 
 *  
 *****************************************************************************/
void CPIC32UBLDlg::OnBnClickedCheckEthEnable()
{
	// TODO: Add your control notification handler code here
	
	boolCheckEthEnable = 1;
	boolCheckComEnable = 0;
	boolCheckUsbEnable = 0;
	PortSelected = ETH;	
}
