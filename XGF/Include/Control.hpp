#pragma once
#include "Defines.hpp"
#include "Actor.hpp"
#include <math.h>
namespace XGF
{
	class Layer;
	class UILayer;
	class Animation;
	class Rectangle;
	class Control : public Actor
	{
	public:
		friend class UILayer;
		Control();
		virtual ~Control();
		//Texture * GetTexture() { return &mTexture; }
		void SetLayer(Layer * l) { mLayer = l; }
	protected:
		virtual XMMATRIX CalculateMatrix()  const override;;
		bool mIsForce;
		Layer * mLayer;
	};


}
