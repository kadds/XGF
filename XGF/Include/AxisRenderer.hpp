#pragma once
#include "Defines.hpp"
#include "Line.hpp"
#include "Shader.hpp"
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
		void SetAxisXColor(Color color, Color colorb);
		void SetAxisYColor(Color color, Color colorb);
		void SetAxisZColor(Color color, Color colorb);

		//call it after call Begin ,before call End
		void DrawAxis();

		void Shutdown();
		//every frame call it
		void Begin(const WVPMatrix & matrix);
		//every frame call it after call Begin
		void End();
	private:
		ShaderStage mShaderStage;
		std::vector<std::unique_ptr<Shape::Line>> lines;
		Color mColor[6];
	};


}
