// EDDX52.cpp : Defines the exported functions for the DLL application.
// Direct Output Pages are VERY buggy, or better say they don't work. So I had to implement the system by myself (MFD only)
#pragma once

//#ifdef _USRDLL 
//#define EDD_API __declspec(dllexport)
//#else
//#define EDD_API __declspec(dllimport)
//#endif  

#include "stdafx.h"
#include "EDDInterface64.h"
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <comdef.h>

extern "C" {
	typedef void __stdcall Pfn_DirectOutput_Device_Callback(void* hDevice, bool bAdded, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_EnumerateCallback(void* hDevice, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_SoftButton_Callback(void* hDevice, DWORD dwButtons, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_Page_Callback(void* hDevice, DWORD dwPage, bool bActivated, void* pvContext);


	typedef HRESULT (__stdcall *DirectOutput_Initialize)(const wchar_t * wszAppName);
	const char * do_initFun = "DirectOutput_Initialize";
	typedef HRESULT (__stdcall *DirectOutput_Deinitialize)();
	const char * do_deInitFun = "DirectOutput_Deinitialize";

	typedef HRESULT (__stdcall *DirectOutput_RegisterDeviceCallback)(Pfn_DirectOutput_Device_Callback pfnCallback, void* pvParam);
	const char * do_regDevCallbackFun = "DirectOutput_RegisterDeviceCallback";
	typedef HRESULT (*DirectOutput_Enumerate)(Pfn_DirectOutput_EnumerateCallback pfnCallback, void* pvParam);
	const char * do_enumCallbackFun = "DirectOutput_Enumerate";

	typedef HRESULT (*DirectOutput_RegisterSoftButtonCallback)(void* hDevice, Pfn_DirectOutput_SoftButton_Callback pfnCallback, void* pvContext);
	const char * do_regSoftBtnCallbackFun = "DirectOutput_RegisterSoftButtonCallback";
	typedef HRESULT (*DirectOutput_RegisterPageCallback)(void* hDevice, Pfn_DirectOutput_Page_Callback pfnCallback, void* pvContext);
	const char * do_regPageCallback = "DirectOutput_RegisterPageCallback";

	typedef HRESULT (*DirectOutput_AddPage)(void* hDevice, DWORD dwPage, DWORD dwFlags);
	const char * do_addPageFun = "DirectOutput_AddPage";

	typedef HRESULT (*DirectOutput_SetLed)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwValue);
	const char * do_setLedFun = "DirectOutput_SetLed";
	typedef HRESULT (*DirectOutput_SetString)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchValue, const wchar_t* wszValue);
	const char * do_setStrFun = "DirectOutput_SetString";
}

HINSTANCE directOutputDll;
DirectOutput_AddPage addPage;
DirectOutput_RegisterSoftButtonCallback registerSoftButtonCallback;
DirectOutput_RegisterPageCallback registerPageCallback;
DirectOutput_SetLed setLed;
DirectOutput_SetString setString;
BSTR callbackActionName = nullptr;

EDDCallBacks callbacks;

const DWORD FLAG_SET_AS_ACTIVE = 0x00000001;

const int ledCount = 20;
const wchar_t * leds[ledCount] = {
	L"FIRE",
	L"FIRE_A_RED",
	L"FIRE_A_GREEN",
	L"FIRE_B_RED",
	L"FIRE_B_GREEN",
	L"FIRE_D_RED",
	L"FIRE_D_GREEN",
	L"FIRE_E_RED",
	L"FIRE_E_GREEN",
	L"TOGGLE_1_2_RED",
	L"TOGGLE_1_2_GREEN",
	L"TOGGLE_3_4_RED",
	L"TOGGLE_3_4_GREEN",
	L"TOGGLE_5_6_RED",
	L"TOGGLE_5_6_GREEN",
	L"POV_2_RED",
	L"POV_2_GREEN",
	L"CLUTCH_RED",
	L"CLUTCH_GREEN",
	L"THROTTLE"
};

const int statesCount = 2;
const wchar_t * states[statesCount] = {
	L"OFF",
	L"ON"
};

const int linesCount = 3;
const wchar_t * lines[linesCount] = {
	L"TOP",
	L"MIDDLE",
	L"BOTTOM"
};

const BSTR setLEDCommand = ::SysAllocString(L"setLED");
const BSTR setStringCommand = ::SysAllocString(L"setString");
const BSTR addPageCommand = ::SysAllocString(L"addPage");
const BSTR setCallbackCommand = ::SysAllocString(L"setCallback");

const BSTR version = ::SysAllocString(L"0.0.0.1");
const wchar_t * name = L"EDDiscovery";

const _bstr_t libDependencies = "\\DirectOutput.dll";

int activePage = 0;
void * activeDevice;

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

DWORD ledStates[ledCount] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};
std::vector<std::vector<BSTR>> mfd_text;

void addPageToVec() {
	std::vector<BSTR> newPage;
	for (int i = 0; i < linesCount; i++) {
		newPage.push_back(BSTR(""));
	}
	mfd_text.push_back(newPage);
	
}

void WriteASCII(const char* str)		// ASCII file..
{
	std::fstream fstream("c:\\code\\eddif.txt", std::ios::app);         // open the file
	fstream << str;
	fstream.close();
}

void WriteUnicode(LPCTSTR buffer)
{
	char buffer2[30000];
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, buffer2, sizeof(buffer2), 0, 0);		// need to convert back to ASCII
	WriteASCII((const char*)buffer2);
}

void __stdcall DirectOutput_Device_Callback(void* hDevice, bool bAdded, void* pvContext) {
	if (bAdded) {
		activeDevice = hDevice;
	}
	else {
		activeDevice = nullptr;
	}
}

