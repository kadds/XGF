#pragma once
#include "Defines.hpp"
#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

namespace XGF::Shape
{
	class CastShadowAbleLight;

	class ShadowMapGenerator
	{
	public:
		ShadowMapGenerator();
		void Initialize(int width, int height);
		void Config(int width, int height);
		Texture * Generate(CastShadowAbleLight * light, const std::vector<Mesh *> & meshs, const LightShadowMatrix & wvp);
		void Shutdown();
		Texture * GetTexture();
	private:
		FrameBuffer mFrameBuffer;
		RenderState mRenderState;
		RenderResource mRenderResource;
	};
}
