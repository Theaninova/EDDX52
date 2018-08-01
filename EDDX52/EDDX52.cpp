// EDDX52.cpp : Defines the exported functions for the DLL application.
// Direct Output Pages are VERY buggy, or better say they don't work. So I had to implement the system by myself (MFD only)
#pragma once

#include "stdafx.h"
#include "EDDInterface64.h"
#include "static_help_functions.h"
#include <fstream>
#include <thread>

//Direct Output functions
HINSTANCE directOutputDll;
DirectOutput_AddPage addPage;
DirectOutput_RegisterSoftButtonCallback registerSoftButtonCallback;
DirectOutput_RegisterPageCallback registerPageCallback;
DirectOutput_SetLed setLed;
DirectOutput_SetString setString;
DirectOutput_RemovePage removePage;
DirectOutput_Deinitialize deInitDO;
//EDD Callbacks
EDDCallBacks callbacks;

const int workPage = 0;
int mfd_line = 0;
void * activeDevice;

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

std::vector<mfd_entry> mfd_text;

bool flashPatternRequiresSync = false;
double flashProgresses[ledCount]{
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
flashPattern flashPatterns[ledCount]{
	{ false, 0, 0, 0, 0, &flashProgresses[0], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[1], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[2], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[3], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[4], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[5], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[6], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[7], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[8], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[9], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[10], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[11], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[12], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[13], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[14], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[15], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[16], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[17], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[18], std::vector<timestamp>(), true },
	{ false, 0, 0, 0, 0, &flashProgresses[19], std::vector<timestamp>(), true },
};
void generateFlashSchedule(flashPattern * pattern) {
	pattern->schedule.clear();
	double time = 0.0;
	for (int i = 0; i < pattern->repeat; i++) {
		pattern->schedule.push_back({ 1, time });
		time += pattern->flashDuration;
		pattern->schedule.push_back({ 0, time });
		time += pattern->flashOffset;
	}
	time += pattern->repeatOffset;
	pattern->schedule.push_back({ 1, time });
}

std::chrono::steady_clock::time_point _start;
std::chrono::steady_clock::time_point _end;
double multi;
bool run = true;
timestamp * currStamp;
void ledFlashServiceLoop() {
	using namespace std::chrono_literals;

	_end = std::chrono::steady_clock::now();
	std::this_thread::sleep_for(50ms);

	while (run) {
		_start = std::chrono::steady_clock::now();
		multi = std::chrono::duration_cast<std::chrono::duration<double>>(_start - _end).count();
		_end = _start;

		if (flashPatternRequiresSync) {
			for (int i = 0; i < ledCount; i++) {
				flashProgresses[i] = 0.0;
			}
			flashPatternRequiresSync = false;
		}

		for (int i = 0; i < ledCount; i++) {
			if (flashPatterns[i].enabled) {
				if (flashPatterns[i].needsSchedule) {
					generateFlashSchedule(&flashPatterns[i]);
					flashPatterns[i].needsSchedule = false;
				}

				if (*flashPatterns[i].progress > flashPatterns[i].schedule[flashPatterns[i].schedule.size() - 1].time) {
					*flashPatterns[i].progress = flashPatterns[i].schedule[flashPatterns[i].schedule.size() - 1].time - *flashPatterns[i].progress;
				}

				for (timestamp &stamp : flashPatterns[i].schedule) {
					if (stamp.time <= *flashPatterns[i].progress) {
						currStamp = &stamp;
					}
					else {
						break;
					}
				}

				setLed(activeDevice, workPage, i, currStamp->turnOn);

				flashProgresses[i] += multi;
			}
		}
		std::this_thread::sleep_for(25ms);

		//_end = std::chrono::steady_clock::now();
	}
}
std::thread flashServiceTask;

void scrollMfd(int newLine) {
	if (newLine >= 0 && newLine < (int)(mfd_text.size())) {
		mfd_line = newLine;

		for (int i = 0; i < 3; i++) {
			if (mfd_line + i >= 0 && mfd_line + i < mfd_text.size())
				setString(activeDevice, workPage, i, (DWORD)wcslen(mfd_text[mfd_line + i].name), mfd_text[mfd_line + i].name);
			else
				setString(activeDevice, workPage, i, 0, L"");
		}
	}
	else if (newLine < 0) {
		mfd_line = 0;
		scrollMfd(mfd_line);
	}
	else if (newLine >= (int)(mfd_text.size())) {
		mfd_line = (int)(mfd_text.size()) - 1;
		scrollMfd(mfd_line);
	}
}

void refreshMfd() {
	scrollMfd(mfd_line);
}

const wchar_t * peakMfdCallback;
bool stopScrolling = false;
wchar_t peakMfdLines[3][16];
double peakMfdDuration;
void peakMfdService() {
	using namespace std::chrono_literals;

	double runDuration = 0.0;
	_end = std::chrono::steady_clock::now();

	int length[3] = { 
		determineStringLength(peakMfdLines[0]), 
		determineStringLength(peakMfdLines[1]), 
		determineStringLength(peakMfdLines[2]) 
	};

	performMfdTransition(setString, peakMfdLines, stdTransitionDuration, activeDevice, workPage);

	int lastLine = 2;
	if (0 == wcscmp(peakMfdLines[2], L"")) {
		if (0 == wcscmp(peakMfdLines[1], L""))
			lastLine = 0;
		else
			lastLine = 1;
	}

	wchar_t lastChar = L' ';
	int lastCharInt = 1;
	if (length[lastLine] >= 15) {
		lastChar = peakMfdLines[lastLine][15];
		lastCharInt = 0;
	}
	
	while (runDuration < peakMfdDuration) {
		_start = std::chrono::steady_clock::now();
		multi = std::chrono::duration_cast<std::chrono::duration<double>>(_start - _end).count();
		_end = _start;

		peakMfdLines[lastLine][length[lastLine] + lastCharInt] = '_';
		setString(activeDevice, workPage, lastLine, 16, peakMfdLines[lastLine]);
		std::this_thread::sleep_for(500ms);
		peakMfdLines[lastLine][length[lastLine] + lastCharInt] = lastChar;
		setString(activeDevice, workPage, lastLine, 16, peakMfdLines[lastLine]);
		std::this_thread::sleep_for(500ms);

		runDuration += multi + 1;
	}

	performMfdTransition(setString, mfd_text, mfd_line, stdTransitionDuration, activeDevice, workPage);

	refreshMfd();

	return;
}
std::thread peakMfdServiceTask;

void turnAllLedOff() {
	for (int i = 0; i < ledCount; i++) {
		setLed(activeDevice, workPage, i, 0);
		flashPatterns[i].enabled = false;
	}
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
	if (dwButtons == SoftButton_Up && !stopScrolling) {
		scrollMfd(mfd_line - 1);
	}
	else if (dwButtons == SoftButton_Down && !stopScrolling) {
		scrollMfd(mfd_line + 1);
	}
	else if (dwButtons == SoftButton_Select) {
		if (stopScrolling) {
			callbacks.RunAction(::SysAllocString(peakMfdCallback), ::SysAllocString(L""));
		}
		else {
			callbacks.RunAction(::SysAllocString(mfd_text[mfd_line].callbackName), ::SysAllocString(L""));
		}
	}
}
void __stdcall DirectOutput_Page_Callback(void* hDevice, DWORD dwPage, bool bActivated, void* pvContext) {
	if (bActivated && workPage == dwPage) {
		//Stuff needs to be implemented manually, because DirectOutput does not work properly, yet forces the use of pages. The problem is, when you change page, everything gets erased.
		for (int i = 0; i < ledCount; i++) {
			setLed(activeDevice, dwPage, i, ledStates[i]);
		}

		scrollMfd(mfd_line);
	}
}
void __stdcall DirectOutput_Enumerate_Callback(void* hDevice, void* pvContext) {
	activeDevice = hDevice;

	registerPageCallback(hDevice, *DirectOutput_Page_Callback, nullptr);
	registerSoftButtonCallback(hDevice, *DirectOutput_SoftButton_Callback, nullptr);
}

EDD_API BSTR __cdecl EDDInitialise(BSTR ver, BSTR folder, EDDCallBacks pCallbacks) {
	WriteASCII("EDDX52 Version: ");
	WriteASCII(version);
	WriteASCII("\n");

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
		addPage(activeDevice, workPage, FLAG_SET_AS_ACTIVE);

		removePage = (DirectOutput_RemovePage)GetProcAddress(directOutputDll, do_removePageFun);

		deInitDO = (DirectOutput_Deinitialize)GetProcAddress(directOutputDll, do_deInitFun);
		
		setLed = (DirectOutput_SetLed)GetProcAddress(directOutputDll, do_setLedFun);
		setString = (DirectOutput_SetString)GetProcAddress(directOutputDll, do_setStrFun);

		turnAllLedOff();

		callbacks = pCallbacks;

		//For blinking LEDs
		flashServiceTask = std::thread(ledFlashServiceLoop);
	}
	else {
		return NULL;
	}
    
	return ::SysAllocString(bsVersion);
}

