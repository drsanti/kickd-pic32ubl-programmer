/*! \mainpage kickd_pic32_ubl

Updating firmware on Microchip PIC32 devices from your own Windows application. 

kickd_pic32_ubl.dll is a simple Windows 32 bit DLL API that offers the functionality of the 
Microchip PIC32UBL.exe Windows PC application. No additional dependencies. 

How to Use: 

- Alternative 1: Use the kickd_pic32_ubl.h DLL include header and the kickd_pic32_ubl.lib file
  for implicit / static linking of the kickd_pic32_ubl.dll.

- Alternative 2: Use the Pic32UblRtLink C++ class for explicit linking 
  at runtime. I.e. the DLL is only loaded on use, and not required at startup of 
  your application. 
  
See also \ref Pic32UblRtLinkdoc for more information and example code.

This DLL is based on the "Microchip PC Application" MFC programming example PIC32UBL.exe.
Copyright (c) 2013 Microchip Technology Inc.  All rights reserved. 

See the Microchip Application Note AN1388 and the related source code ("PC Application" folder) 
for copyright and additional information: 
http://ww1.microchip.com/downloads/en/AppNotes/AN1388%20Source%20Code%202014_02_14.zip

For the modifications and DLL wrapper around the Microchip PIC32UBL code, the following applies:\n
Author: Oliver Heggelbacher.\n
This is free and unencumbered software released into the public domain
according to http://unlicense.org. No warranty. 

kickd_pic32_ubl.dll and the related example have been compiled using Visual Studio 2008, 
but there is no need to recompile kickd_pic32_ubl.dll yourself. kickd_pic32_ubl.dll can be used
"as is" - It is linked statically and does not require additional runtime DLLs.
kickd_pic32_ubl.dll uses __cdecl calling convention (not __stdcall / WINAPI).


*/

/*! 
\page Pic32UblRtLinkdoc Pic32UblRtLink - How to use
The Pic32UblRtLink adapter class implements explicit linking of the kickd_pic32_ubl.dll. 
This is to avoid external references to the DLL during compile time and allows starting your application 
even when the kickd_pic32_ubl.dll is not available. 

How to use:

- Make sure the PIC_DLL_PATH define in Pic32UblRtLink.h contains the correct path to the DLL.
- Create a Pic32UblRtLink object, and get your downloader started, e.g using something like this: 

\code
Pic32UblRtLink *pUpdater;
pUpdater = new Pic32UblRtLink();   // (with pUpdater a Pic32UblRtLink * pointer) 
bool success = (pUpdater->getDLLStatus() == 0); 
if (!success) {
	// indicate error
}
else {
	success = pUpdater->ConnectAndProgram("myfile.hex", 0x4d8, 0x3c);
	if (!success) {
		// indicate error
	}
}
if (!success) {
	delete pUpdater; 
	pUpdater = NULL;
}
\endcode

- Then periodically call the message pump and check the status of the current programming task. E.g something like

\code
bool finished = false; 
int status = -1; 
while (!finished) {
	pUpdater->MessagePump();
	Sleep(10);
	int newStatus = pUpdater->GetStatus();
	if (newStatus != status) {
		status = newStatus;
		switch (status) {
			case UBLAPI_READY:
					cout << "Could not connect to device. Aborting." << endl; 
					finished = true;
					success = false;
					break;
			case UBLAPI_CONNECTING:
					cout << "Connecting..." << endl; 
					break;
			case UBLAPI_ERASE:
					cout << "Erasing..." << endl; 
					break;
			case UBLAPI_PROGRAM:
					cout << "Programming..." << endl; 
					break;
			case UBLAPI_VERIFY:
					cout << "Verifying..." << endl; 
					break;
			case UBLAPI_APPSTART:
					cout << "Programming completed. Application started." << endl; 
					finished = true;
					break;
			case UBLAPI_ERROR:
					cout << "Error while programming." << endl; 
					finished = true;
					success = false;
					break;
		}
	}
} 
pUpdater->Disconnect();
delete pUpdater;
pUpdater = NULL;
\endcode

A fully working example is "pic32_firmware_upload_demo.exe", source code in 
folder \pic32_firmware_upload_demo. 

It uses a Microchip example .hex file name by default, "Demo_App_PIC32_Starter_Kits.hex".

A sample run, using a custom firmware file for our own device "usb2drive" looked like this: 

\verbatim 
Kickdrive / Microchip PIC32 UBL Demo application. Use at own risk!
PIC32 UBL bootloader must be active.
Firmware hex file used: usb2drive_V1.1.1.353.hex
Press any key to continue . . .
Starting...
Connecting...
Erasing...
Programming...
Verifying...
Programming completed. Application started.
Microchip PIC32 UBL demo ended.
Press any key to continue . . .
\endverbatim


*/ 