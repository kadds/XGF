#include "../../Include/Logger.hpp"
#include <iostream>



namespace XGF {
	namespace Logger
	{
		char gFilename[MAX_PATH];

		LogRecorder gLogRecorder;
		bool gHasRecord = false;
		void ShowXGFDialog(char * str, char * name);
		const char * formatStr = "File:{0}:{2}; Function:{1}; Message:{3}";

		void Logger::GetHRString(char * out, int size, const char * msg, HRESULT hr)
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
			XGF_Info(Application, "end application");
			spdlog::drop_all();
			CloseConsole();
			if(mRemoveWhenClose)
			{
				DeleteFileA(gFilename);
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

		bool LogRecorder::IsOpenConsole() const
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


		LogRecorder::LogRecorder(): mRemoveWhenClose(true)
		{
			setlocale(LC_CTYPE, "");
			OpenConsole();
			spdlog::flush_every(std::chrono::seconds(3));
			char filepath[MAX_PATH];
			GetTempPathA(MAX_PATH, filepath);
			GetTempFileNameA(filepath, "Log", 0, gFilename);
			std::vector<spdlog::sink_ptr> sinks{ 
				std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>(),
				std::make_shared<spdlog::sinks::basic_file_sink_mt>(gFilename, false),
				std::make_shared<spdlog::sinks::msvc_sink_mt>(),
			};
			auto caster = fmt::formatter<LogSystem::LogSystem>();
			for (int i = 0; i <= LogSystem::None; i ++)
			{
				auto logger = std::make_shared<spdlog::logger>(
					fmt::to_string(caster.formatStr((LogSystem::LogSystem)i)),
					sinks.begin(), sinks.end()
				);
				spdlog::register_logger(logger);
				loggers.push_back(std::move(logger));
			}
			
			spdlog::set_error_handler([](const std::string& msg)
			{
				std::cerr << "an error occurred: " << msg << std::endl;
			});
#ifdef _DEBUG
			SetLevel(LogLevel::Debug);
#else
			SetLevel(LogLevel::Info);
#endif
			XGF_Info(Application, "start application");
			XGF_Debug(Application, fmt::format("log file path:{0}", gFilename));
			char path[256];
			GetModuleFileNameA(NULL, path, 256);
			XGF_Info(Application, "application path:", path);

		}
		void ShowXGFDialog(const char* str, const char * name)
		{
			if (MessageBoxA(nullptr, name, str, MB_YESNO | MB_ICONERROR) == IDYES)
			{
				ShellExecuteA(0,"open","notepad.exe",name, 0, SW_SHOW);
			}
		}
	}
}