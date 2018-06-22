#include "static_help_functions.h"

void wrapWcharP(const wchar_t * &str, wchar_t(&out)[3][16]) {
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

		lastWrap = wrapPossible + 1;

		i = wrapPossible;

		line++;
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