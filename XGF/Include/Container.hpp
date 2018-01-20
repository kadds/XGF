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

		Actor * AddChild(Actor * actor);
		void AddChild(Container * container);

		void RemoveChild(Actor & actor);
		void RemoveChild(Container & container);

		void RemoveAllChild();
		void RemoveAllActor();
		void RemoveAllContainer();

		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; };
		Transform & GetTransform() { return mTransform; };

		Scene * GetScene() { return mScene; }

		void SetPosition(Point & p) {};//TODO::
		void SetSize(Point & size) {};

		float GetZOrder() { return mZOrder; };

		Actor * GetActorById(int Id, bool includeChildContainer = false);

	protected:
		float mZOrder;
	private:
		Container * mParent;
		std::vector<Actor *> mChild;
		std::vector<Container *> mContainerChild;

		EventDispatcher mEventDispatcher;
		Point mSize;
		Point mPosition;
		Scene * mScene;
		Transform mTransform;
	};
}
