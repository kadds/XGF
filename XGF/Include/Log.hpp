#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <fstream>
#include <deque>
#include <time.h>
#include <sstream>
#include <initializer_list>
#include "spdlog.h"
/*
»’º«class
*/
namespace XGF {
	namespace Log {
		std::string WcharToChar(const wchar_t* wch, size_t encode = CP_ACP);
		enum class LogLevel
		{
			Debug,
			Info,
			Warn,
			Error,
		};

		void constexpr CaseData(std::ostream & s) {};

		template<typename Head, typename... Rest>
		inline constexpr void CaseData(std::ostream & s, Head head, Rest...rest)
		{
			if constexpr(std::is_same<typename std::decay<Head>::type, wchar_t const *>::value || std::is_same<typename std::decay<Head>::type, wchar_t *>::value)
			{
				s << WcharToChar(head);
			}
			else if constexpr(std::is_same<typename std::decay<Head>::type, wchar_t>::value)
			{
				wchar_t wch[2] = { head, 0 };
				s << WcharToChar(wch);
			}
			else
				s << head;
			//pushToss(s, head);
			CaseData(s, rest...);
		};
		/*
		template<typename T>
		inline void pushToss(std::ostream & s, T t)
		{
			s << t;
		}
		inline void pushToss(std::ostream & s, const wchar_t * t)
		{
			s << WcharToChar(t);
		}
		inline void pushToss(std::ostream & s, const wchar_t t)
		{
			wchar_t wch[2] = { t, L'\0' };
			s << WcharToChar(wch);
		}
		*/
		class LogData 
		{
		public:
			const char * file;
			const char * funname;
			::std::stringstream str;
			::std::stringstream str2;
			int line;

			LogData(const char * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname);
			
			LogData(const char * errorstr, int cline, char * cfile, char * cfunname);
			LogData(const wchar_t * errorstr, int cline, char * cfile, char * cfunname);

			LogData(const char * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname);
			LogData(const wchar_t * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname);
			LogData(const wchar_t * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname);
			~LogData();
			
		};
		
		void Record(LogLevel level, LogData & logData);
		void CheckHR(HRESULT hr, LogLevel level, LogData & logData);
		
		
		void GetHRString(char * out, int size, const char * msg, HRESULT hr);
		

		extern char gFilename[MAX_PATH];
		class LogRecorder 
		{
		private:
			bool openConsole;
			std::shared_ptr<spdlog::logger> stdLogger;
		public:
			LogRecorder();
			~LogRecorder();
			void Log(std::string str, LogLevel lv);
			void OpenConsole();
			void CloseConsole();
			void ShowConsole();
			void HideConsole();
			bool IsShowConsole();
			void SetLevel(LogLevel lv);
		};
		extern LogRecorder gLogRecorder;
		extern bool gHasRecord;
	};
	
};

#ifdef _DEBUG
#define XGF_USE_FUNCTION_NAME
#endif

#ifdef XGF_USE_FUNCTION_NAME
#define _FUNNAME_XGF_ __FUNCDNAME__
#else
#define _FUNNAME_XGF_ nullptr
#endif
#define XGF_Error_Check(hr, ErrorStr, ...) { HRESULT hrt = hr;if(!SUCCEEDED(hrt)) {::XGF::Log::LogData dt(ErrorStr, __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__);::XGF::Log::CheckHR(hrt, ::XGF::Log::LogLevel::Error, dt); }}
#define XGF_Warn_Check(hr, WarnStr, ...) { HRESULT hrt = hr;if(!SUCCEEDED(hrt)) {::XGF::Log::LogData dt(WarnStr, __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__);::XGF::Log::CheckHR(hrt, ::XGF::Log::LogLevel::Warn, dt); }}

#define XGF_Info_Check(hr, InfoStr, ...) { HRESULT hrt = hr;if(!SUCCEEDED(hrt)) {::XGF::Log::LogData dt(InfoStr, __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__);::XGF::Log::CheckHR(hrt, ::XGF::Log::LogLevel::Info, dt); }}

#define XGF_ReportError_Ex(ErrorStr, ...) {XGF::Log::LogData dt(ErrorStr, __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__); ::XGF::Log::Record(::XGF::Log::LogLevel::Error, dt);}

#define XGF_ReportWarn_Ex(WarnStr, ...) {::XGF::Log::LogData dt(WarnStr, __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__); ::XGF::Log::Record(::XGF::Log::LogLevel::Warn, dt);}

#define XGF_ReportInfo_Ex(InfoStr, ...) {::XGF::Log::LogData dt(InfoStr, __LINE__, __FILE__, _FUNNAME_XGF_);  ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__); ::XGF::Log::Record(::XGF::Log::LogLevel::Info, dt);}

#define XGF_ReportDebug_Ex(DebugStr, ...) {::XGF::Log::LogData dt((DebugStr), __LINE__, __FILE__, _FUNNAME_XGF_); ::XGF::Log::CaseData(dt.str2, ##__VA_ARGS__);::XGF::Log::Record(::XGF::Log::LogLevel::Debug, dt);}

#define XGF_ReportError0(ErrorStr) ::XGF::Log::Record(::XGF::Log::LogLevel::Error, ::XGF::Log::LogData(ErrorStr, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportError(ErrorStr, ErrorStr2) ::XGF::Log::Record(::XGF::Log::LogLevel::Error, ::XGF::Log::LogData(ErrorStr, ErrorStr2, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportWarn0(WarnStr) ::XGF::Log::Record(::XGF::Log::LogLevel::Warn, ::XGF::Log::LogData(WarnStr, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportWarn(WarnStr, WarnStr2) ::XGF::Log::Record(::XGF::Log::LogLevel::Warn, ::XGF::Log::LogData(WarnStr, WarnStr2, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportInfo0(InfoStr) ::XGF::Log::Record(::XGF::Log::LogLevel::Info, ::XGF::Log::LogData(InfoStr, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportInfo(InfoStr, InfoStr2) ::XGF::Log::Record(::XGF::Log::LogLevel::Info, ::XGF::Log::LogData(InfoStr, InfoStr2, __LINE__, __FILE__, _FUNNAME_XGF_))

#ifdef _DEBUG
#define XGF_ReportDebug(DebugStr, DebugStr2) ::XGF::Log::Record(::XGF::Log::LogLevel::Debug, ::XGF::Log::LogData(DebugStr, DebugStr2, __LINE__, __FILE__, _FUNNAME_XGF_))
#define XGF_ReportDebug0(DebugStr) ::XGF::Log::Record(::XGF::Log::LogLevel::Debug, ::XGF::Log::LogData(DebugStr, __LINE__, __FILE__, _FUNNAME_XGF_))
#else
#define XGF_ReportDebug0(DebugStr) ((void)0)
#define XGF_ReportDebug(DebugStr, DebugStr2) ((void)0)
#endif

#ifdef _DEBUG
#define XGF_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#else
#define XGF_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (assert(_CRT_WIDE(#expression)), 0) \
        )
#endif