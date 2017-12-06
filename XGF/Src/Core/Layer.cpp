#include "../../Include/Layer.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
namespace XGF
{
	Layer::Layer()
	{
	}


	Layer::~Layer()
	{
	}

	void Layer::SetParent(Scene * parent)
	{
		mParent = parent;
	}

	void Layer::OnCreate()
	{
		mBatches = new Batches();
		mBatches->Initialize(mParent->GetFramework()->GetGDI());
	}

	void Layer::OnDestory()
	{
		mBatches->Shutdown(); delete mBatches;
	}

	void Layer::Add(const Actor * actor)
	{
		mActors.push_back(actor);
	}

	void Layer::Delete(const Actor * actor)
	{
		//mActors.erase(actor);
	}

};
