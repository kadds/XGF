#pragma once
#include "Defines.hpp"
#include "Line.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include <memory>
namespace XGF
{
	class Camera;
	class AxisRenderer
	{
	public:
		AxisRenderer();
		~AxisRenderer();
		void Initialize(float len = 1e5);
		void SetAxisXColor(SM::Color color, SM::Color colorb);
		void SetAxisYColor(SM::Color color, SM::Color colorb);
		void SetAxisZColor(SM::Color color, SM::Color colorb);

		//call it after call Begin ,before call End
		void DrawAxis();

		void Shutdown();
		//every frame call it
		void Begin(const WVPMatrix & matrix);
		//every frame call it after call Begin
		void End();
	private:
		Batch mBatch;
		std::vector<std::unique_ptr<Shape::Line>> lines;
		SM::Color mColor[6];
	};


}
