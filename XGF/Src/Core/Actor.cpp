#include "../../Include/Actor.hpp"

#include "../../Include/Batch.hpp"
namespace XGF{
	Actor::Actor()
	{
		
	}
	
	Actor::~Actor()
	{
	}
	void Actor::_Updata(float deltaTime)
	{
		
	}
	void Actor::_Render()
	{
		DirectX::XMMATRIX matrix;
		matrix = mTransform.GetMatrix();
		Render(&matrix);
	}

}
