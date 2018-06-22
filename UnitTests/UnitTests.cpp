// UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>
#include <comdef.h>
#include <iostream>

void wrapWcharP(const wchar_t * &str, wchar_t (&out)[3][16]) {
	size_t length = wcslen(str);

	int lastWrap = 0;
	int wrapPossible = 0;
	int line = 0;
	for (int i = 0; i < length; i++) {
		if (line > 2)
			break;

		for (int o = 0; o < 16; o++) {
			if (i >= length)
				break;

			if (str[i] == L' ') {
				wrapPossible = i;
			}

			i++;
		}

		if (wrapPossible == lastWrap)
			wrapPossible = lastWrap + 15;

		for (int o = lastWrap; o < wrapPossible; o++) {
			out[line][o - lastWrap] = str[o];
		}

		lastWrap = wrapPossible;

		i = wrapPossible + 2;

		line++;
	}
}

int main()
{
	const wchar_t * str = L"This is a simple test for wrapping text";
	wchar_t out[3][16] = {
		L"               ",
		L"               ",
		L"               "
	};

	wrapWcharP(str, out);

	std::cout << out[0] << std::endl;
	std::cout << out[1] << std::endl;
	std::cout << out[2] << std::endl;

	int x;
	std::cin >> x;

    return 0;
}