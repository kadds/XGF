#pragma once
#include "Defines.hpp"
#include <Windows.h>
/*
简单的工具类
*/
class Tools
{
public:
	static Tools* GetInstance();
	wchar_t * GetPathBy(wchar_t *filename, wchar_t * buffer, int maxlen);
	char * GetPathBy(char *filename, char * buffer, int maxlen);
	void GetFontPath(const char * name, char *buffer, int maxlen);
private:
	void GetAppPath(wchar_t * buffer, int len);
	static Tools * mInstance;
	DISALLOW_COPY_AND_ASSIGN(Tools);
	Tools();
	~Tools();
};
#define GetFilePath(filename, buffer, maxlen) Tools::GetInstance()->GetPathBy(filename, buffer, maxlen)

