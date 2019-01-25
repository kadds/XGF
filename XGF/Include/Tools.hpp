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
		// name without extension
		string GetFontPath(const string & name);
		void GetAppPath(wchar_t * buffer, int len);
		std::string WcharToChar(const wchar_t* wch, size_t encode = CP_ACP);
		std::pair<std::unique_ptr<char>, int> LoadFromFile(const std::string & file);
		std::pair<std::unique_ptr<char>, int> LoadFromFile(const std::wstring & file);

		void SetCurrentThreadName(const char * name);
	};
#define GetFilePath(filename, buffer, maxlen) Tools::GetPathBy(filename, buffer, maxlen)


}
