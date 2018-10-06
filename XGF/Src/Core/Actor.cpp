#include "../../Include/Actor.hpp"
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


	void Actor::_Render()
	{
		Render();
	}

}
