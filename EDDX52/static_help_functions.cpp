#include "stdafx.h"
#include "static_help_functions.h"

void wrapWcharP(const wchar_t * &str, wchar_t(&out)[3][16]) {
	size_t length = wcslen(str);

	int lastWrap = 0;
	int wrapPossible = 0;
	int line = 0;
	for (int i = 0; i < length; i++) {
		if (line > 2) {
			break;
		}

		for (int o = 0; o < 16; o++) {
			if (i >= length) {
				for (int o = lastWrap; o < length; o++) {
					out[line][o - lastWrap] = str[o];
				}

				return;
			}

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

		lastWrap = wrapPossible + 1;

		i = wrapPossible;

		line++;
	}

	for (int i = lastWrap; i < 13; i++) {
		out[2][i] = str[lastWrap + i];
	}

	out[2][13] = '.';
	out[2][14] = '.';
	out[2][15] = '.';
}

void performMfdTransition(DirectOutput_SetString &setString, std::vector<mfd_entry> mfd_text, int mfd_line /* DON'T MAKE IT A REFERENCE*/, long transitionDurationMS, void * activeDevice, DWORD workPage) {
	wchar_t newContent[3][16] = {
		L"               ",
		L"               ",
		L"               "
	};

	wchar_t * name;
	size_t length = mfd_text.size();
	size_t length2;
	for (mfd_line; mfd_line < length; mfd_line++) {
		name = mfd_text[length - mfd_line - 1].name.GetBSTR();
		length2 = mfd_text[length - mfd_line - 1].name.length();
		for (int z = 0; z < 16; z++) {
			if (z < length2)
				newContent[mfd_line][z] = name[z];
		}
	}

	performMfdTransition(setString, newContent, transitionDurationMS, activeDevice, workPage);
}

void performMfdTransition(DirectOutput_SetString &setString, wchar_t (&newContent)[3][16], long transitionDurationMS, void * activeDevice, DWORD workPage) {
	auto pause = std::chrono::milliseconds(transitionDurationMS / 6);

	for (int i = 2; i >= 0; i--) {
		setString(activeDevice, workPage, i, 16, L"                ");
		std::this_thread::sleep_for(pause);
	}

	for (int i = 0; i < 3; i++) {
		setString(activeDevice, workPage, i, wcslen(newContent[i]), newContent[i]);
		std::this_thread::sleep_for(pause);
	}
}

void WriteASCII(const std::string str)		// ASCII file..
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

int determineStringLength(const wchar_t * str) {
	size_t length = wcslen(str);

	int out = 0;
	for (int i = 0; i < length; i++) {
		if (str[i] != ' ') {
			out = i;
		}
	}

	return out;
}

void performMfdEnterAnimation(DirectOutput_SetString &setString, wchar_t (&out)[3][16], wchar_t (&display)[3][16], int (&length)[3], void * activeDevice, DWORD workPage)
{
	using namespace std::chrono_literals;

	for (int z = 0; z < 3; z++) {
		setString(activeDevice, workPage, z, 16, display[z]);
		for (int i = 0; i < length[z]; i++) {
			std::this_thread::sleep_for(50ms);
			display[z][i] = out[z][i];
			display[z][i + 1] = '_';
			setString(activeDevice, workPage, z, 16, display[z]);
		}
		display[z][length[z]] = out[z][length[z]];
		std::this_thread::sleep_for(50ms);
		setString(activeDevice, workPage, z, 16, display[z]);
	}
}