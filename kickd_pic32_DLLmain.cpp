/*
$Id: kickd_pic32_DLLmain.cpp 336 2014-02-28 12:44:41Z oliver $
*/ 

// Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "kickd_pic32_DLLmain.h"
#include <assert.h>

#pragma optimize( "", off )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CKickdPic32Ubl

BEGIN_MESSAGE_MAP(CKickdPic32Ubl, CWinApp)
END_MESSAGE_MAP()


// CKickdPic32Ubl construction

CKickdPic32Ubl::CKickdPic32Ubl()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CKickdPic32Ubl object

CKickdPic32Ubl theApp;


// CKickdPic32Ubl initialization

BOOL CKickdPic32Ubl::InitInstance()
{
	//! multithreading and multiple instances not supported, go through global object always
	//! (improvement should protect this with a critical section) 
	return CWinApp::InitInstance();
}

int CKickdPic32Ubl::ExitInstance( ) {
	return CWinApp::ExitInstance();
}