#pragma once
#include "Defines.hpp"
#include "SceneAnimation.hpp"
#include "Container.hpp"
namespace XGF
{
	class GDI;
	using std::vector;
	/*
	* ������
	*/
	class XGFramework;
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();
		Scene(const Scene&) = delete;
		Scene & operator = (const Scene &) = delete;

		void _Render(float deltaTime);
		virtual void Render(float deltaTime) = 0;
		void _Update(float deltaTime);
		virtual void Update(float deltaTime) = 0;
		virtual SceneAnimation * OnSwitchIn();
		virtual SceneAnimation * OnSwitchOut();

		void _OnSize(const Event & ev);
		virtual void OnSize(int ClientX, int ClientY) = 0;

		void _OnCreate(XGFramework * framework);
		virtual void OnCreate() = 0;
		void _OnDestroy();
		virtual void OnDestroy() = 0;

		void _OnActivate(const Event & ev);
		virtual void OnActivate(bool isActivate) {};
		XGFramework& GetFramework();
		void AddChild(std::shared_ptr<Container> container);
		void SwitchScene(std::shared_ptr<Scene> scene);

		Container& GetRootContainer();;

		void RenderUI(WVPMatrix & matrix);
	protected:
		Container mRootContainer;
	private:
		XGFramework *mFramework;
	};



}
