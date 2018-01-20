#pragma once
#include "Defines.hpp"
#include <Windows.h>
namespace XGF
{
	/*
	简单的工具类
	*/
	namespace Tools
	{
		wchar_t * GetPathBy(const wchar_t *filename, wchar_t * buffer, int maxlen);
		char * GetPathBy(const char *filename, char * buffer, int maxlen);
		void GetFontPath(const char * name, char *buffer, int maxlen);
		void GetAppPath(wchar_t * buffer, int len);

	};
#define GetFilePath(filename, buffer, maxlen) Tools::GetPathBy(filename, buffer, maxlen)


}
