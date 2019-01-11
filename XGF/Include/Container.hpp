#pragma once
#include "Defines.hpp"
#include <functional>
#include "Transform.hpp"
#include "EventDispatcher.hpp"

namespace XGF
{
	typedef std::function<void(void)> OnCreateListener;
	typedef std::function<void(void)> OnDestroyListener;
	/*
	* container����
	*/
	class Scene;
	class Actor;
	class Container
	{
	public:
		Container();
		~Container();

		void _OnCreate(Scene * scene);
		void _OnDestroy();

		void _Render();
		void _Update(float deltaTime);

		void SetParent(Container * c);

		std::shared_ptr<Actor> AddChild(std::shared_ptr<Actor> actor);
		std::shared_ptr<Container> AddChild(std::shared_ptr<Container> container);

		void RemoveChild(std::shared_ptr<Actor> actor);
		void RemoveChild(std::shared_ptr<Container> container);

		void RemoveAllChild();
		void RemoveAllActor();
		void RemoveAllContainer();

		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; };
		Transform & GetTransform() { return mTransform; };

		Scene & GetScene() { return *mScene; }

		float GetZOrder() const { return mZOrder; };

		std::shared_ptr<Actor> GetActorById(int Id, bool includeChildContainer = false);

		void SetScene(Scene * scene);

		bool HasDFlag();
		const SM::Matrix & GetMixMatrix();
	protected:
		float mZOrder;
	public:
		void GenerateMixMatrix(const SM::Matrix & matrix, bool isChange);
	private:
		static void EventDispatch(const Event & e, EventDispatcher & edp);
		Container * mParent;
		std::vector<std::shared_ptr<Actor>> mChild;
		std::vector<std::shared_ptr<Container>> mContainerChild;

		EventDispatcher mEventDispatcher;
		Scene * mScene;
		Transform mTransform;
		SM::Matrix mMatrix;
		bool mDFlag;
	};
}
