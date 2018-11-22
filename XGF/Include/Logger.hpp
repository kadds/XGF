#pragma once
#include "Defines.hpp"
#include "Tools.hpp"
#include <d3d11_1.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "dxerr.h"
#pragma warning(push)
#pragma warning(disable:4530)
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)
#include <string>
#include <codecvt>
/*
»’º«class
*/
namespace XGF {
	namespace Logger
	{
		enum class LogLevel
		{
			Debug,
			Info,
			Warn,
			Error,
		};
		namespace LogSystem
		{
			enum LogSystem
			{
				Application = 0,
				Framework,
				Audio,
				Render,
				Input,
				NetWork,
				IO,
				Shader,
				Unknown,
				// insert here
				None,
			};
		}
		

		void GetHRString(char * out, int size, const char * msg, HRESULT hr);
		void ShowXGFDialog(const char* str, const char * name);
		extern char gFilename[MAX_PATH];
		
		extern bool gHasRecord;

		inline std::string WCharToChar(const wchar_t * wt)
		{
			return fmt::internal::utf16_to_utf8(wt).str();
			//return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wt);
		}
		template<typename...Args>
		constexpr std::string GenerateFormatStr(const Args&... args)
		{
			static_assert(sizeof...(Args) <= 32, "The number of parameters must be less than 32");
			static const char * str = "{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}";
			return str + 64 - sizeof...(Args) * 2;
		}
		template<typename...Args>
		std::string JoinParameter(const Args&... args)
		{
			return fmt::format(GenerateFormatStr(args...), args...);
		}
		extern  const char * formatStr;
		class LogRecorder
		{
		private:
			bool openConsole;
			std::vector<std::shared_ptr<spdlog::logger>> loggers;
		public:
			LogRecorder();
			~LogRecorder();
			template<typename ...Args>
			void Debug(LogSystem::LogSystem system, const char *  file, const char *  functionName, int line, const Args &... args)
			{
				loggers[(int) system]->debug(formatStr, file, functionName, line, JoinParameter(args...));
			};
			template<typename ...Args>
			void Info(LogSystem::LogSystem system, const char * file, const char * functionName, int line, Args &... args)
			{
				loggers[(int) system]->info(formatStr, file, functionName, line, JoinParameter(args...));
			};
			template<typename ...Args>
			void Warn(LogSystem::LogSystem system, const char * file, const char *  functionName, int line, Args &... args)
			{
				loggers[(int) system]->warn(formatStr, file, functionName, line, JoinParameter(args...));
			};
			template<typename ...Args>
			void Error(LogSystem::LogSystem system, const char * file, const char *  functionName, int line, Args &... args)
			{
				loggers[(int) system]->error(formatStr, file, functionName, line, JoinParameter(args...));
				ShowXGFDialog("A serious error has occurred, do you need to check it?", gFilename);
				exit(-1);
			};

			void OpenConsole();
			void CloseConsole();
			void ShowConsole();
			void HideConsole();
			bool IsOpenConsole() const;
			void SetLevel(LogLevel lv);
		};
		extern LogRecorder gLogRecorder;
	};
	
};


namespace fmt
{
	template <>
	struct formatter<XGF::Logger::LogSystem::LogSystem> : fmt::formatter<fmt::string_view>
	{
		auto formatStr(XGF::Logger::LogSystem::LogSystem  system)
		{
			fmt::string_view name = "Unknown";
			switch (system)
			{
			case XGF::Logger::LogSystem::Application:   name = "Application"; break;
			case XGF::Logger::LogSystem::Framework: name = "Framework"; break;
			case XGF::Logger::LogSystem::Audio:  name = "Audio"; break;
			case XGF::Logger::LogSystem::Render:  name = "Render"; break;
			case XGF::Logger::LogSystem::Input:  name = "Input"; break;
			case XGF::Logger::LogSystem::NetWork:  name = "NetWork"; break;
			case XGF::Logger::LogSystem::IO:  name = "IO"; break;
			case XGF::Logger::LogSystem::Shader: name = "Shader"; break;
			case XGF::Logger::LogSystem::Unknown:  name = "Unknown"; break;
			case XGF::Logger::LogSystem::None:  name = "None"; break;
			}
			return name;
		};
		// parse is inherited from formatter<string_view>.
		template <typename FormatContext>
		auto format(XGF::Logger::LogSystem::LogSystem system, FormatContext &ctx)
		{
			fmt::string_view name = "Unknown";
			return formatter<fmt::string_view>::format(formatStr(system), ctx);
		}

	};

