#include "../../Include/Container.hpp"
#include "../../Include/Actor.hpp"
namespace XGF
{
	Container::Container() : mZOrder(0)
	{
	}
	Container::~Container()
	{
	}

	void Container::_OnCreate(Scene * scene)
	{
		mScene = scene;
		mParent = nullptr;
		mDFlag = true;
	}

	void Container::_OnDestroy()
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

	std::shared_ptr<Actor> Container::AddChild(std::shared_ptr<Actor> actor)
	{
		mChild.push_back(actor);
		actor->SetParent(this);
		actor->OnAddToContainer();
		return actor;
	}

	std::shared_ptr<Container> Container::AddChild(std::shared_ptr<Container> container)
	{
		mContainerChild.push_back(container);
		container->SetParent(this);
		GetEventDispatcher().InsertAllEventListener(std::bind(&EventDispatcher::Dispatch, &container->GetEventDispatcher(), std::placeholders::_1));
		container->SetScene(mScene);
		return container;
	}

	void Container::RemoveChild(std::shared_ptr<Actor> actor)
	{
		auto it = std::find(mChild.begin(), mChild.end(), actor);
		if (it != mChild.end())
		{
			actor->OnRemoveFromContainer();
			mChild.erase(it);
		}
	}

	void Container::RemoveChild(std::shared_ptr<Container> container)
	{
		auto it = std::find(mContainerChild.begin(), mContainerChild.end(), container);
		if (it != mContainerChild.end())
		{
			GetEventDispatcher().RemoveAllEventListener(std::bind(&EventDispatcher::Dispatch, &container->GetEventDispatcher(), std::placeholders::_1));
			container->_OnDestroy();
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
			it->_OnDestroy();
		mContainerChild.clear();
	}

	std::shared_ptr<Actor> Container::GetActorById(int Id, bool includeChildContainer)
	{
		for (auto it : mChild)
			if (it->GetId() == Id)
				return it;
		if (includeChildContainer)
		{
			for (auto it : mContainerChild)
			{
				std::shared_ptr<Actor> actor = it->GetActorById(Id, true);
				if (actor)
					return actor;
			}
		}
		return nullptr;
	}

	void Container::SetScene(Scene* scene)
	{
		mScene = scene;
	}

	bool Container::HasDFlag()
	{
		return mDFlag;
	}

	const SM::Matrix& Container::GetMixMatrix()
	{
		return mMatrix;
	}

	void Container::GenerateMixMatrix(const SM::Matrix & matrix, bool isChange)
	{
		bool c = isChange || mTransform.IsChange();
		if(c)
		{
			mMatrix = mTransform.GetMatrix() * matrix;
		}
		mDFlag = c;
		for (auto & child : mContainerChild)
		{
			child->GenerateMixMatrix(mMatrix, c);
		}
	}

	void Container::EventDispatch(const Event & e, EventDispatcher & edp)
	{
		edp.Dispatch(e);

	}

}
