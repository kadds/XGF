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

			std::shared_ptr<PolygonPlePoint3> mPolygon;
			std::shared_ptr<PolygonPleIndex> mPolygonPleIndex;
			std::shared_ptr<PolygonPleIndex> GetIndex() const;
			void SetZ(float z);
			float GetZ() { return mPolygon->mPoint[0].z; }

			Transform & GetTransform() { return mTransform; };
		protected:
			void Reset(int n, int indexCount);
			Transform mTransform;
		private:
			//TopologyMode
		};
		float triangleArea(Point a, Point b, Point c);
		//A Point is in Polygon inner?
		bool pInPolygon(const std::shared_ptr<PolygonPlePoint3> & ql, float x, float y);
	}
	
}

