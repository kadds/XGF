#pragma once
#include "Defines.hpp"
#include <functional>
#include "Asyn.hpp"
namespace XGF
{

	typedef std::function<void(int code, std::any)> OnFinishTaskListener;
	typedef std::function<void(int process, std::any)> OnReportTaskProcessListener;
	/*
	异步任务类
	在TaskThread 结束前 务必调用一次Finish函数释放Task资源
	*/
	class AsyncTask : public std::enable_shared_from_this<AsyncTask>
	{
	public:
		static std::shared_ptr<AsyncTask> NewTask(Asyn * mainThread, std::function<void(std::shared_ptr< AsyncTask> )> fun);

		void Report(int process, EventDataType data);
		void Finish(int code, EventDataType data);

		AsyncTask & SetFinshListener(OnFinishTaskListener ls) { onFinishTaskListener = ls; return *this; }
		AsyncTask & SetReportListener(OnReportTaskProcessListener ls) { onReportTaskProcessListener = ls; return *this; }
		static void DoFinishTaskEvent(const Event & ev);
		static void DoReportTaskEvent(const Event & ev);
		OnFinishTaskListener GetFinishTaskListener() { return onFinishTaskListener; }
		OnReportTaskProcessListener GetReportTaskProcessListener() { return onReportTaskProcessListener; }
	public:
		AsyncTask();
		~AsyncTask();
	private:
		
		void Start(Asyn * main, std::function<void(std::shared_ptr<AsyncTask>)> fun);
		OnFinishTaskListener onFinishTaskListener;
		OnReportTaskProcessListener onReportTaskProcessListener;
		Asyn mThread;

		Asyn *mainThread;
		DISALLOW_COPY_AND_ASSIGN(AsyncTask);
	};



}
