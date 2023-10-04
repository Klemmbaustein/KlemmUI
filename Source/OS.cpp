#include "OS.h"
#if _WIN32
//Include Windows Headers
#include <Windows.h>
#endif


// TODO: Make better
#if _WIN32
std::wstring OS::Utf8ToWstring(std::string utf8)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, wstr, wchars_num);
	// do whatever with wstr
	std::wstring str = wstr;
	delete[] wstr;
	return str;
}
#else
std::wstring OS::Utf8ToWstring(std::string utf8)
{
	return std::wstring(utf8.begin(), utf8.end());
}
#endif