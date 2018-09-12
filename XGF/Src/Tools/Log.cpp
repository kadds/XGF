#include "../../Include/Log.hpp"
#include "../../Include/dxerr.h"
#include <sstream> 
#include <time.h>
#include <iostream>
#pragma warning(push)
#pragma warning(disable:4530)
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)


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
			std::stringstream ss;

			if (logData.funname != nullptr)
			{
				if (level == LogLevel::Debug)
					ss << logData.str.str() << " " << (!logData.str2.str()._Equal("") ? logData.str2.str() : "");
				else
				ss <<  logData.file << " line:" << 
					logData.line << " function:" << 
					logData.funname
					<< " " << logData.str.str() << " " << ( !logData.str2.str()._Equal("") ? logData.str2.str() : "");
			}
			else
				if (level == LogLevel::Debug)
					ss << logData.str.str() << " " << (!logData.str2.str()._Equal("") ? logData.str2.str() : "");
				else
					ss << logData.file << " line:" << logData.line
					<< " " << logData.str.str() << " " << logData.str2.str();

			gLogRecorder.Log(ss.str(), level);
			if (level == LogLevel::Error) 
			{
				std::string str = "An error occurred in the application \n";
				str += ss.str();
				MessageBoxA(NULL, str.c_str(), "Error", 0);
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

		void Log::GetHRString(char * out, int size, const char * msg, HRESULT hr)
		{
			const int len = 4096;
			char *wdxerr = new char[len];
			size_t converted = 0;
			const wchar_t * estring = DXGetErrorStringW(hr);
			wcstombs_s(&converted, wdxerr, len / sizeof(char), estring, _TRUNCATE);
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
			stdLogger->info("close application");
			spdlog::drop_all();
			CloseConsole();	
		}

		void LogRecorder::Log(std::string str, LogLevel level)
		{
			switch (level)
			{
			case LogLevel::Debug:
				stdLogger->debug(str);
				break;
			case LogLevel::Info:
				stdLogger->info(str);
				break;
			case LogLevel::Warn:
				stdLogger->warn(str);
				break;
			case LogLevel::Error:
				stdLogger->error(str);
				break;
			}
		}

		void LogRecorder::OpenConsole()
		{
			AllocConsole();
			SetConsoleTitle(L"XGF Debug");
			FILE * stdd;
			freopen_s(&stdd, "CONOUT$", "w+t", stdout);
			HWND hwnd = GetConsoleWindow();
			HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);
			DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);

			SetConsoleCtrlHandler(NULL, TRUE);
			openConsole = true;
		}

		void LogRecorder::CloseConsole()
		{
			FreeConsole();
			openConsole = false;
		}

		void LogRecorder::ShowConsole()
		{
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_SHOW);
		}

		void LogRecorder::HideConsole()
		{
			HWND hwnd = GetConsoleWindow();
			ShowWindow(hwnd, SW_HIDE);
		}

		bool LogRecorder::IsShowConsole()
		{
			return openConsole;
		}

		void LogRecorder::SetLevel(LogLevel level)
		{
			switch (level)
			{
			case LogLevel::Debug:
				spdlog::set_level(spdlog::level::debug);
				break;
			case LogLevel::Info:
				spdlog::set_level(spdlog::level::info);
				break;
			case LogLevel::Warn:
				spdlog::set_level(spdlog::level::warn);
				break;
			case LogLevel::Error:
				spdlog::set_level(spdlog::level::err);
				break;
			}
			
		}


		LogRecorder::LogRecorder()
		{
			setlocale(LC_CTYPE, "");
			OpenConsole();
			spdlog::flush_every(std::chrono::seconds(3));
			spdlog::set_level(spdlog::level::info);
			char filepath[MAX_PATH];
			GetTempPathA(MAX_PATH, filepath);
			GetTempFileNameA(filepath, "Log", 0, gFilename);
			std::vector<spdlog::sink_ptr> sinks{ 
				std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>(),
				std::make_shared<spdlog::sinks::basic_file_sink_mt>(gFilename, false)
			};
			stdLogger = std::make_shared<spdlog::logger>("std", sinks.begin(), sinks.end());

			spdlog::set_error_handler([](const std::string& msg)
			{
			std::cerr << "an error occurred: " << msg << std::endl;
			});

			std::string path = "log file save as ";
			path += gFilename;
			stdLogger->info("start application");
			stdLogger->info(path);
#ifdef _DEBUG
			spdlog::set_level(spdlog::level::debug);
#else
			spdlog::set_level(spdlog::level::info); // info
#endif
			
		}
		void ShowXGFDialog(char* str, char * name)
		{
			if (MessageBoxA(nullptr, name, str, MB_YESNO | MB_ICONERROR) == IDYES)
			{
				ShellExecuteA(0,"open","notepad.exe",name, 0, SW_SHOW);
			}
		}
		LogData::LogData(const char * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str(errorstr)
			, str2(errorstr2) {}
		LogData::LogData(const char * errorstr, int cline, char * cfile, char * cfunname) : file(cfile), funname(cfunname), line(cline), str(errorstr), str2("") {}
		LogData::LogData(const wchar_t * errorstr, int cline, char * cfile, char * cfunname) : file(cfile), funname(cfunname), line(cline), str2("")
		{
			char * temp = new char[8192];
			wcstombs_s(nullptr, temp, 8192, errorstr, 8192);
			str.str(temp);
			delete[] temp;
		}
		LogData::LogData(const char * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str(errorstr)
		{
			char * temp = new char[8192];
			wcstombs_s(nullptr, temp, 8192, errorstr2, 8192);
			str2.str(temp);
			delete[] temp;
		}
		LogData::LogData(const wchar_t * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str2(errorstr2)
		{

			char * temp = new char[8192];
			wcstombs_s(nullptr, temp, 8192, errorstr, _TRUNCATE);
			str.str(temp);
			delete[] temp;
		}
		LogData::LogData(const wchar_t * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline)
		{
			char * temp = new char[8192];
			char * temp2 = new char[8192];
			wcstombs_s(nullptr, temp, 8192, errorstr, 8192);
			wcstombs_s(nullptr, temp, 8192, errorstr2, 8192);
			str.str(temp);
			str2.str(temp2);
			delete[] temp;
			delete[] temp2;
		}
		LogData::~LogData()
		{
		}
}
}