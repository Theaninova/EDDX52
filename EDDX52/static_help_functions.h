#pragma once
#include "stdafx.h"
#include <fstream>

void wrapWcharP(const wchar_t * &str, wchar_t(&out)[3][16]);
void WriteASCII(const std::string str);
void WriteUnicode(LPCTSTR buffer);