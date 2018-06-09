#pragma once
#include "Geometry.hpp"

namespace XGF 
{
	namespace Shape 
	{
		class BoxGeometry : public Geometry
		{
		public:
			BoxGeometry();
			~BoxGeometry();
			BoxGeometry(float x, float y, float z);
			void SetSize(float x, float y, float z);
		private:
			void InitializeIndex();
		};
	}
}