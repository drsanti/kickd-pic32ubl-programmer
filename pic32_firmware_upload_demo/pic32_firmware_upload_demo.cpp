//! pic32_firmware_upload_demo.cpp 
/*!
	Standard C++ example for using the kickd_pic32_ubl.dll
*/

#include "stdafx.h"

#include <iostream> 
using namespace std; 
#include "Pic32UblRtLink.h"

#define FW_HEX_FILE "Demo_App_PIC32_Starter_Kits.hex"

Pic32UblRtLink *pUpdater;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Kickdrive / Microchip PIC32 UBL Demo application. Use at own risk!" << endl; 
	cout << "PIC32 UBL bootloader must be active." << endl; 
	cout << "Firmware hex file used: " << FW_HEX_FILE << endl; 
	system("pause");
	cout << "Starting..." << endl; 
	//! Load kickd_pic32_ubl.dll
	pUpdater = new Pic32UblRtLink(); 
	bool success = (pUpdater->getDLLStatus() == 0); 
	if (!success) {
		cout << "Failed to load " PIC_DLL_PATH << endl; 
	}
	else {
		//! Start programming (your device must be in bootloader mode already!) 
		success = pUpdater->ConnectAndProgram(FW_HEX_FILE, 0x4d8, 0x3c);
		if (!success) {
			delete pUpdater; 
			pUpdater = NULL;
			cout << "Failed to start programming. File not found, device not ready or bootloader not active." << endl; 
		}
	}
	if (success) {
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
	}
	cout << "Microchip PIC32 UBL demo ended." << endl; 
	system("pause");
	return (success ? 0 : 1); 
}

