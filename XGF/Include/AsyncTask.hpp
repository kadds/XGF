#pragma once
#include "Defines.hpp"
#include <thread>
#include <future>
#include <functional>
#include "Asyn.hpp"
namespace XGF
{

	typedef std::function<void(int code, void * tsk)> OnFinishTaskListener;
	typedef std::function<void(int process, void * tsk)> OnReportTaskProcessListener;
	/*
	异步任务类
	在TaskThread 结束前 务必调用一次Finish函数释放Task资源
	*/
	class AsyncTask
	{
	public:
		static AsyncTask * NewTask(Asyn * mainThread, std::function<void(AsyncTask * asyncTask)> fun);

		void Report(int process, void *data);
		void Finish(int code, void * data);

		AsyncTask & SetFinshListener(OnFinishTaskListener ls) { onFinishTaskListener = ls; return *this; }
		AsyncTask & SetReportListener(OnReportTaskProcessListener ls) { onReportTaskProcessListener = ls; return *this; }
		static void DoFinishTaskEvent(const Event & ev);
		static void DoReportTaskEvent(const Event & ev);
		OnFinishTaskListener GetFinishTaskListener() { return onFinishTaskListener; }
		OnReportTaskProcessListener GetReportTaskProcessListene() { return onReportTaskProcessListener; }
	private:
		AsyncTask();
		~AsyncTask();
		void Start(Asyn * mainThread, std::function<void(AsyncTask * asyncTask)> fun);
		OnFinishTaskListener onFinishTaskListener;
		OnReportTaskProcessListener onReportTaskProcessListener;
		Asyn mThread;

		Asyn *mainThread;
	};



}
