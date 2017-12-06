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
	�첽������
	��TaskThread ����ǰ ��ص���һ��Finish�����ͷ�Task��Դ
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
