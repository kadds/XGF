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
	void AsyncTask::Report(int process, void * data)
	{
		mainThread->PostEvent(EVENT_ONASYNCTASKREPORT, process, data, this);
	}

	void AsyncTask::Finish(int code, void * data)
	{
		mainThread->PostEvent(EVENT_ONASYNCTASKFINISH, code, data, this);
	}

	void AsyncTask::DoFinishTaskEvent(const Event & ev)
	{
		AsyncTask * tk = (AsyncTask *)(ev.Content.z.address);
		auto fun = tk->GetFinishTaskListener();
		if (fun != nullptr)
			fun(ev.Content.x.num, ev.Content.y.address);
		delete tk;
	}

	void AsyncTask::DoReportTaskEvent(const Event & ev)
	{
		AsyncTask * tk = (AsyncTask *)(ev.Content.z.address);
		auto fun = tk->GetReportTaskProcessListene();
		if (fun != nullptr)
			fun(ev.Content.x.num, ev.Content.y.address);
	}

}

