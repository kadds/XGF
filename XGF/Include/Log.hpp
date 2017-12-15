#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <fstream>
#include <deque>
#include <time.h>
#include <sstream>
#include <initializer_list>
/*
»’º«class
*/
namespace XGF {
	namespace Log {
		enum class LogLevel
		{
			Debug,
			Info,
			Warn,
			Error,
		};
		template<typename... args>
		void CaseData(std::ostream & s, args...ag)
		{
			int k[] = { (s << ag, 0)... ,0 };
		};
		class LogData 
		{
		public:
			const char * file;
			const char * funname;
			LogData(const char * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str(errorstr)
				,str2(errorstr2){};
			::std::stringstream str;
			::std::stringstream str2;
			int line;
			LogData(const char * errorstr,  int cline, char * cfile, char * cfunname) :file(cfile), funname(cfunname), line(cline), str(errorstr), str2(""){};
			LogData(const wchar_t * errorstr, int cline, char * cfile, char * cfunname) :file(cfile), funname(cfunname), line(cline), str2("") 
			{
				char * temp = new char[8192];
				wcstombs_s(nullptr,temp, 8192, errorstr, 8192);
				str.str(temp);
				delete[] temp;
			};

			LogData(const char * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str(errorstr)
			{
				char * temp = new char[8192];
				wcstombs_s(nullptr, temp, 8192, errorstr2, 8192);
				str2.str(temp);
				delete[] temp;
			};
			LogData(const wchar_t * errorstr, const char * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline), str2(errorstr2)
			{
				
				char * temp = new char[8192];
				wcstombs_s(nullptr, temp, 8192, errorstr, 8192);
				str.str(temp);
				delete[] temp;
			};
			LogData(const wchar_t * errorstr, const wchar_t * errorstr2, int cline, const char * cfile, const char * cfunname) :file(cfile), funname(cfunname), line(cline)
			{
				char * temp = new char[8192];
				char * temp2 = new char[8192];
				wcstombs_s(nullptr, temp, 8192, errorstr, 8192);
				wcstombs_s(nullptr, temp, 8192, errorstr2, 8192);
				str.str(temp);
				str2.str(temp2);
				delete[] temp;
				delete[] temp2;
			};
			~LogData()
			{
			}
			
		};
		
		void Record(LogLevel level, LogData & logData);
		void CheckHR(HRESULT hr, LogLevel level, LogData & logData);
		
		
		void GetHRString(char * out, int size, const char * msg, HRESULT hr);
		
		extern std::stringstream mDebugBuffer;
		extern std::stringstream mInfoBuffer;
		extern std::stringstream mErrorBuffer;
		extern std::stringstream mWarnBuffer;
		extern const size_t gMaxBufferSize;
		extern char gFilename[MAX_PATH];
		class LogRecorder 
		{
		private:
			std::ofstream mLogfile;
		public:
			LogRecorder();
			~LogRecorder();
			std::ofstream & Open();
			void Close();
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
#define XGF_ReportDebug0(DebugStr) ;
#define XGF_ReportDebug(DebugStr, DebugStr2) ;
#endif

