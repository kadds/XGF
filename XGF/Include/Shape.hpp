#pragma once
#include "Polygon.hpp"
#include "Defines.hpp"
#include <d3d11_1.h>

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
			virtual bool IsInBoundBox(const Point & p, const FXMMATRIX matrix) const {return false;};
			virtual void SetPosition(const Point & p) {};
			virtual void GetPosition(Point & p) const;
			PolygonPlePoint3 mPolygon;
			PolygonPleIndex mPolygonPleIndex;
			const PolygonPleIndex & GetIndex() const;
			virtual void GetBoundBox(PolygonPlePoint3 & ppe) const {};
			void SetZ(float z);
			float GetZ() { return mPolygon.mPoint[0].z; }
			void Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige, const Texture & tx);
			virtual void Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige, ID3D11ShaderResourceView * tex);
			virtual void Render(Batch & batch, const XMMATRIX * matirix, const BindingBridge & bbrige);
		private:
		};
		float triangleArea(Point a, Point b, Point c);
		//A Point is in Polygon inner?
		bool pInPolygon(const PolygonPlePoint3& ql, int x, int y);
	}
	
}

