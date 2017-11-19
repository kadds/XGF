#include "../../Include/Scene.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/ShaderConst.hpp"
#include "../../Include/Layer.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
#include "..\..\Include\Scene.hpp"
Scene::Scene()
{
}


Scene::~Scene()
{
}
void Scene::UpdataLayer(float dt)
{
    for (auto i = mLayerList.begin(); i != mLayerList.end(); ++i)
    {
        (*i)->Updata(dt);
    }
}

void Scene::RenderLayer(WVPMatrix & wvp)
{
	mFramework->GetGDI()->ClearDepthStencilBuffer();
	for (auto i = mLayerList.begin(); i != mLayerList.end(); ++i)
	{
		(*i)->BeginBatches(wvp);
		(*i)->Render();
		(*i)->EndBatches();
	}
}
void Scene::SwitchScene(Scene * scene)
{
	static_cast<XGFramework *>(GetFramework())->SwitchScene(scene);
}
void Scene::Clear(Color & c)
{
	mFramework->GetGDI()->Clear(c);
}
void Scene::ClearDepthStencilBuffer()
{
	mFramework->GetGDI()->ClearDepthStencilBuffer();
}
void Scene::OnSwitchIn()
{
}

void Scene::OnSwitchOut()
{
}

void Scene::AddLayer(Layer * layer)
{
	layer->SetParent(this);
	
	mLayerList.push_back(layer);
	layer->OnCreate();
}

void Scene::SetFramework(XGFramework * framework)
{
	mFramework = framework;
}

void Scene::OnCreate()
{
	
}

void Scene::OnDestory()
{
	for (auto it = mLayerList.begin(); it != mLayerList.end(); ++it)
	{
		(*it)->OnDestory();
	}
	mLayerList.clear();
}

int Scene::GetWidth()
{
	return mFramework->GetWindowsWidth(); 
}

int Scene::GetHeight()
{
	return mFramework->GetWindowsHeight();
}
