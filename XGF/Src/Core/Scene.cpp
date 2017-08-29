#include "../../Include/Scene.hpp"
#include "../../Include/SceneFramework.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/ShaderConst.hpp"
#include "../../Include/Layer.hpp"
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

void Scene::RenderLayer(Batches & batches)
{
	for (auto i = mLayerList.begin(); i != mLayerList.end(); ++i)
	{
		(*i)->Render(batches);
	}
}
void Scene::SwitchScene(Scene * scene)
{
	static_cast<SceneFramework *>(GetFramework())->SwitchScene(scene);
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
