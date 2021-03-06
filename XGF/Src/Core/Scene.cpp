#include "../../Include/Scene.hpp"
#include "../../Include/Container.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/EventDispatcher.hpp"
namespace XGF
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	void Scene::_Update(float dt)
	{
		Update(dt);
		mRootContainer._Update(dt);
		mRootContainer.GenerateMixMatrix(SM::Matrix::Identity, false);
	}
	void Scene::_Render()
	{
		Render();
	}
	void Scene::SwitchScene(std::shared_ptr<Scene> scene)
	{
		static_cast<XGFramework *>(&GetFramework())->SwitchScene(scene);
	}

	Container& Scene::GetRootContainer()
	{
		return mRootContainer;
	}

	void Scene::RenderUI(WVPMatrix& matrix)
	{
		mRootContainer._Render();
	}


	void Scene::_OnSize(const Event & ev)
	{
		OnSize(ev.GetDataInt(0), ev.GetDataInt(1));
	}

	XGFramework& Scene::GetFramework()
	{
		return *mFramework;
	}

	void Scene::AddChild(std::shared_ptr<Container> container)
	{
		container->SetParent(&mRootContainer);
		mRootContainer.AddChild(container);
		container->_OnCreate(this);
	}

	void Scene::_OnCreate(XGFramework * framework)
	{
		mFramework = framework;
		mRootContainer._OnCreate(this);
		mFramework->GetEventDispatcher().InsertAllEventListener(std::bind(&EventDispatcher::Dispatch, &mRootContainer.GetEventDispatcher(), std::placeholders::_1));
		mFramework->GetEventDispatcher().InsertSystemEventListener(SystemEventId::Size, std::bind(&Scene::_OnSize, this, std::placeholders::_1));
		mFramework->GetEventDispatcher().InsertSystemEventListener(SystemEventId::Activate, std::bind(&Scene::_OnActivate, this, std::placeholders::_1));
		OnCreate();
	}

	void Scene::_OnDestroy()
	{
		OnDestroy();
		mRootContainer._OnDestroy();
		mFramework->GetEventDispatcher().RemoveAllEventListener(std::bind(&EventDispatcher::Dispatch, &mRootContainer.GetEventDispatcher(), std::placeholders::_1));
		mFramework->GetEventDispatcher().RemoveSystemEventListener(SystemEventId::Size, std::bind(&Scene::_OnSize, this, std::placeholders::_1));
		mFramework->GetEventDispatcher().RemoveSystemEventListener(SystemEventId::Activate, std::bind(&Scene::_OnActivate, this, std::placeholders::_1));
	}

	void Scene::_OnActivate(const Event & ev)
	{
		OnActivate(ev.GetData<bool>(0));
	}

};
