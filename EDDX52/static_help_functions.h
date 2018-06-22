#pragma once
#include "stdafx.h"
#include <fstream>
#include <thread>
#include <chrono>

void wrapWcharP(const wchar_t * &str, wchar_t(&out)[3][16]);
void WriteASCII(const std::string str);
void WriteUnicode(LPCTSTR buffer);

int determineStringLength(const wchar_t * str);
void performMfdEnterAnimation(DirectOutput_SetString &setString, wchar_t(&out)[3][16], wchar_t(&display)[3][16], int(&length)[3], void * activeDevice, DWORD workPage);
void performMfdTransition(DirectOutput_SetString &setString, wchar_t(&newContent)[3][16], long transitionDurationMS, void * activeDevice, DWORD workPage);
void performMfdTransition(DirectOutput_SetString &setString, std::vector<mfd_entry> mfd_text, int mfd_line, long transitionDurationMS, void * activeDevice, DWORD workPage);