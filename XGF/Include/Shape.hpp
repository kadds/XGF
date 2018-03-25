#pragma once
#include "Defines.hpp"
#include "Polygon.hpp"
#include <d3d11_1.h>
#include "Transform.hpp"

namespace XGF
{
	class Batch;
	namespace Shape
	{
		typedef int ShapeType;
		/**
		* 定义了一些基本的可渲染图元
		*/
		class Shape
		{
		public:
			Shape(int n, int indexCount);
			virtual ~Shape();
			
			virtual void SetPosition(const Point & p) {};
			virtual void GetPosition(Point & p) const;

			PolygonPlePoint3 mPolygon;
			PolygonPleIndex mPolygonPleIndex;
			const PolygonPleIndex & GetIndex() const;
			void SetZ(float z);
			float GetZ() { return mPolygon.mPoint[0].z; }
			void Render(Batch & batch, const BindingBridge & bbrige, const Texture & tx);
			virtual void Render(Batch & batch, const BindingBridge & bbrige, ID3D11ShaderResourceView * tex);
			virtual void Render(Batch & batch, const BindingBridge & bbrige);

			Transform mTransform;
		private:
			
		};
		float triangleArea(Point a, Point b, Point c);
		//A Point is in Polygon inner?
		bool pInPolygon(const PolygonPlePoint3& ql, float x, float y);
	}
	
}

