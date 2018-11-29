#include "../../Include/Scene.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Container.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
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
	void Scene::_Render(float deltaTime)
	{
		Render(deltaTime);
	}
	void Scene::SwitchScene(std::shared_ptr<Scene> scene)
	{
		static_cast<XGFramework *>(&GetFramework())->SwitchScene(scene);
	}
	void Scene::Clear(const SM::Color & c)
	{
		mFramework->GetGDI().Clear(c);
	}
	void Scene::ClearDepthStencilBuffer()
	{
		mFramework->GetGDI().ClearDepthStencilBuffer();
	}

	Container& Scene::GetRootContainer()
	{
		return mRootContainer;
	}

	void Scene::RenderUI(WVPMatrix& matrix)
	{
		GetFramework().GetUIBatches().Begin(matrix);
		mRootContainer._Render();
		GetFramework().GetUIBatches().End();
	}

	SceneAnimation * Scene::OnSwitchIn()
	{
		return nullptr;
	}
	SceneAnimation * Scene::OnSwitchOut()
	{
		return nullptr;
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
		OnCreate(&framework->GetGDI());
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
