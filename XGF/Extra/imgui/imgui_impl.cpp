
#include "imgui_impl.hpp"
#include "../../Include/InputDefinitions.hpp"
#include "../../Include/Font.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/SystemShaders.hpp"
#include "../../Include/Renderer.hpp"

using namespace XGF;

namespace XGF::UI 
{
	int Imgui::mInstanceCount = 0;
	void Imgui::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		mInstanceCount++;
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; 
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; 
		io.BackendPlatformName = "imgui_impl_xgf";
		io.ImeWindowHandle = gdi.GetTopHwnd();
		

		io.KeyMap[ImGuiKey_Tab] = (int)Input::KeyBoardKey::Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = (int)Input::KeyBoardKey::Left;
		io.KeyMap[ImGuiKey_RightArrow] = (int)Input::KeyBoardKey::Right;
		io.KeyMap[ImGuiKey_UpArrow] = (int)Input::KeyBoardKey::Up;
		io.KeyMap[ImGuiKey_DownArrow] = (int)Input::KeyBoardKey::Down;
		io.KeyMap[ImGuiKey_PageUp] = (int)Input::KeyBoardKey::PageUp;
		io.KeyMap[ImGuiKey_PageDown] = (int)Input::KeyBoardKey::PageDown;
		io.KeyMap[ImGuiKey_Home] = (int)Input::KeyBoardKey::Home;
		io.KeyMap[ImGuiKey_End] = (int)Input::KeyBoardKey::End;
		io.KeyMap[ImGuiKey_Insert] = (int)Input::KeyBoardKey::Insert;
		io.KeyMap[ImGuiKey_Delete] = (int)Input::KeyBoardKey::Delete;
		io.KeyMap[ImGuiKey_Backspace] = (int)Input::KeyBoardKey::BackSpace;
		io.KeyMap[ImGuiKey_Space] = (int)Input::KeyBoardKey::SpaceBar;
		io.KeyMap[ImGuiKey_Enter] = (int)Input::KeyBoardKey::Enter;
		io.KeyMap[ImGuiKey_Escape] = (int)Input::KeyBoardKey::Escape;
		io.KeyMap[ImGuiKey_A] = (int)Input::KeyBoardKey::A;
		io.KeyMap[ImGuiKey_C] = (int)Input::KeyBoardKey::C;
		io.KeyMap[ImGuiKey_V] = (int)Input::KeyBoardKey::V;
		io.KeyMap[ImGuiKey_X] = (int)Input::KeyBoardKey::X;
		io.KeyMap[ImGuiKey_Y] = (int)Input::KeyBoardKey::Y;
		io.KeyMap[ImGuiKey_Z] = (int)Input::KeyBoardKey::Z;
		int width, height;
		
		width = gdi.GetWidth();
		height = gdi.GetHeight();
		Resize(width, height);

		Shaders shaders = SystemShaders::GetImguiShaders();
		mRenderResource = RenderResource(shaders);
		mRenderState = RenderState();
		auto& blendState = mRenderState.GetBlendState();
		blendState.SetAlphaToCoverageEnable(false);
		auto& renderTarget = blendState.GetRenderTarget(0);
		renderTarget.SetBlendEnable(true);
		renderTarget.SetSrcBlend(Blend::SRC_ALPHA);
		renderTarget.SetDestBlend(Blend::INV_SRC_ALPHA);
		renderTarget.SetBlendOp(BlendOp::ADD);
		renderTarget.SetSrcBlendAlpha(Blend::INV_SRC_ALPHA);
		renderTarget.SetDestBlendAlpha(Blend::ZERO);
		renderTarget.SetBlendOpAlpha(BlendOp::ADD);
		renderTarget.SetRenderTargetWriteMask(0xF);

		auto& rasterizer = mRenderState.GetRasterizerState();
		rasterizer.SetFillMode(FillMode::Solid);
		rasterizer.SetCullMode(CullMode::None);
		rasterizer.SetScissorEnable(true);
		rasterizer.SetDepthClipEnable(true);

