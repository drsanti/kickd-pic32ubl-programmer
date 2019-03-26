#pragma once

//! DLL file path, relative to the current application path. Adjust if necessary
#define PIC_DLL_PATH "kickd_pic32_ubl.dll"

#include "Windows.h"

//! custom error/status coded, as used in the original Microchip CPIC32UBLDlg class
enum {
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

//! Adapter class for accessing the kickd_pic32_ubl.dll without lib/header file.
/*!
	See \ref Pic32UblRtLinkdoc for more information and example code. 
*/
class Pic32UblRtLink
{
public:
	Pic32UblRtLink(void);
	~Pic32UblRtLink();
	//! returns the current status of the FTDI DLL support
	/*!
		\returns
			0 - DLL ok\n
			1 - one or several DLL functions are not available\n
			2 - DLL not found\n
	*/
	int getDLLStatus() const;

	//! the actual DLL name / path used
	const char* getDLLFilename() const { return PIC_DLL_PATH; };

	//! wrapper for the MicrochipUblApi.h / kickd_pic32_ubl.dll function
	bool ConnectAndProgram(const char *hexFilePath, const int vid, const int pid);
	//! wrapper for the MicrochipUblApi.h / kickd_pic32_ubl.dll function
	void Disconnect();
	//! wrapper for the MicrochipUblApi.h / kickd_pic32_ubl.dll function
	void MessagePump();
	//! wrapper for the MicrochipUblApi.h / kickd_pic32_ubl.dll function
	int GetStatus();

private:
	//! DLL specific init functions
	void Init();
	void Shutdown();

	//! explicit linking of kickd_pic32_ubl.dll
	int LoadDLL();
	//! free the library again
	void UnloadDLL();

	HMODULE m_hmodule;
	int m_dllStatus; 

	typedef void (__cdecl *PtrToInit)(); 
	PtrToInit m_pInit; 

	typedef void (__cdecl *PtrToShutdown)(); 
	PtrToShutdown m_pShutdown; 

	typedef bool (__cdecl *PtrToConnectAndProgram)(const char *, const int, const int); 
	PtrToConnectAndProgram m_pConnectAndProgram; 
	
	typedef void (__cdecl *PtrToDisconnect)(); 
	PtrToDisconnect m_pDisconnect; 

	typedef void (__cdecl *PtrToMessagePump)();
	PtrToMessagePump m_pMessagePump; 

	typedef int (__cdecl *PtrToGetStatus)();
	PtrToGetStatus m_pGetStatus;
};
