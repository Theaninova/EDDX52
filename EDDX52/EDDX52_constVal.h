#pragma once
#include "stdafx.h"

//Version stuff
const static std::string version = "1.0";
const static wchar_t * bsVersion = L"1.0";
const static _bstr_t name = L"EDDX52";
//Command names
const static _bstr_t setLEDCommand = L"setLED";
const static _bstr_t setLEDAdvancedCommand = L"advSetLED";
const static _bstr_t setStringCommand = L"setString";
const static _bstr_t getLinesCountCommand = L"getLinesCount";
const static _bstr_t resetMFDCommand = L"resetMFD";
const static _bstr_t setLineCommand = L"setLine";
const static _bstr_t resetLedCommand = L"resetLED";
const static _bstr_t resetCommand = L"reset";
const static _bstr_t turnAllLedOffCommand = L"turnAllLEDOff";
const static _bstr_t syncAllFlashPtrnCommand = L"syncFlashPatterns";
const static _bstr_t permaSyncToCommand = L"permaSyncTo";
const static _bstr_t desyncCommand = L"desync";
const static _bstr_t peakMfdCommand = L"peakMFD";
//Error and return codes
const static _bstr_t errorTooFewParam = L"!TOO_FEW_PARAM";
const static _bstr_t errorInvalidAct = L"!INVALID_ACTION ";
const static _bstr_t errorLedNotFound = L"!LED_NOT_FOUND";
const static _bstr_t errorStateNotFound = L"!LED_STATE_NOT_FOUND";
const static _bstr_t success = L"SUCCESS";

//Stuff for Simplified LED control
const static int ledCount = 20;
const static int simpleLedCount = 11;
struct ButtonLed {
	_bstr_t name;
	DWORD redCompName;
	DWORD greenCompName;
};
const static ButtonLed simpleLeds[simpleLedCount] = {
	{ L"FIRE", 0, 0 },
{ L"FIRE_A", 1, 2 },
{ L"FIRE_B", 3, 4 },
{ L"FIRE_D", 5, 6 },
{ L"FIRE_E", 7, 8 },
{ L"TOGGLE_1_2", 9, 10 },
{ L"TOGGLE_3_4", 11, 12 },
{ L"TOGGLE_5_6", 13, 14 },
{ L"POV_2", 15, 16 },
{ L"CLUTCH", 17, 18 },
{ L"THROTTLE", 19, 19 }
};
//Still simplified LED stuff
const static int simpleLedStatesCount = 5;
struct LedState {
	_bstr_t name;
	DWORD stateRed;
	DWORD stateGreen;
};
const static LedState simpleLedStates[simpleLedStatesCount]{
	{ L"ON", 1, 1 },
{ L"OFF", 0, 0 },
{ L"RED", 1, 0 },
{ L"GREEN", 0, 1 },
{ L"YELLOW", 1, 1 }
};
//MFD Entry with callback support
struct mfd_entry {
	_bstr_t name;
	_bstr_t callbackName;
};
//Flash Patterns
struct timestamp {
	DWORD turnOn;
	double time;
};
struct flashPattern {
	bool enabled;

	int repeat;
	double repeatOffset;
	double flashDuration;
	double flashOffset;

	double * progress;

	std::vector<timestamp> schedule;
	bool needsSchedule;
};