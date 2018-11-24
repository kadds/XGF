#pragma once
#include "Defines.hpp"
#include <vector>
#include <functional>
#include "Transform.hpp"
#include "EventDispatcher.hpp"

namespace XGF
{
	typedef std::function<void(void)> OnCreateListener;
	typedef std::function<void(void)> OnDestroyListener;
	/*
	* containerÈÝÆ÷
	*/
	class Scene;
	class Actor;
	class Container
	{
	public:
		Container();
		~Container();

		void _OnCreate(Scene * scene);
		void _OnDestory();

		void _Render();
		void _Update(float deltaTime);

		void SetParent(Container * c);

		std::shared_ptr<Actor> AddChild(std::shared_ptr<Actor> actor);
		void AddChild(std::shared_ptr<Container> container);

		void RemoveChild(std::shared_ptr<Actor> actor);
		void RemoveChild(std::shared_ptr<Container> container);

		void RemoveAllChild();
		void RemoveAllActor();
		void RemoveAllContainer();

		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; };
		Transform & GetTransform() { return mTransform; };

		Scene & GetScene() { return *mScene; }

		void SetPosition(const Point & p) {};//TODO::
		void SetSize(const Point & size) {};

		float GetZOrder() { return mZOrder; };

		std::shared_ptr<Actor> GetActorById(int Id, bool includeChildContainer = false);
	protected:
		float mZOrder;
	private:
		Container * mParent;
		std::vector<std::shared_ptr<Actor>> mChild;
		std::vector<std::shared_ptr<Container>> mContainerChild;

		EventDispatcher mEventDispatcher;
		Point mSize;
		Point mPosition;
		Scene * mScene;
		Transform mTransform;
	};
}
