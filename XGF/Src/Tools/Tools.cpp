#include "../../Include/Tools.hpp"
#include <Windows.h>
#include <shlobj.h>
#include <filesystem>
namespace fs = std::experimental::filesystem;

namespace XGF
{
	namespace Tools
	{

		wchar_t * GetPathBy(const wchar_t * filename, wchar_t * buffer, int maxlen)
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

		char * GetPathBy(const char * filename, char * buffer, int maxlen)
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


		string GetFontPath(const string & name)
		{
			wchar_t buffer[MAX_PATH];
			SHGetSpecialFolderPathW(NULL, buffer, CSIDL_FONTS, FALSE);
			string path(buffer);
			path += L"\\";
			path += name;
			if (fs::exists(path + L".ttf"))
			{
				path += L".ttf";
				return path;
			}
			else if (fs::exists(path + L".ttc"))
			{
				path += L".ttc";
				return path;
			}
			else
			{
				return string();
			}
		}

		void GetAppPath(wchar_t * t, int len)
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
		std::string WcharToChar(const wchar_t* wch, size_t encode)
		{
			std::string str;
			int len = WideCharToMultiByte(encode, 0, wch, wcslen(wch), NULL, 0, NULL, NULL);
			char    *ch = new char[len + 1];
			WideCharToMultiByte(encode, 0, wch, wcslen(wch), ch, len, NULL, NULL);
			ch[len] = '\0';
			str = ch;
			delete ch;
			return str;
		}
	}

}
