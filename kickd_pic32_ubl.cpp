#include "kickd_pic32_ubl.h"
#include "targetver.h"
#include "PIC32UBLDlg.h"
#include <assert.h>

#pragma optimize( "", off )

CPIC32UBLDlg *pPic32Ubl;

void  __cdecl mcUblInit() {
	pPic32Ubl = new CPIC32UBLDlg();
}
void  __cdecl mcUblShutdown() {
	pPic32Ubl->Disconnect();
	//! See Microsoft MFC TN017: Destroying Window Objects
	pPic32Ubl->DestroyWindow();
	assert(pPic32Ubl->m_hWnd == 0);
	delete pPic32Ubl;
	pPic32Ubl = 0;
}

bool  __cdecl mcUblConnectAndProgram(const char *hexFilePath, const int vid, const int pid) {
	//! can only be used if an instance of CPIC32UBLDlg was created first, using mcUblInit().
	assert(pPic32Ubl);

	// load hex fail, return with failure
	if (!pPic32Ubl->OnBnClickedButtonLoadhex(hexFilePath)) {
		return false; 
	}
	pPic32Ubl->OnBnClickedButtonConnect(USB, 1, 115200, vid, pid, 1, 6234); 
	return true;
}

void  __cdecl mcUblDisconnect() {
	assert(pPic32Ubl);
	pPic32Ubl->Disconnect();
}

void  __cdecl mcUblMessagePump() {
	assert(pPic32Ubl);
	pPic32Ubl->messagePump();
}

int  __cdecl mcUblGetStatus() {
	assert(pPic32Ubl);
	return (int)pPic32Ubl->ublApiStatus; 
}