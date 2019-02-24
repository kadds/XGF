#pragma once
#include "Defines.hpp"
#include "Mesh.hpp"
#include "Light/Light.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

namespace XGF
{
	class RenderPath
	{
	public:
		virtual void Render(Camera & camera, std::vector<Shape::Mesh *> & meshes, std::vector<Shape::Light *> & lights);
		virtual Shaders LoadShaders(Shape::Mesh * mesh);

	protected:
		RenderState mRenderState;
		std::vector<RenderResource> mCachedRenderResources;
	};

	class ForwardRenderPath : public RenderPath
	{
		
	};
}