		auto& depthState = mRenderState.GetDepthStencilState();
		depthState.SetDepthEnable(false);
		depthState.SetDepthWriteMask(true);
		depthState.SetDepthFunc(ComparisonFunc::ALWAYS);
		depthState.SetStencilEnable(false);

		depthState.SetFrontFaceStencilFailOp(StencilOp::KEEP);
		depthState.SetFrontFaceStencilDepthFailOp(StencilOp::KEEP);
		depthState.SetFrontFaceStencilPassOp(StencilOp::KEEP);
		depthState.SetFrontFaceStencilFunc(ComparisonFunc::ALWAYS);

		depthState.SetBackFaceStencilFailOp(StencilOp::KEEP);
		depthState.SetBackFaceStencilDepthFailOp(StencilOp::KEEP);
		depthState.SetBackFaceStencilPassOp(StencilOp::KEEP);
		depthState.SetBackFaceStencilFunc(ComparisonFunc::ALWAYS);

		RebuildFont();


		auto& samplerState = mRenderResource.GetSamplerState<PixelShader>()[0];
		samplerState.SetFilter(Filter::MIN_MAG_MIP_LINEAR);
		samplerState.SetAddressU(Address::WRAP);
		samplerState.SetAddressV(Address::WRAP);
		samplerState.SetAddressW(Address::WRAP);
		samplerState.SetMipLODBias(0.f);
		samplerState.SetComparisonFunc(ComparisonFunc::ALWAYS);
		samplerState.SetMinLOD(0.f);
		samplerState.SetMaxLOD(0.f);

		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const int indexCount = 2048;
		const int vertexCount = 1024;

