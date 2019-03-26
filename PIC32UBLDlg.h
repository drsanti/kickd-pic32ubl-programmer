// PIC32UBLDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Hex.h"
#include "BootLoader.h"


//! Modified CPIC32UBLDlg code
/*
This modified implementation of the original Microchip MFC dialog can be run without 
UI or windows handle. 
The added messagePump() method needs to ensure the application is progressing and application
status can be reported ( ublApiStatus / ublApiProgress). 
*/

class CPIC32UBLDlg : public CWnd
{
enum UblApiStatus {
	UBLAPI_DISCONNECTED = 0,
	UBLAPI_READY = 1,
	UBLAPI_CONNECTING = 2,
	UBLAPI_ERASE = 3,
	UBLAPI_PROGRAM = 4,
	UBLAPI_VERIFY = 5,
	UBLAPI_APPSTART = 6, 
	UBLAPI_READVER = 7,
	UBLAPI_ERROR = 8
};

// Construction
public:
	CPIC32UBLDlg();

	//! dispatches as messages and returns, when message queue empty 
	void messagePump();

// Implementation
protected:
	CBootLoader mBootLoader;
	bool EraseProgVer;
	bool ConnectionEstablished;
	void PrintKonsole(CString string);
	void ClearKonsole(void);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	UINT PortSelected;
public:
	void OnBnClickedCancel();
	void OnBnClickedButtonConnect(UINT portType, INT comport, INT baud, 
	UINT vid, UINT pid, ULONG ip, USHORT skt);
	void Disconnect(); 

	// PIC32 Bootloader functions.
	void ExtraInitialization(void);
	void StartSendReceiveTimers(void);
	void ProgressBarTask(void);
	void ProgramHexFile(void);
	
	afx_msg LRESULT OnReceiveResponse(WPARAM, LPARAM);
	afx_msg LRESULT OnTransmitFailure(WPARAM, LPARAM);

	char TxCommand;
	char TxData[255];

	afx_msg void OnTimer(UINT nIDEvent);
	CString cstringEditConsole;
	afx_msg void OnBnClickedButtonErase();
	bool OnBnClickedButtonLoadhex(const char *hexFilePath);
	afx_msg void OnBnClickedButtonProgram();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonVerify();
	afx_msg void OnBnClickedButtonEraseProgVerify();
	afx_msg void OnBnClickedButtonRunapplication();
	afx_msg void OnBnClickedButtonBootVer();
	unsigned long uLongEditBoxVID;
	unsigned long uLongEditBoxPID;
	bool boolRadioEnableCom;
	bool boolRadioEnableUSB;
	afx_msg void OnBnClickedRadioEnableCom();
	BOOL boolCheckComEnable;
	BOOL boolCheckUsbEnable;
	afx_msg void OnBnClickedCheckComEnable();
	afx_msg void OnBnClickedCheckUsbEnable();
	CString stringEditBoxUSBVID;
	CString stringEditBoxUSBPID;
	afx_msg void OnBnClickedCheckEthEnable();
	BOOL boolCheckEthEnable;
	DWORD ip_value;
	UINT valEditBoxSocket;

	//! this is public information that is evaluated by UblApi 
	int ublApiStatus; 
	int	ublApiProgress; 
};
