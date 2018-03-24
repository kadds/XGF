#pragma once
#include "Batches.hpp"
#include "Defines.hpp"
#include "Actions.hpp"
#include "SceneAnimation.hpp"
#include "Container.hpp"
#include <vector>
namespace XGF
{
	using std::vector;
	/*
	* ≥°æ∞¿‡
	*/
	class XGFramework;
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		void _Render(float deltaTime);
		virtual void Render(float deltaTime) = 0;
		void _Update(float deltaTime);
		virtual void Update(float deltaTime) = 0;
		virtual SceneAnimation * OnSwitchIn();
		virtual SceneAnimation * OnSwitchOut();

		void _OnSize(const Event & ev);
		virtual void OnSize(int ClientX, int ClientY) = 0;

		void _OnCreate(XGFramework * framework);
		virtual void OnCreate(GDI * gdi) = 0;
		void _OnDestroy();
		virtual void OnDestroy() = 0;

		void _OnActivate(const Event & ev);
		virtual void OnActivate(bool isActivate) {};
		XGFramework * GetFramework()
		{
			return mFramework;
		}
		void AddChild(std::shared_ptr<Container> container);
		void SwitchScene(Scene * scene);
		void Clear(const Color & c);
		void ClearDepthStencilBuffer();

		Container & GetRootContainer() { return mRootContainer; };
	protected:
		Container mRootContainer;

		XGFramework *mFramework;
	private:
		DISALLOW_COPY_AND_ASSIGN(Scene);
	};



}
