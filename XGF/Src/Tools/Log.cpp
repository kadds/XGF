#include "../../Include/Log.hpp"
#include "../../Include/dxerr.h"
#include <sstream> 
#include <locale>
#include <iomanip>
#include<ctime>
#include <cstdio>
#include <time.h>
#include <shlobj.h>
#include<utility>
namespace XGF {
	namespace Log {
		std::stringstream mDebugBuffer;
		std::stringstream mInfoBuffer;
		std::stringstream mErrorBuffer;
		std::stringstream mWarnBuffer;
		const size_t gMaxBufferSize = 20480;
		char gFilename[MAX_PATH];
		LogRecorder gLogRecorder;
		bool gHasRecord = false;
		void ShowXGFDialog(char * str, char * name);
		void Record(LogLevel level, LogData & logData)
		{
			std::time_t t = std::time(NULL);
			std::tm tm;
			localtime_s(&tm, &t);
			std::stringstream * ss = nullptr;
			char * str = nullptr;
			switch (level)
			{
			case LogLevel::Debug:
				ss = &mDebugBuffer;
				str = "Debug";
				break;
			case LogLevel::Info:
				ss = &mInfoBuffer;
				str = "Info";
				break;
			case LogLevel::Warn:
				ss = &mWarnBuffer;
				str = "Warn";
				gHasRecord = true;
				break;
			case LogLevel::Error:
				ss = &mErrorBuffer;
				str = "Error";
				break;
			default:
				return;
			}
			if (logData.funname != nullptr)
			{
				*ss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "](" << str << ")-->" <<
					logData.file << "  line:" << logData.line << "  function:" << logData.funname
					<< " " << logData.str.str() << "    " << logData.str2.str() << std::endl;
			}
			else
				*ss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "](" << str << ")-->" <<
				logData.file << "  line:" << logData.line
				<< " " << logData.str.str() << "    " << logData.str2.str() << std::endl;
			if (ss->str().size() > gMaxBufferSize)
			{
				gLogRecorder.Open() << ss->str();
				gLogRecorder.Close();
				ss->str("");
			}
			
			if (level == LogLevel::Error)
			{
				//flush all
				gLogRecorder.Open() << mDebugBuffer.str() << std::endl << mInfoBuffer.str() << std::endl << mWarnBuffer.str() << std::endl << mErrorBuffer.str();
				gLogRecorder.Close();
				ShowXGFDialog("A error hanpend, Do you want to open log file?", gFilename);
				gHasRecord = false;
				exit(-1);
			}
		}
		void CheckHR(HRESULT hr, LogLevel level, LogData & logData)
		{
			if (!SUCCEEDED(hr))
			{
				char *msg = new char[8192];
				GetHRString(msg, 8192, nullptr, hr);
				logData.str2 << msg;
				Record(level, logData);
				delete[] msg;
			}
		}
		void OutputDebugStringEx(const wchar_t *strOutputString, ...)
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
		void OutputDebugStringEx(const char *strOutputString, ...)
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

		void Log::GetHRString(char * out, int size, const char * msg, HRESULT hr)
		{
			char *wdxerr = new char[2048];
			size_t converted = 0;
			const wchar_t * estring = DXGetErrorStringW(hr);
			wcstombs_s(&converted, wdxerr, wcslen(estring) + 1, estring, _TRUNCATE);
			if (msg != nullptr)
			{
				sprintf_s(out, size, "DXerror: %s\n%s", wdxerr, msg);
			}
			else
			{
				sprintf_s(out, size, "DXerror: %s", wdxerr);
			}
			delete[]wdxerr;
		}
		LogRecorder::~LogRecorder()
		{
			if (mLogfile.is_open())
			{
				mLogfile.flush();
				mLogfile.close();
			}
#ifdef _DEBUG
			if (gHasRecord)
			{
				ShowXGFDialog("Some Log had recorded, Do you want to open log file?", gFilename);
			}
#endif
		}

		std::ofstream & LogRecorder::Open()
		{
			mLogfile.open(gFilename, std::ios::out | std::ios::app);
			std::time_t t = std::time(NULL);
			std::tm timeinfo;
			localtime_s(&timeinfo, &t);
			mLogfile.imbue(std::locale());
			return mLogfile;
		}

		void LogRecorder::Close()
		{
			mLogfile.flush();
			mLogfile.close();
		}

		LogRecorder::LogRecorder()
		{
			char filepath[MAX_PATH];
			GetTempPathA(MAX_PATH,
				filepath);
			GetTempFileNameA(filepath, "Log", 0, gFilename);
		}
		void ShowXGFDialog(char* str, char * name)
		{
			if (MessageBoxA(nullptr, name, str, MB_YESNO | MB_ICONERROR) == IDYES)
			{
				ShellExecuteA(0,"open","notepad.exe",name, 0, SW_SHOW);
			}
		}
}
}