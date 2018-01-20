#pragma once
#include "Defines.hpp"
#include <thread>
#include <future>
#include <functional>
#include <any>
#include "Asyn.hpp"
namespace XGF
{

	typedef std::function<void(int code, std::any)> OnFinishTaskListener;
	typedef std::function<void(int process, std::any)> OnReportTaskProcessListener;
	/*
	异步任务类
	在TaskThread 结束前 务必调用一次Finish函数释放Task资源
	*/
	class AsyncTask
	{
	public:
		static AsyncTask * NewTask(Asyn * mainThread, std::function<void(AsyncTask * asyncTask)> fun);

		void Report(int process, std::any data);
		void Finish(int code, std::any data);

		AsyncTask & SetFinshListener(OnFinishTaskListener ls) { onFinishTaskListener = ls; return *this; }
		AsyncTask & SetReportListener(OnReportTaskProcessListener ls) { onReportTaskProcessListener = ls; return *this; }
		static void DoFinishTaskEvent(const Event & ev);
		static void DoReportTaskEvent(const Event & ev);
		OnFinishTaskListener GetFinishTaskListener() { return onFinishTaskListener; }
		OnReportTaskProcessListener GetReportTaskProcessListener() { return onReportTaskProcessListener; }
	private:
		AsyncTask();
		~AsyncTask();
		void Start(Asyn * mainThread, std::function<void(AsyncTask * asyncTask)> fun);
		OnFinishTaskListener onFinishTaskListener;
		OnReportTaskProcessListener onReportTaskProcessListener;
		Asyn mThread;

		Asyn *mainThread;
		DISALLOW_COPY_AND_ASSIGN(AsyncTask);
	};



}