void __stdcall DirectOutput_SoftButton_Callback(void* hDevice, DWORD dwButtons, void* pvContext) {
	if (callbackActionName != nullptr) {
		const OLECHAR out(dwButtons);
		callbacks.RunAction(callbackActionName, ::SysAllocString(&out));
	}
	else {
		WriteASCII("No Soft Button Callback set.");
	}
}

void __stdcall DirectOutput_Page_Callback(void* hDevice, DWORD dwPage, bool bActivated, void* pvContext) {
	if (bActivated) {
		activePage = dwPage;

		//Stuff needs to be implemented manually, because DirectOutput does not work properly, yet forces the use of pages. The problem is, when you change page, everything gets erased.
		for (int i = 0; i < ledCount; i++) {
			setLed(activeDevice, dwPage, i, ledStates[i]);
		}

		for (int i = 0; i < linesCount; i++) {
			setString(activeDevice, dwPage, i, (DWORD) wcslen(mfd_text[activePage][i]), mfd_text[activePage][i]);
		}
	}
}

void __stdcall DirectOutput_Enumerate_Callback(void* hDevice, void* pvContext) {
	activeDevice = hDevice;

	registerPageCallback(hDevice, *DirectOutput_Page_Callback, nullptr);
	registerSoftButtonCallback(hDevice, *DirectOutput_SoftButton_Callback, nullptr);
}

EDD_API BSTR __cdecl EDDInitialise(BSTR ver, BSTR folder, EDDCallBacks pCallbacks) {
	WriteASCII("EDDX52 says \"Hello there!\"\nEDDX52 Version: 0.0.0.1\n");
	HINSTANCE directOutputDll = LoadLibrary(folder + libDependencies);
	if (directOutputDll) {
		DirectOutput_Initialize init = (DirectOutput_Initialize)GetProcAddress(directOutputDll, do_initFun);
		init(name);

		registerSoftButtonCallback = (DirectOutput_RegisterSoftButtonCallback)GetProcAddress(directOutputDll, do_regSoftBtnCallbackFun);
		registerPageCallback = (DirectOutput_RegisterPageCallback)GetProcAddress(directOutputDll, do_regPageCallback);

		DirectOutput_RegisterDeviceCallback regDev = (DirectOutput_RegisterDeviceCallback)GetProcAddress(directOutputDll, do_regDevCallbackFun);
		regDev(*DirectOutput_Device_Callback, nullptr);
		DirectOutput_Enumerate enumerate = (DirectOutput_Enumerate)GetProcAddress(directOutputDll, do_enumCallbackFun);
		enumerate(*DirectOutput_Enumerate_Callback, nullptr);

		addPage = (DirectOutput_AddPage)GetProcAddress(directOutputDll, do_addPageFun);
		addPage(activeDevice, activePage, FLAG_SET_AS_ACTIVE);
		addPageToVec();

		
		setLed = (DirectOutput_SetLed)GetProcAddress(directOutputDll, do_setLedFun);
		setString = (DirectOutput_SetString)GetProcAddress(directOutputDll, do_setStrFun);

		callbacks = pCallbacks;
	}
	else {
		return NULL;
	}
    
	return ::SysAllocString(L"0.0.0.1");
}

EDD_API BSTR __cdecl EDDActionCommand(BSTR action, SAFEARRAY& args) {
	if (0 == wcscmp(action, setLEDCommand)) {
		wchar_t * led;
		wchar_t * state;
		long index1 = 0;
		long index2 = 1;
		::SafeArrayGetElement(&args, &index1, &led);
		::SafeArrayGetElement(&args, &index2, &state);

		DWORD dwLed = 0;
		DWORD dwState = 0;

		for (int i = 0; i < ledCount; i++) {
			if (0 == wcscmp(leds[i], led)) {
				dwLed = i;
				break;
			}
		}

		for (int i = 0; i < statesCount; i++) {
			if (0 == wcscmp(states[i], state)) {
				dwState = i;
				break;
			}
		}
		
		ledStates[dwLed] = dwState;
		setLed(activeDevice, activePage, dwLed, dwState);

		return ::SysAllocString(L"TRUE");
	}
	else if (0 == wcscmp(action, setStringCommand)) {
		wchar_t * line;
		wchar_t * text;
		wchar_t * page;
		long index1 = 0;
		long index2 = 1;
		long index3 = 2;
		::SafeArrayGetElement(&args, &index1, &line);
		::SafeArrayGetElement(&args, &index2, &text);
		::SafeArrayGetElement(&args, &index3, &page);

		DWORD dwPage = _wtol(page);
		DWORD dwLine = 0;

		for (int i = 0; i < linesCount; i++) {
			if (lines[i] == line) {
				dwLine = i;
				break;
			}
		}

		setString(activeDevice, dwPage, dwLine, (DWORD) wcslen(text), text);
		mfd_text[dwPage][dwLine] = text;

		return ::SysAllocString(L"TRUE");
	}
	else if (0 == wcscmp(action, addPageCommand)) {
		addPage(activeDevice, (DWORD) mfd_text.size(), 0);
		const OLECHAR out = (const OLECHAR) mfd_text.size();
		addPageToVec();
		return ::SysAllocString(&out);
	}
	else if (0 == wcscmp(action, setCallbackCommand)) {
		long index1 = 0;
		::SafeArrayGetElement(&args, &index1, &callbackActionName);
		return ::SysAllocString(L"TRUE");
	}

	return ::SysAllocString(L"FALSE");
}

//EDD_API void __cdecl EDDTerminate()
//{
//	WriteASCII("Goodbye from EDDX52");
//	DirectOutput_Deinitialize deinit = (DirectOutput_Deinitialize)GetProcAddress(directOutputDll, do_deInitFun);
//	deinit();
//}