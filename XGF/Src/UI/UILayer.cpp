#include "../../Include/UILayer.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/DX8Input.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/InputManager.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF
{
	UILayer::UILayer()
	{
	}


	UILayer::~UILayer()
	{
	}

	void UILayer::OnCreate()
	{
		mBatches = (Batches *)new UIBatches();
		mBatches->Initialize(mParent->GetFramework()->GetGDI());
	}

	void UILayer::OnDestory()
	{
		mBatches->Shutdown();
		delete mBatches;
	}

	void UILayer::Render()
	{
		for each (auto var in mActors)
		{
			(const_cast<Actor *>(var))->_Render(*mBatches);
		}
	}

	void UILayer::Updata(float deltaTime)
	{
		for each (auto var in mActors)
		{
			(const_cast<Actor *>(var))->_Updata(deltaTime);
		}
	}

	void UILayer::OnSize(int ClientX, int ClientY)
	{
	}

	void UILayer::OnMouseDowm(const MousePoint & mp, int pk)
	{
		if (pk & MOUSE_POSITION_RELATIVEMODE) return;
		bool isin;

		for each (auto var in mActors)
		{
			if (var->CanGetMouseEvent())
			{
				isin = const_cast<Actor*>(var)->IsInBBox(Point(static_cast <float>(mp.x), static_cast <float>(mp.y)
					, static_cast <float>(0.0f)));
				const_cast<Actor*>(var)->OnMouseDowm(mp, pk, isin);
			}
		}
	}
	void UILayer::OnMouseUp(const MousePoint & mp, int pk)
	{
		if (pk & MOUSE_POSITION_RELATIVEMODE) return;
		bool isin;
		for each (auto var in mActors)
		{
			if (var->CanGetMouseEvent())
			{
				isin = const_cast<Actor*>(var)->IsInBBox(Point(static_cast <float>(mp.x), static_cast <float>(mp.y), static_cast <float>(0.0f)));
				const_cast<Actor*>(var)->OnMouseUp(mp, pk, isin);
			}
		}
	}
	void UILayer::OnMouseMove(const MousePoint & mp, int pk)
	{
		if (pk & MOUSE_POSITION_RELATIVEMODE) return;
		bool isin;
		for each (auto var in mActors)
		{
			if (var->CanGetMouseEvent())
			{
				isin = const_cast<Actor*>(var)->IsInBBox(Point(static_cast<float>(mp.x), static_cast <float>(mp.y), static_cast <float>(0.0f)));
				const_cast<Actor*>(var)->OnMouseMove(mp, pk, isin);
			}
		}
	}
	void UILayer::OnKeyDowm(Key k)
	{

	}
	void UILayer::OnKeyUp(Key k)
	{

	}

	void UILayer::Add(const Actor * actor)
	{
		static_cast<Control *>(const_cast<Actor *>(actor))->SetLayer(this);
		Layer::Add(actor);
	}

}
