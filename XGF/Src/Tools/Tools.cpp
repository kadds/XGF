#include "../../Include/Tools.hpp"
#include <Windows.h>
#include <shlobj.h>
#include <fstream>  
#include <filesystem>
#include "../../Include/Logger.hpp"
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
			unsigned int len = WideCharToMultiByte((UINT)encode, 0, wch, (int)wcslen(wch), NULL, 0, NULL, NULL);
			char    *ch = new char[len + 1];
			WideCharToMultiByte((UINT)encode, 0, wch, (int)wcslen(wch), ch, len, NULL, NULL);
			ch[len] = '\0';
			str = ch;
			delete[]ch;
			return str;
		}
		std::pair<std::unique_ptr<char>, int> LoadFromFile(const std::string & file)
		{
			std::ifstream fs(file, std::ios::binary);
			if(!fs)
			{
				XGF_Warn(IO, "can't find file ", file);
				return std::pair<std::unique_ptr<char>, int>();
			}
			auto pbuf = fs.rdbuf();
			// FIXME: sizeof(long) 
			long size = static_cast<long>(pbuf->pubseekoff(0, std::ios::end, std::ios::in));
			fs.seekg(0, std::ios::beg);
			auto cdata = new char[size];
			auto data = std::make_pair(std::unique_ptr<char>(cdata), size);
			pbuf->pubseekpos(0, std::ios::in);
			pbuf->sgetn(cdata, size);
			fs.close();
			return data;
		}

		std::pair<std::unique_ptr<char>, int> LoadFromFile(const std::wstring & file)
		{
			std::ifstream fs(file, std::ios::binary);
			if (!fs)
			{
				XGF_Warn(IO, "can't find file ", ::XGF::Logger::WCharToChar(file.c_str()));
				return std::pair<std::unique_ptr<char>, int>();
			}
			auto pbuf = fs.rdbuf();
			// FIXME: sizeof(long) 
			long size = static_cast<long>(pbuf->pubseekoff(0, std::ios::end, std::ios::in));
			fs.seekg(0, std::ios::beg);
			auto cdata = new char[size];
			auto data = std::make_pair(std::unique_ptr<char>(cdata), size);
			pbuf->pubseekpos(0, std::ios::in);
			pbuf->sgetn(cdata, size);
			fs.close();
			return data;
		}

		const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;
#pragma pack(pop)

		void SetThreadName(DWORD dwThreadID, const char * threadName)
		{
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = threadName;
			info.dwThreadID = dwThreadID;
			info.dwFlags = 0;
			__try
			{
				RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		void SetCurrentThreadName(const char* name)
		{
			SetThreadName(-1, name);
		}
	}

}
