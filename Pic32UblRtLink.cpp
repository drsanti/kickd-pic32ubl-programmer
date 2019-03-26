#include <tchar.h>
#include "Pic32UblRtLink.h"

#pragma optimize( "", off )

Pic32UblRtLink::Pic32UblRtLink()
{
	m_dllStatus = LoadDLL();
	Init();
}

Pic32UblRtLink::~Pic32UblRtLink()
{
	Shutdown();
	UnloadDLL();
}

int Pic32UblRtLink::getDLLStatus() const {
	return m_dllStatus;
}

int Pic32UblRtLink::LoadDLL() {
	//! Return value is 0, if successful, 1 if one or several DLL functions could not be found, 
	//! or 2 if the DLL could not be found at all

	//! Helper macro: Reset pointers
	#define LIBFCTNULL(fctName) m_p##fctName = NULL;
	LIBFCTNULL(Init);
	LIBFCTNULL(Shutdown);
	LIBFCTNULL(ConnectAndProgram);
	LIBFCTNULL(Disconnect);
	LIBFCTNULL(MessagePump);
	LIBFCTNULL(GetStatus);

	m_hmodule = LoadLibrary(_T(PIC_DLL_PATH));	
	//! DLL not found
	if (m_hmodule == NULL) return 2;

	//! Helper macro: Obtain a pointer for a DLL function. 
	//! Exits the function and returns with value 1 if DLL function not available
	#define GETLIBFCT(fctName) m_p##fctName = (PtrTo##fctName)GetProcAddress(m_hmodule, "mcUbl" #fctName); if (m_p##fctName == NULL) return 1;

	GETLIBFCT(Init);
	GETLIBFCT(Shutdown);
	GETLIBFCT(ConnectAndProgram);
	GETLIBFCT(Disconnect);
	GETLIBFCT(MessagePump);
	GETLIBFCT(GetStatus);
	//! all fine 
	return 0;
}

void Pic32UblRtLink::UnloadDLL() {
	if (m_hmodule != NULL) {
		FreeLibrary(m_hmodule);
		m_hmodule = NULL;
	}
}

//! Wrapper members for the kickd_pic32_ubl.dll library functions

void Pic32UblRtLink::Init()
{
	if (!m_pInit) return;
	(*m_pInit)();
}

void Pic32UblRtLink::Shutdown()
{
	if (!m_pShutdown) return;
	(*m_pShutdown)();
}

bool Pic32UblRtLink::ConnectAndProgram(const char *hexFilePath, const int vid, const int pid)
{
	if (!m_pConnectAndProgram) return false;
	return (*m_pConnectAndProgram)(hexFilePath, vid, pid);
}

void Pic32UblRtLink::Disconnect()
{
	if (!m_pDisconnect) return;
	(*m_pDisconnect)();
}

void Pic32UblRtLink::MessagePump()
{
	if (!m_pMessagePump) return;
	(*m_pMessagePump)();
}

int Pic32UblRtLink::GetStatus()
{
	if (!m_pGetStatus) return 2;
	return (*m_pGetStatus)();
}