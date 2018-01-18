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

	void Container::_Updata(float deltaTime)
	{
		for (auto it : mContainerChild)
		{
			it->_Updata(deltaTime);
		}
		for (auto it : mChild)
		{
			it->_Updata(deltaTime);
		}
	}

	void Container::SetParent(Container * c)
	{
		mParent = c;
	}

	Actor & Container::AddChild(Actor & actor)
	{
		mChild.push_back(&actor);
		actor.SetParent(this);
		actor.OnAddToContainer();
		return actor;
	}

	void Container::AddChild(Container & container)
	{
		mContainerChild.push_back(&container);
		container.SetParent(this);
		
	}

	void Container::RemoveChild(Actor & actor)
	{
		/*auto it = std::find(mChild.begin(), mChild.end(), actor);
		if (it != mChild.end())
		{
			actor.OnRemoveFromContainer();
			mChild.erase(it);
		}*/
		//TODO::Remove code
	}

	void Container::RemoveChild(Container & container)
	{
	}

}