EDD_API BSTR __cdecl EDDActionCommand(BSTR action, SAFEARRAY& args) {
	if (0 == wcscmp(action, setLEDAdvancedCommand)) {
		const wchar_t *  led;
		const wchar_t *  state;

		const wchar_t * repeats;
		const wchar_t * repeatOffset;
		const wchar_t * flashDuration;
		const wchar_t * flashOffset;

		long index1 = 0;
		long index2 = 1;
		long index3 = 2;
		long index4 = 3;
		long index5 = 4;
		long index6 = 5;

		if (::SafeArrayGetElement(&args, &index1, &led) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &state) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		DWORD dwLed = _wtol(led);
		DWORD dwState = _wtol(state);

		if (!(::SafeArrayGetElement(&args, &index3, &repeats) != S_OK ||
			::SafeArrayGetElement(&args, &index4, &repeatOffset) != S_OK ||
			::SafeArrayGetElement(&args, &index5, &flashDuration) != S_OK ||
			::SafeArrayGetElement(&args, &index6, &flashOffset) != S_OK)) {
			int iRepeats = _wtol(repeats);
			double dRepeatOffset = wcstod(repeatOffset, NULL);
			double dFlashDuration = wcstod(flashDuration, NULL);
			double dFlashOffset = wcstod(flashOffset, NULL);

			flashPatterns[dwLed] = { true, iRepeats, dRepeatOffset, dFlashDuration, dFlashOffset, &flashProgresses[dwLed], std::vector<timestamp>(), true };

			return ::SysAllocString(success);
		}
		
		flashPatterns[dwLed].enabled = false;
		ledStates[dwLed] = dwState;
		setLed(activeDevice, workPage, dwLed, dwState);

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, setLEDCommand)) {
		const wchar_t *  chLed;
		const wchar_t *  chState;
		long index1 = 0;
		long index2 = 1;
		if (::SafeArrayGetElement(&args, &index1, &chLed) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &chState) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		const ButtonLed * led = nullptr;
		const LedState * state = nullptr;

		bool ledFound = false;
		bool stateFound = false;

		for (int i = 0; i < simpleLedStatesCount; i++) {
			if (0 == wcscmp(simpleLedStates[i].name, chState)) {
				state = &simpleLedStates[i];
				stateFound = true;
				break;
			}
		}

		for (int i = 0; i < simpleLedCount; i++) {
			if (0 == wcscmp(simpleLeds[i].name, chLed)) {
				led = &simpleLeds[i];
				ledFound = true;
				break;
			}
		}

		if (!stateFound)
			return ::SysAllocString(errorStateNotFound);
		if (!ledFound)
			return ::SysAllocString(errorLedNotFound);

		ledStates[led->greenCompName] = state->stateGreen;
		ledStates[led->redCompName] = state->stateRed;
		flashPatterns[led->redCompName].enabled = false;
		flashPatterns[led->greenCompName].enabled = false;
		setLed(activeDevice, workPage, led->redCompName, state->stateRed);
		setLed(activeDevice, workPage, led->greenCompName, state->stateGreen);

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, setStringCommand)) {
		const wchar_t * line;
		const wchar_t * text;
		const wchar_t * wcId;
		const wchar_t * callback;
		long index1 = 0;
		long index2 = 1;
		long index3 = 2;
		long index4 = 3;
		if (::SafeArrayGetElement(&args, &index1, &line) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &text) != S_OK ||
			::SafeArrayGetElement(&args, &index3, &wcId) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		if (::SafeArrayGetElement(&args, &index3, &callback) != S_OK) {
			callback = L"";
		}

		DWORD dwLine = _wtol(line);

		int x = (dwLine - (int)mfd_text.size());

		for (int i = 0; i <= x; i++) {
			mfd_text.push_back(mfd_entry { L"", L"", wcId});
		}

		if (dwLine >= mfd_line && dwLine < mfd_line + 3)
			setString(activeDevice, workPage, dwLine - mfd_line, (DWORD) wcslen(text), text);
		mfd_text[dwLine].name = text;
		mfd_text[dwLine].callbackName = callback;

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, getLinesCountCommand)) {
		return ::SysAllocString("+" + _bstr_t(mfd_text.size()));
	}
	else if (0 == wcscmp(action, resetMFDCommand)) {
		mfd_line = 0;
		mfd_text.clear();
		setString(activeDevice, workPage, 0, 0, L"");
		setString(activeDevice, workPage, 1, 0, L"");
		setString(activeDevice, workPage, 2, 0, L"");

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, setLineCommand)) {
		const wchar_t * line;
		long index1 = 0;
		if (::SafeArrayGetElement(&args, &index1, &line) != S_OK)
			return ::SysAllocString(errorTooFewParam);
		int dwLine = _wtol(line);
		scrollMfd(dwLine);
		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, resetLedCommand)) {
		turnAllLedOff();
		flashPatternRequiresSync = true;
		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, resetCommand)) {
		mfd_line = 0;
		mfd_text.clear();
		setString(activeDevice, workPage, 0, 0, L"");
		setString(activeDevice, workPage, 1, 0, L"");
		setString(activeDevice, workPage, 2, 0, L"");

		turnAllLedOff();
		flashPatternRequiresSync = true;

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, turnAllLedOffCommand)) {
		turnAllLedOff();
		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, syncAllFlashPtrnCommand)) {
		flashPatternRequiresSync = true;
		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, permaSyncToCommand)) {
		const wchar_t * chLed1;
		const wchar_t * chLed2;
		long index1 = 0;
		long index2 = 1;
		if (::SafeArrayGetElement(&args, &index1, &chLed1) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &chLed2) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		DWORD led1 = _wtol(chLed1);
		DWORD led2 = _wtol(chLed2);

		//because both are pointers, you are syncing them permanentley.
		flashPatterns[led2].progress = &flashProgresses[led1];

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, desyncCommand)) {
		const wchar_t * chLed;
		long index1 = 0;
		if (::SafeArrayGetElement(&args, &index1, &chLed) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		DWORD led = _wtol(chLed);

		flashPatterns[led].progress = &flashProgresses[led];

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, peakMfdCommand)) {
		const wchar_t * str;
		const wchar_t * duration;
		long index1 = 0;
		long index2 = 1;
		if (::SafeArrayGetElement(&args, &index1, &str) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &duration) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		peakMfdDuration = wcstod(duration, NULL);

		wrapWcharP(str, peakMfdLines);

		peakMfdServiceTask = std::thread(peakMfdService);
		peakMfdServiceTask.detach();

		return ::SysAllocString(success);
	}
	else if (0 == wcscmp(action, removeMfdEntry)) {
		const wchar_t * wcId;
		long index1 = 0;
		if (::SafeArrayGetElement(&args, &index1, &wcId) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		size_t mfd_size = mfd_text.size();
		for (int i = 0; i < mfd_size; i++) {
			if (0 == wcscmp(mfd_text[i].id, wcId)) {
				mfd_text.erase(mfd_text.begin() + i);
				refreshMfd();
				return ::SysAllocString(success);
			}
		}

		return ::SysAllocString(errorElementNotFound);
	}
	else if (0 == wcscmp(action, editMfdEntry)) {
		const wchar_t * wcId;
		const wchar_t * name;
		const wchar_t * callback;
		long index1 = 0;
		long index2 = 1;
		long index3 = 2;
		if (::SafeArrayGetElement(&args, &index1, &wcId) != S_OK ||
			::SafeArrayGetElement(&args, &index2, &name) != S_OK) {
			return ::SysAllocString(errorTooFewParam);
		}

		size_t mfd_size = mfd_text.size();
		for (int i = 0; i < mfd_size; i++) {
			if (0 == wcscmp(mfd_text[i].id, wcId)) {
				mfd_text[i].name = name;
				if (::SafeArrayGetElement(&args, &index3, &callback) == S_OK) {
					mfd_text[i].callbackName = callback;
				}
				refreshMfd();
				return ::SysAllocString(success);
			}
		}

		return ::SysAllocString(errorElementNotFound);
	}

	return ::SysAllocString(errorInvalidAct);
}

EDD_API void EDDTerminate()
{
	run = false;
	flashServiceTask.join();
	deInitDO;
}
