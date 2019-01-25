#pragma once
#include "Defines.hpp"
#include "Transform.hpp"
#include "Polygon.hpp"

namespace XGF
{
	class Batch;
	class PolygonPleIndex;
	
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

			std::shared_ptr<PolygonPlePointBinder> mPolygon;
			std::shared_ptr<PolygonPleIndex> mPolygonPleIndex;
			std::shared_ptr<PolygonPleIndex> GetIndex() const;
			void SetZ(float z);
			float GetZ();

			Transform & GetTransform() { return mTransform; };
		protected:
			void Reset(int n, int indexCount);
			Transform mTransform;
		private:
			//TopologyMode
		};
		float triangleArea(Point a, Point b, Point c);
		//A Point is in Polygon inner?
		bool pInPolygon(const std::shared_ptr<PolygonPlePointBinder> & ql, float x, float y);
	}
	
}

