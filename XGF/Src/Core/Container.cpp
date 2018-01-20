#include "../../Include/Container.hpp"
#include "../../Include/Actor.hpp"
namespace XGF
{
	Container::Container()
	{
	}
	Container::~Container()
	{
	}

	void Container::_OnCreate(Scene * scene)
	{
		mScene = scene;
		mParent = nullptr;
	}

	void Container::_OnDestory()
	{
		RemoveAllChild();
		mScene = nullptr;
		mParent = nullptr;
	}

	void Container::_Render()
	{
		for (auto it : mContainerChild)
		{
			it->_Render();
		}
		for (auto it : mChild)
		{
			it->_Render();
		}
	}

	void Container::_Update(float deltaTime)
	{
		for (auto it : mContainerChild)
		{
			it->_Update(deltaTime);
		}
		for (auto it : mChild)
		{
			it->_Update(deltaTime);
		}
	}

	void Container::SetParent(Container * c)
	{
		mParent = c;
	}

	Actor * Container::AddChild(Actor * actor)
	{
		mChild.push_back(actor);
		actor->SetParent(this);
		actor->OnAddToContainer();
		return actor;
	}

	void Container::AddChild(Container * container)
	{
		mContainerChild.push_back(container);
		container->SetParent(this);
		
	}

	void Container::RemoveChild(Actor & actor)
	{
		auto it = std::find(mChild.begin(), mChild.end(), &actor);
		if (it != mChild.end())
		{
			actor.OnRemoveFromContainer();
			mChild.erase(it);
		}
	}

	void Container::RemoveChild(Container & container)
	{
		auto it = std::find(mContainerChild.begin(), mContainerChild.end(), &container);
		if (it != mContainerChild.end())
		{
			container._OnDestory();
			mContainerChild.erase(it);
		}
	}

	void Container::RemoveAllChild()
	{
		RemoveAllContainer();
		RemoveAllActor();
	}

	void Container::RemoveAllActor()
	{
		for (auto it : mChild)
			it->OnRemoveFromContainer();
		mChild.clear();
	}

	void Container::RemoveAllContainer()
	{
		for (auto it : mContainerChild)
			it->_OnDestory();
		mContainerChild.clear();
	}

	Actor * Container::GetActorById(int Id, bool includeChildContainer)
	{
		for (auto it : mChild)
			if (it->GetId() == Id)
				return it;
		if (includeChildContainer)
		{
			Actor * actor = nullptr;
			for (auto it : mContainerChild)
			{
				actor = it->GetActorById(Id, true);
				if (actor != nullptr)
					return actor;
			}
		}
		return nullptr;
	}

}