	template<>
	struct formatter< std::string, wchar_t >
	{

		template< typename ParseContextT >
		constexpr auto parse(ParseContextT& ctx)
		{
			return ctx.begin();
		}

		template< typename FormatContextT >
		auto format(const std::string& str, FormatContextT& ctx)
		{
			const internal::utf8_to_utf16 wstr(str);
			return format_to(ctx.begin(), L"{}", wstr.c_str());
		}
	};

	template<>
	struct formatter< std::string_view, wchar_t >
	{

		template< typename ParseContextT >
		constexpr auto parse(ParseContextT& ctx)
		{
			return ctx.begin();
		}

		template< typename FormatContextT >
		auto format(const std::string_view& str, FormatContextT& ctx)
		{
			const internal::utf8_to_utf16 wstr(str);
			return format_to(ctx.begin(), L"{}", wstr.c_str());
		}
	};

	template<>
	struct formatter< std::wstring, char >
	{

		template< typename ParseContextT >
		constexpr auto parse(ParseContextT& ctx)
		{
			return ctx.begin();
		}

		template< typename FormatContextT >
		auto format(const std::wstring& wstr, FormatContextT& ctx)
		{
			const internal::utf16_to_utf8 str(wstr);
			return format_to(ctx.begin(), "{}", str.c_str());
		}
	};

	template<>
	struct formatter< std::wstring_view, char >
	{

		template< typename ParseContextT >
		constexpr auto parse(ParseContextT& ctx)
		{
			return ctx.begin();
		}

		template< typename FormatContextT >
		auto format(const std::wstring_view& wstr, FormatContextT& ctx)
		{
			const internal::utf16_to_utf8 str(wstr);
			return format_to(ctx.begin(), "{}", str.c_str());
		}
	};
}
//https://github.com/fmtlib/fmt/issues/572

#ifdef _DEBUG
#define XGF_USE_FUNCTION_NAME
#define XGF_USE_FILE_NAME
#define XGF_USE_FILE_LINE
#endif

#ifdef XGF_USE_FUNCTION_NAME
#define XGF_FUNCTIONNAME __FUNCTION__
#else
#define XGF_FUNCTIONNAME  "-"
#endif

#ifdef XGF_USE_FILE_NAME
#define XGF_FILENAME __FILE__
#else
#define XGF_FILENAME "-"
#endif

#ifdef XGF_USE_FILE_LINE
#define XGF_FILELINE __LINE__
#else
#define XGF_FILELINE -1
#endif

#define XGF_Log(system, type, ...) ::XGF::Logger::gLogRecorder.##type(::XGF::Logger::LogSystem::##system, XGF_FILENAME, XGF_FUNCTIONNAME, XGF_FILELINE, ##__VA_ARGS__)

#define XGF_Debug(system, ...) XGF_Log(system, Debug, ##__VA_ARGS__)
#define XGF_Info(system, ...) XGF_Log(system, Info, ##__VA_ARGS__)
#define XGF_Warn(system, ...) XGF_Log(system, Warn, ##__VA_ARGS__)
#define XGF_Error(system, ...) XGF_Log(system, Error, ##__VA_ARGS__)

#define XGF_Log_Check(system, type, hr, ...) { \
	HRESULT hrt = hr;\
	if(!SUCCEEDED(hrt)) {\
		XGF_Log(system, type, ::XGF::Logger::WCharToChar(DXGetErrorStringW(hr)), ##__VA_ARGS__);\
		}\
}

#define XGF_Error_Check(system, hr, ...) XGF_Log_Check(system, Error, hr, ##__VA_ARGS__)
#define XGF_Warn_Check(system, hr, ...) XGF_Log_Check(system, Warn, hr, ##__VA_ARGS__)
#define XGF_Info_Check(system, hr, ...) XGF_Log_Check(system, Info, hr, ##__VA_ARGS__)
#define XGF_Debug_Check( system, hr, ...) XGF_Log_Check(system, Debug, hr, ##__VA_ARGS__)


#define TAG_TYPE_DEBUG ::XGF::Logger::LogLevel::Debug
#define TAG_TYPE_INFO ::XGF::Logger::LogLevel::Info
#define TAG_TYPE_WARN ::XGF::Logger::LogLevel::Warn
#define TAG_TYPE_ERROR ::XGF::Logger::LogLevel::Error
