#include "../../Include/Tools.hpp"
#include <Windows.h>
#include <shlobj.h>
Tools* Tools::mInstance = nullptr;

Tools::Tools()
{

}


Tools::~Tools()
{
}

Tools * Tools::GetInstance()
{
	return mInstance;
}

wchar_t * Tools::GetPathBy(wchar_t * filename, wchar_t * buffer, int maxlen)
{
	GetModuleFileName(NULL, buffer, maxlen);
	int p = -2;
	for (int i = 0; i < MAX_PATH + 1; i++)
	{
		if (buffer[i] == L'\0')
			break;
		else if (buffer[i] == L'\\')
			p = i;
		else if (buffer[i] == L'/')
			p = i;
	}
	p++;
	int i = 0;
	if (p >= 0)
		while (filename[i] != L'\0')
		{
			buffer[p + i] = filename[i];//TODO::maybe error!!
			i++;
		}
	else
		return nullptr;
	buffer[p + i] = L'\0';
	return buffer;
}

char * Tools::GetPathBy(char * filename, char * buffer, int maxlen)
{
	GetModuleFileNameA(NULL, buffer, maxlen);
	int p = -2;
	for (int i = 0; i < MAX_PATH + 1; i++)
	{
		if (buffer[i] == '\0')
			break;
		else if (buffer[i] == '\\')
			p = i;
		else if (buffer[i] == '/')
			p = i;
	}
	p++;
	int i = 0;
	if (p >= 0)
		while (filename[i] != '\0')
		{
			buffer[p + i] = filename[i];//TODO::maybe error!!
			i++;
		}
	else
		return nullptr;
	buffer[p + i] = '\0';
	return buffer;
}
struct FontEnumInformation
{
	const char * name;
	char * OutBuffer;
	int len;
};
void Tools::GetFontPath(const char * name, char * buffer, int maxlen)
{
	SHGetSpecialFolderPathA(NULL, buffer, CSIDL_FONTS, FALSE);
	strcat_s(buffer, maxlen, "\\");
	strcat_s(buffer, maxlen, name);
}

void Tools::GetAppPath(wchar_t * t, int len)
{
	GetModuleFileName(NULL, t, len);
	int p = -1;
	for (int i = 0; i < MAX_PATH + 1; i++)
	{
		if (t[i] == '\0')
			break;
		else if (t[i] == '\\')
			p = i;
		else if (t[i] == '/')
			p = i;
	}
	if (p >= 0)
		t[p] = '\0';
}
