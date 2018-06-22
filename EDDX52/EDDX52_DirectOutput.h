#pragma once
#include "stdafx.h"

extern "C" {
	typedef void __stdcall Pfn_DirectOutput_Device_Callback(void* hDevice, bool bAdded, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_EnumerateCallback(void* hDevice, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_SoftButton_Callback(void* hDevice, DWORD dwButtons, void* pvContext);
	typedef void __stdcall Pfn_DirectOutput_Page_Callback(void* hDevice, DWORD dwPage, bool bActivated, void* pvContext);

	const DWORD FLAG_SET_AS_ACTIVE = 0x00000001;

	const DWORD SoftButton_Select = 0x00000001; // X52Pro ScrollClick
	const DWORD SoftButton_Up = 0x00000002; // X52Pro ScrollUp, FIP RightScrollClockwize
	const DWORD SoftButton_Down = 0x00000004; // X52Pro ScrollDown, FIP RightScrollAnticlockwize

	typedef HRESULT(__stdcall *DirectOutput_Initialize)(const wchar_t * wszAppName);
	const char * do_initFun = "DirectOutput_Initialize";
	typedef HRESULT(__stdcall *DirectOutput_Deinitialize)();
	const char * do_deInitFun = "DirectOutput_Deinitialize";

	typedef HRESULT(__stdcall *DirectOutput_RegisterDeviceCallback)(Pfn_DirectOutput_Device_Callback pfnCallback, void* pvParam);
	const char * do_regDevCallbackFun = "DirectOutput_RegisterDeviceCallback";
	typedef HRESULT(*DirectOutput_Enumerate)(Pfn_DirectOutput_EnumerateCallback pfnCallback, void* pvParam);
	const char * do_enumCallbackFun = "DirectOutput_Enumerate";

	typedef HRESULT(*DirectOutput_RegisterSoftButtonCallback)(void* hDevice, Pfn_DirectOutput_SoftButton_Callback pfnCallback, void* pvContext);
	const char * do_regSoftBtnCallbackFun = "DirectOutput_RegisterSoftButtonCallback";
	typedef HRESULT(*DirectOutput_RegisterPageCallback)(void* hDevice, Pfn_DirectOutput_Page_Callback pfnCallback, void* pvContext);
	const char * do_regPageCallback = "DirectOutput_RegisterPageCallback";

	typedef HRESULT(*DirectOutput_AddPage)(void* hDevice, DWORD dwPage, DWORD dwFlags);
	const char * do_addPageFun = "DirectOutput_AddPage";
	typedef HRESULT(*DirectOutput_RemovePage)(void* hDevice, DWORD dwPage);
	const char * do_removePageFun = "DirectOutput_RemovePage";

	typedef HRESULT(*DirectOutput_SetLed)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD dwValue);
	const char * do_setLedFun = "DirectOutput_SetLed";
	typedef HRESULT(*DirectOutput_SetString)(void* hDevice, DWORD dwPage, DWORD dwIndex, DWORD cchValue, const wchar_t* wszValue);
	const char * do_setStrFun = "DirectOutput_SetString";
}