		mIndexBinder = std::make_shared<PolygonPleIndex>(indexCount);
		mPointBinder = std::make_shared<PolygonPlePoint2Binder>(vertexCount);
		mTextureBinder = std::make_shared<PolygonPleTextureBinder>(vertexCount);
		mColorBinder = std::make_shared<PolygonPleColorBinder>(vertexCount);
		mBindingBridge.AddBinder(mPointBinder);
		mBindingBridge.AddBinder(mTextureBinder);
		mBindingBridge.AddBinder(mColorBinder);
	}
	void Imgui::Shutdown()
	{
		mInstanceCount--;
		ImGui::DestroyContext();
	}
	void Imgui::Begin()
	{
		auto& inputManager = Context::Current().QueryFramework().GetInputManager();
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = Context::Current().QueryFramework().GetTimeIntervalFromLastRender();
		io.KeyCtrl = inputManager.IskeyDown(Input::KeyBoardKey::LeftControl) | inputManager.IskeyDown(Input::KeyBoardKey::RightControl);
		io.KeyAlt = inputManager.IskeyDown(Input::KeyBoardKey::LeftAlt) | inputManager.IskeyDown(Input::KeyBoardKey::RightAlt);
		io.KeyShift = inputManager.IskeyDown(Input::KeyBoardKey::LeftShift) | inputManager.IskeyDown(Input::KeyBoardKey::RightShift);
		io.KeySuper = inputManager.IskeyDown(Input::KeyBoardKey::LeftCommand) | inputManager.IskeyDown(Input::KeyBoardKey::RightCommand);
		ImGui::NewFrame();
	}
	void Imgui::End()
	{
		ImGui::Render();
		auto draw_data = ImGui::GetDrawData();

		// Avoid rendering when minimized
		if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
			return;

		auto& context = Context::Current();
		auto& renderer = context.QueryRenderer();

		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		float mvp[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		};
		mRenderResource.SetConstantBuffer<VertexShader>(0, mvp);

		if (mIndexBinder->GetCapacity() < draw_data->TotalIdxCount || mPointBinder->GetCapacity() < draw_data->TotalVtxCount)
		{
			ResizeBuffer(draw_data->TotalIdxCount, draw_data->TotalVtxCount);
		}
		mIndexBinder->SetActualCount(draw_data->TotalIdxCount);
		mPointBinder->SetActualCount(draw_data->TotalVtxCount);
		mTextureBinder->SetActualCount(draw_data->TotalVtxCount);
		mColorBinder->SetActualCount(draw_data->TotalVtxCount);
		if (draw_data->TotalIdxCount == 0 || draw_data->TotalVtxCount == 0)
		{
			return;
		}
		int indexStart = 0;
		int vertexStart = 0;
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			int vtxCount = cmd_list->VtxBuffer.Size;

			memcpy(&mIndexBinder->Get(indexStart),
				cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

			for (int j = 0; j < vtxCount; j++)
			{
				int pos = vertexStart + j;
				auto vtxb = cmd_list->VtxBuffer.Data[j];

				mPointBinder->GetData(pos).Set(vtxb.pos.x, vtxb.pos.y);
				mColorBinder->GetData(pos).Set((vtxb.col & 0xff) / 255.0f, (vtxb.col >> 8 & 0xff) / 255.0f, (vtxb.col >> 16 & 0xff) / 255.0f, (vtxb.col >> 24 & 0xff) / 255.0f);
				mTextureBinder->GetData(pos).Set(vtxb.uv.x, vtxb.uv.y);
			}
			vertexStart += vtxCount;
			indexStart += cmd_list->IdxBuffer.Size;
		}

		BufferSharedRenderCommand::StartBaseRenderCommand(mBindingBridge, *mIndexBinder.get(), *mRenderResource.GetShader<VertexShader>());
		indexStart = 0;
		vertexStart = 0;
		ImVec4 lastRect;
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];

			int countIndex = 0;
			for (int i = 0; i < cmd_list->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[i];
				mRenderResource.SetTexture<PixelShader>(0, &mTextures[(int)pcmd->TextureId]);
				Flush(indexStart, vertexStart, ImVec4((float)(pcmd->ClipRect.x - draw_data->DisplayPos.x), (float)(pcmd->ClipRect.y - draw_data->DisplayPos.y),
					(float)(pcmd->ClipRect.z - draw_data->DisplayPos.x), (float)(pcmd->ClipRect.w - draw_data->DisplayPos.y)), pcmd->ElemCount);
				indexStart += pcmd->ElemCount;
			}
			vertexStart += cmd_list->VtxBuffer.Size;
		}

	}
	void Imgui::Resize(int width, int height)
	{
		auto& io = ImGui::GetIO();
		io.DisplaySize.x = (float)width;
		io.DisplaySize.y = (float)height;
	}
	void Imgui::OnInputEvent(const Event& ev)
	{
		ImGuiIO& io = ImGui::GetIO();
		switch (ev.GetEventGroup()) 
		{
		case EventGroupType::Mouse:
			if (ev.GetMouseEventId() == MouseEventId::MouseMove)
				io.MousePos = ImVec2((float) ev.GetDataMousePosition(0), (float) ev.GetDataMousePosition(1));
			else if (ev.GetMouseEventId() == MouseEventId::MouseDown)
			{
				int key = (int)ev.GetData<Input::MouseKey>(0);
				if (key < 5)
					io.MouseDown[key] = true;
			}
			else if (ev.GetMouseEventId() == MouseEventId::MouseUp)
			{
				int key = (int)ev.GetData<Input::MouseKey>(0);
				if (key < 5)
					io.MouseDown[key] = false;

			}
			else if (ev.GetMouseEventId() == MouseEventId::MouseWheel)
			{
				io.MouseWheel += (float)ev.GetDataMousePosition(0) / WHEEL_DELTA;
			}

			break;
		case EventGroupType::KeyBoard:
			if (ev.GetKeyBoardEventId() == KeyBoardEventId::KeyDown)
				io.KeysDown[(int)ev.GetData<Input::KeyBoardKey>(0)] = true;
			else if (ev.GetKeyBoardEventId() == KeyBoardEventId::KeyUp)
				io.KeysDown[(int)ev.GetData<Input::KeyBoardKey>(0)] = false;
			else if (ev.GetKeyBoardEventId() == KeyBoardEventId::Char)
				io.AddInputCharacter(ev.GetData<unsigned int>(0));
		}
	}
	std::string Imgui::Version()
	{
		return ImGui::GetVersion();
	}
	void Imgui::AddFont(const Font& font)
	{
		auto fontName = font.GetFontName();
		auto size = fontName.size();
		if (size >= 40)
			fontName = fontName.substr(fontName.size() - 40, fontName.size());

		ImFontConfig cfg;
		cfg.DstFont = 0;
		cfg.FontData = 0;
		cfg.FontDataSize = 0;
		cfg.FontDataOwnedByAtlas = false;
		cfg.FontNo = 0;
		cfg.GlyphExtraSpacing = ImVec2(0, 0);
		cfg.GlyphMaxAdvanceX = FLT_MAX;
		cfg.GlyphMinAdvanceX = 0;
		cfg.GlyphOffset = ImVec2(0, 0);
		cfg.GlyphRanges = 0;
		cfg.MergeMode = true;
		strcpy_s(cfg.Name, fontName.c_str());
		cfg.OversampleH = 3;
		cfg.OversampleV = 1;
		cfg.PixelSnapH = false;
		cfg.RasterizerFlags = 0;
		cfg.RasterizerMultiply = 1.0f;
		cfg.SizePixels = 0.f;

		ImGuiIO& io = ImGui::GetIO();
		
		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		builder.BuildRanges(&ranges);
		auto curfont = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(font.GetFontRawData()), font.GetFontRawDataSize(), (float)(font.GetFontSize()) , &cfg, ranges.Data);
		mFonts.try_emplace(&font, curfont);
		RebuildFont();
	}

	void Imgui::PushCurrentFont(const Font& font)
	{
		auto it = mFonts.find(&font);
		if (it != mFonts.end())
		{
			ImGui::PushFont(it->second);
		}
	}

	void Imgui::PopCurrentFont()
	{
		ImGui::PopFont();
	}

	void Imgui::RebuildFont()
	{
		unsigned char* pixels;
		int fwidth, fheight;
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&pixels, &fwidth, &fheight);
		
		mTextures.emplace_back();
		Texture& texture = mTextures.back();
		texture.Create(fwidth, fheight, TextureFormat::DXGI_FORMAT_R8G8B8A8_UNORM, (char*)pixels, fwidth * 4);

		// Store our identifier
		io.Fonts->TexID = (ImTextureID)(mTextures.size() - 1);
	}

	void Imgui::ResizeBuffer(int index, int vertex)
	{
		if (mIndexBinder->GetCapacity() != index)
			mIndexBinder = std::make_shared<PolygonPleIndex>(index);
		if (mPointBinder->GetCapacity() != vertex)
		{
			mPointBinder = std::make_shared<PolygonPlePoint2Binder>(vertex);
			mTextureBinder = std::make_shared<PolygonPleTextureBinder>(vertex);
			mColorBinder = std::make_shared<PolygonPleColorBinder>(vertex);
			mBindingBridge.Clear();
			mBindingBridge.AddBinder(mPointBinder);
			mBindingBridge.AddBinder(mTextureBinder);
			mBindingBridge.AddBinder(mColorBinder);
		}
		
	}
	void Imgui::Flush(int indexStart, int vertexStart, const ImVec4 & rect, unsigned drawCount)
	{
		std::vector<Rect> rects(1);
		rects[0].left = (long)rect.x;
		rects[0].top = (long)rect.y;
		rects[0].right = (long)rect.z;
		rects[0].bottom = (long)rect.w;
		mRenderState.SetScissorRects(rects);

		auto& context = Context::Current();
		auto& renderer = context.QueryRenderer();

		renderer.Commit(RenderGroupType::Normal,
			BufferSharedRenderCommand::MakeRenderCommand(mBindingBridge, *mIndexBinder.get(), RenderStage(mRenderState, mRenderResource), indexStart, vertexStart, drawCount));
	}
}

