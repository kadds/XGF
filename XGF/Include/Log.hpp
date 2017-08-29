#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <fstream>
#include <time.h>

/*
日记class
*未完成
*/
class Log {
public:
	void Warning(const char * str, const char * file, int line, const char * funName = nullptr);
	void CheckHR(HRESULT hr, const char * file, int line, const char * funName = nullptr);
	void CheckHR(LPCWSTR str, HRESULT hr, const char * file, int line, const char * funName = nullptr);
	void CheckHR(LPCSTR str, HRESULT hr, const char * file, int line, const char * funName = nullptr);
	void Error(const char *str, const char * file, int line, const char * funName = nullptr, bool msgbox = true);
	static Log * GetInstance() {
		return &mInstance;
	}

private:
	std::ofstream mLogfile;
	Log();
	~Log();
	static Log mInstance;
	bool Used; 
	char filename[260];
	void GetHRString(char * out, int size, const char * msg ,HRESULT hr);
};
void OutputDebugStringEx(const wchar_t *strOutputString, ...);
void OutputDebugStringEx(const char *strOutputString, ...);

#ifdef _DEBUG
#define DebugOut(x) OutputDebugStringA(x);
#else
#define DebugOut(x)  ;
#endif

#define ERROR_SHADER_FILE_NOT_FIND_STRING "Shader file not find!"
#define ERROR_BATCH_MEM_OUT_OF_RANGE "Shader file not find!"

#ifdef _DEBUG
#define _USE_FUNCTION_NAME
#endif

#ifdef _USE_FUNCTION_NAME
#define _FUNNAME_XGF_ __FUNCDNAME__
#else
#define _FUNNAME_XGF_ nullptr
#endif
#define Check(hr) Log::GetInstance()->CheckHR(hr,__FILE__,__LINE__,_FUNNAME_XGF_)
#define CheckEx(str,hr) Log::GetInstance()->CheckHR(str,hr,__FILE__,__LINE__,_FUNNAME_XGF_)
#define ReportError(str) Log::GetInstance()->Error(str,__FILE__,__LINE__,_FUNNAME_XGF_)
#define ReportWarning(str) Log::GetInstance()->Warning(str,__FILE__,__LINE__,_FUNNAME_XGF_)
