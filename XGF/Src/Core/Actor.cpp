#include "../../Include/Actor.hpp"
#include "../../Include/Container.hpp"
namespace XGF{
	Actor::Actor():mParent(nullptr)
	{
	}

	Actor::Actor(int id):mId(id), mParent(nullptr)
	{
	}
	
	Actor::~Actor()
	{
	}

	void Actor::_Update(float deltaTime)
	{
		Update(deltaTime);
	}

	const SM::Matrix & Actor::GetMixMatrix()
	{
		bool c = GetShape()->GetTransform().IsChange() || mParent->HasDFlag();
		if (c)
			mMatrix = GetShape()->GetTransform().GetMatrix() * mParent->GetMixMatrix();
		return mMatrix;
	}


	void Actor::_Render()
	{
		Render();
	}

}
