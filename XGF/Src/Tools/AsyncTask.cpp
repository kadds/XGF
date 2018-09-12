#include "../../Include/AsyncTask.hpp"

namespace XGF
{
	AsyncTask::AsyncTask() :onFinishTaskListener(nullptr), onReportTaskProcessListener(nullptr)
	{

	}

	AsyncTask::~AsyncTask()
	{
	}

	void AsyncTask::Start(Asyn * main, std::function<void(std::shared_ptr<AsyncTask>)> fun)
	{
		mainThread = main;
		mThread.DoAsyn(std::bind(fun, shared_from_this()));

	}
	std::shared_ptr<AsyncTask> AsyncTask::NewTask(Asyn * mainThread, std::function<void(std::shared_ptr<AsyncTask>)> fun)
	{
		auto as = std::make_shared<AsyncTask>();
		as->Start(mainThread, fun);
		return as;
	}
	void AsyncTask::Report(int process, EventDataType data)
	{
		mainThread->PostEvent(SystemEventId::AsynReport, { process, data, shared_from_this() });
	}

	void AsyncTask::Finish(int code, EventDataType data)
	{
		mainThread->PostEvent(SystemEventId::AsynFinish, { code, data, shared_from_this() });
	}

	void AsyncTask::DoFinishTaskEvent(const Event & ev)
	{
		auto tk = ev.GetDataSmartPointer<AsyncTask>(2);
		auto fun = tk->GetFinishTaskListener();
		if (fun != nullptr)
			fun(ev.GetDataInt(0), ev.mData[1]);
	}

	void AsyncTask::DoReportTaskEvent(const Event & ev)
	{
		auto tk = ev.GetDataSmartPointer<AsyncTask>(2);
		auto fun = tk->GetReportTaskProcessListener();
		if (fun != nullptr)
			fun(ev.GetDataInt(0), ev.mData[1]);
	}

}

