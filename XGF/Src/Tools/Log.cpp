#include "../../Include/Log.hpp"
#include "../../Include/dxerr.h"
#include <sstream> 
#include <locale>
#include <iomanip>
Log Log::mInstance;
void Log::Warning(const char * str, const char * file, int line, const char * funName)
{
	Used = true;
	char *buffer;
	int len;
	if (funName != nullptr)
	{
		len = static_cast<int>(strlen(str) + strlen(file) + strlen(funName)) + 1000;
		buffer = new char[len];
		sprintf_s(buffer, len, "Warning: %s\nSource File:%s\nLine:%d\nFunction name:%s", str, file, line, funName);
	}
	else
	{
		len = static_cast<int>(strlen(str) + strlen(file)) + 1000;
		buffer = new char[len];
		sprintf_s(buffer, len, "Warning: %s\nSource File:%s\nLine:%d\nno function name", str, file, line);
	}
	mLogfile << buffer << std::endl;
	delete[] buffer;
}
void Log::CheckHR(HRESULT hr, const char * file, int line, const char * funName)
{
	if (!SUCCEEDED(hr))
	{
		char msg[240];
		GetHRString(msg, 240, nullptr, hr);
		Error(msg, file, line, funName);
	}
}
void Log::CheckHR(LPCWSTR str, HRESULT hr, const char * file, int line, const char * funName)
{
	if (!SUCCEEDED(hr))
	{
		char cstr[100];
		size_t converted = 0;
		wcstombs_s(&converted, cstr, wcslen(str) + 1, str, _TRUNCATE);
		char msg[240];
		GetHRString(msg, 240, cstr, hr);
		Error(msg, file, line, funName);
	}
}
void Log::CheckHR(LPCSTR str, HRESULT hr, const char * file, int line, const char * funName)
{
	if (!SUCCEEDED(hr))
	{
		char *msg = new char[8192];
		GetHRString(msg, 8192, str, hr);
		Error(msg, file, line, funName);
		delete[] msg;
	}
}
void XGF_OutputDebugStringEx(const wchar_t *strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
	wchar_t *strBuffer = new wchar_t[nLen];
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
	delete[] strBuffer;
}
void XGF_OutputDebugStringEx(const char *strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char *strBuffer = new char[nLen];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	delete[] strBuffer;
}

void Log::Error(const char * str, const char * file, int line, const char * funName, bool msgbox)
{
	Used = true;
	char *buffer;
	int len;
	if (funName != nullptr)
	{
		len = static_cast<int>(strlen(str) + strlen(file) + strlen(funName)) + 500;
		buffer = new char[len];
		sprintf_s(buffer, len, "Error: %s\nSource File:%s\nLine:%d\nFunction name:%s", str, file, line, funName);
	}
	else
	{
		len = static_cast<int>(strlen(str) + strlen(file)) + 500;
		buffer = new char[len];
		sprintf_s(buffer, len, "Error: %s\nSource File:%s\nLine:%d\nNo function name", str, file, line);
	}
	mLogfile << buffer << std::endl << std::fflush;
	if(msgbox)
		MessageBoxA(GetForegroundWindow(), buffer, "Error", MB_ICONERROR);
	exit(-3);
}

Log::Log():Used(false)
{
	const time_t tim = time(NULL);
	sprintf_s(filename, sizeof(filename), "Log.%lld.log", tim);
	mLogfile.open(filename, std::ios::out);
	std::time_t t = std::time(NULL);
	std::tm timeinfo;
	localtime_s(&timeinfo, &t);
	mLogfile.imbue(std::locale("CHS"));
	mLogfile << "DateTime:" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << std::endl << std::endl;
}

Log::~Log()
{
	mLogfile.flush();
	mLogfile.close();
	if (!Used)
		DeleteFileA(filename);
}

void Log::GetHRString(char * out, int size, const char * msg, HRESULT hr)
{
	char *wdxerr = new char[1024];
	size_t converted = 0;
	const wchar_t * estring = DXGetErrorStringW(hr);
	wcstombs_s(&converted, wdxerr, wcslen(estring) + 1, estring, _TRUNCATE);
	if (msg!= nullptr)
	{
		sprintf_s(out, size, "DXerror: %s\n%s", wdxerr, msg);
	}
	else
	{
		sprintf_s(out, size, "DXerror: %s", wdxerr);
	}
	delete[]wdxerr;
}
