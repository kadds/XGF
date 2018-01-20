#include "../../Include/AsyncTask.hpp"

namespace XGF
{
	AsyncTask::AsyncTask() :onFinishTaskListener(nullptr), onReportTaskProcessListener(nullptr)
	{

	}


	AsyncTask::~AsyncTask()
	{
	}

	void AsyncTask::Start(Asyn * main, std::function<void(AsyncTask * asyncTask)> fun)
	{
		mainThread = main;
		mThread.DoAsyn(std::bind(fun, this));

	}
	AsyncTask * AsyncTask::NewTask(Asyn * mainThread, std::function<void(AsyncTask * asyn)> fun)
	{
		AsyncTask * as = new AsyncTask();
		as->Start(mainThread, fun);
		return as;
	}
	void AsyncTask::Report(int process, std::any data)
	{
		mainThread->PostEvent(SystemEventId::AsynReport, { process, data, this });
	}

	void AsyncTask::Finish(int code, std::any data)
	{
		mainThread->PostEvent(SystemEventId::AsynFinish, { code, data, this });
	}

	void AsyncTask::DoFinishTaskEvent(const Event & ev)
	{
		AsyncTask * tk = ev.GetData<AsyncTask *>(2);
		auto fun = tk->GetFinishTaskListener();
		if (fun != nullptr)
			fun(ev.GetDataInt(0), ev.mData[1]);
		delete tk;
	}

	void AsyncTask::DoReportTaskEvent(const Event & ev)
	{
		AsyncTask * tk = ev.GetData<AsyncTask *>(2);
		auto fun = tk->GetReportTaskProcessListener();
		if (fun != nullptr)
			fun(ev.GetDataInt(0), ev.mData[1]);
	}

}

