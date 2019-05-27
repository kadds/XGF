#pragma once

#pragma push_macro("new")
#undef new
#include "imgui.h"
#pragma pop_macro("new")
#include "..//..//Include/Shader.hpp"
#include "..//..//Include/Polygon.hpp"

#include <unordered_map>
namespace XGF {
	class Event;
	class Font;
}
namespace XGF::UI {
	class Imgui {
	public:
		void Initialize();
		void Shutdown();

		void Begin();
		void End();

		void Resize(int width, int height);

		void OnInputEvent(const Event& ev);
		static std::string Version();

		void AddFont(const Font & font);
		void PushCurrentFont(const Font& font);
		void PopCurrentFont();
	private:
		void RebuildFont();
		void ResizeBuffer(int index, int vertex);
		void Flush(int indexStart, int vertexStart, const ImVec4& rect, unsigned drawCount);
		RenderState mRenderState;
		RenderResource mRenderResource;
		std::vector<Texture> mTextures;
		std::shared_ptr<PolygonPleIndex> mIndexBinder;
		std::shared_ptr<PolygonPlePoint2Binder> mPointBinder;
		std::shared_ptr<PolygonPleColorBinder> mColorBinder;
		std::shared_ptr<PolygonPleTextureBinder> mTextureBinder;
		BindingBridge mBindingBridge;
		static int mInstanceCount;
		std::unordered_map<const Font*, ImFont*> mFonts;
	};
}
