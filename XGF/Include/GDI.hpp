#pragma once
#include "Defines.hpp"
#include <optional>
#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include "FrameBuffer.hpp"
#include <stack>

namespace XGF
{
	struct ViewPort 
	{
		float left;
		float top;
		float width;
		float height;
		float minDepth;
		float maxDepth;
		ViewPort(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f)
			: left(left), top(top), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth)
		{
		}
		ViewPort() {}
	};
	struct Rect
	{
		long left;
		long top;
		long right;
		long bottom;
		bool operator == (const Rect& rc) const
		{
			return rc.left == left && rc.right == right && rc.top == top && rc.bottom == bottom;
		}
	};

	class VPtrPlaceholder
	{
	public:
		void * vptr;
		template<typename Type>
		Type * GetPtr()
		{
			return static_cast<Type *>(vptr);
		}
		void SetPtr(void * v)
		{
			vptr = v;
		}
		VPtrPlaceholder(void * ptr = nullptr)
		{
			vptr = ptr;
		}
	};
	typedef VPtrPlaceholder RawSamplerState;
	typedef VPtrPlaceholder RawDepthStencilState;
	typedef VPtrPlaceholder RawRasterizerState;
	typedef VPtrPlaceholder RawBlendState;

	enum class DisplayMode
	{
		Windowed,
		Borderless,
		FullScreen
	};
	enum class ComparisonFunc
	{
		NEVER = 1,
		LESS = 2,
		EQUAL = 3,
		LESS_EQUAL = 4,
		GREATER = 5,
		NOT_EQUAL = 6,
		GREATER_EQUAL = 7,
		ALWAYS = 8,
	};
	enum class Filter
	{
		MIN_MAG_MIP_POINT = 0,
		MIN_MAG_POINT_MIP_LINEAR = 0x1,
		MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
		MIN_POINT_MAG_MIP_LINEAR = 0x5,
		MIN_LINEAR_MAG_MIP_POINT = 0x10,
		MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
		MIN_MAG_LINEAR_MIP_POINT = 0x14,
		MIN_MAG_MIP_LINEAR = 0x15,
		ANISOTROPIC = 0x55,
		COMPARISON_MIN_MAG_MIP_POINT = 0x80,
		COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
		COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
		COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
		COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
		COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
		COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
		COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
		COMPARISON_ANISOTROPIC = 0xd5,
		MINIMUM_MIN_MAG_MIP_POINT = 0x100,
		MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
		MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
		MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
		MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
		MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
		MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
		MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
		MINIMUM_ANISOTROPIC = 0x155,
		MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
		MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
		MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
		MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
		MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
		MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
		MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
		MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
		MAXIMUM_ANISOTROPIC = 0x1d5
	};
	enum class StencilOp
	{
		KEEP = 1,
		ZERO = 2,
		REPLACE = 3,
		INCR_SAT = 4,
		DECR_SAT = 5,
		INVERT = 6,
		INCR = 7,
		DECR = 8
	};

	enum class Address
	{
		WRAP = 1,
		MIRROR = 2,
		CLAMP = 3,
		BORDER = 4,
		MIRROR_ONCE = 5
	};
	class GDI;
	template<typename BaseStructType, typename RawStateType>
	class CacheState
	{
	protected:
		std::unordered_map<BaseStructType, RawStateType, BaseStructType> mCacheStates;
	public:
		std::unordered_map<BaseStructType, RawStateType, BaseStructType> & GetMap()
		{
			return mCacheStates;
		}
		std::optional<RawStateType> GetRawState(const BaseStructType & type)
		{
			auto it = mCacheStates.find(type);
			if (it != mCacheStates.end()) return it->second;
			else return {};
		}
		void SaveRawState(const BaseStructType & type, RawStateType rawState)
		{
			mCacheStates[type] = rawState;
		}
	};
	struct SamplerState
	{
		ClassPropertyWithInit(MaxAnisotropy, unsigned int, 1)
		ClassPropertyWithInit(MaxLOD, float, D3D11_FLOAT32_MAX)
		ClassPropertyWithInit(MinLOD, float, 0.f)
		ClassPropertyWithInit(MipLODBias, float, 0.f)
		ClassPropertyWithInit(BorderColor, Point4, Point4(0.f, 0.f, 0.f, 0.f))
		ClassPropertyWithInit(ComparisonFunc, ComparisonFunc, ComparisonFunc::ALWAYS)
		ClassPropertyWithInit(Filter, Filter, Filter::MIN_MAG_MIP_LINEAR)
		ClassPropertyWithInit(AddressU, Address, Address::WRAP)
		ClassPropertyWithInit(AddressV, Address, Address::WRAP)
		ClassPropertyWithInit(AddressW, Address, Address::WRAP)
	public:
		bool operator == (const SamplerState & s) const
		{
			return mMaxAnisotropy == s.mMaxAnisotropy && mAddressU == s.mAddressU && mAddressV == s.mAddressV && mAddressW == s.mAddressW 
				&& mBorderColor == s.mBorderColor && mComparisonFunc == s.mComparisonFunc && s.mFilter == mFilter && mMaxLOD == s.mMaxLOD
				&& mMinLOD == s.mMinLOD && mMipLODBias == s.mMipLODBias;
		}
		size_t operator()(const SamplerState & p) const {
			return hash_val(mMaxAnisotropy, (int)mAddressU, (int)mAddressV, (int)mAddressW, mBorderColor.x, mBorderColor.y, mBorderColor.z, mBorderColor.w, (int)mComparisonFunc, (int)mFilter, mMaxLOD, mMinLOD, mMipLODBias);
		}
	};
	enum class Blend
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DEST_ALPHA = 7,
		INV_DEST_ALPHA = 8,
		DEST_COLOR = 9,
		INV_DEST_COLOR = 10,
		SRC_ALPHA_SAT = 11,
		BLEND_FACTOR = 14,
		INV_BLEND_FACTOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19
	};

	enum class BlendOp
	{
		ADD = 1,
		SUBTRACT = 2,
		REV_SUBTRACT = 3,
		MIN = 4,
		MAX = 5
	};

	struct DepthStencilState
	{
		ClassPropertyWithInit(DepthEnable, bool, true)
		ClassPropertyWithInit(DepthFunc, ComparisonFunc, ComparisonFunc::LESS)
		ClassPropertyWithInit(DepthWriteMask, bool, true)
		ClassPropertyWithInit(StencilEnable, bool, false)
		ClassPropertyWithInit(StencilReadMask, unsigned __int8, 0xFF)
		ClassPropertyWithInit(StencilWriteMask, unsigned __int8, 0xFF)

		ClassPropertyWithInit(FrontFaceStencilDepthFailOp, StencilOp, StencilOp::KEEP)
		ClassPropertyWithInit(FrontFaceStencilFailOp, StencilOp, StencilOp::KEEP)
		ClassPropertyWithInit(FrontFaceStencilPassOp, StencilOp, StencilOp::REPLACE)
		ClassPropertyWithInit(FrontFaceStencilFunc, ComparisonFunc, ComparisonFunc::ALWAYS)

		ClassPropertyWithInit(BackFaceStencilDepthFailOp, StencilOp, StencilOp::KEEP)
		ClassPropertyWithInit(BackFaceStencilFailOp, StencilOp, StencilOp::KEEP)
		ClassPropertyWithInit(BackFaceStencilPassOp, StencilOp, StencilOp::REPLACE)
		ClassPropertyWithInit(BackFaceStencilFunc, ComparisonFunc, ComparisonFunc::ALWAYS)
	public:
		bool operator == (const DepthStencilState & b) const
		{
			return mDepthEnable == b.mDepthEnable && mDepthFunc == b.mDepthFunc && mDepthWriteMask == b.mDepthWriteMask && b.mStencilEnable == mStencilEnable
				&& mStencilReadMask == b.mStencilReadMask && mStencilWriteMask == b.mStencilWriteMask && mFrontFaceStencilDepthFailOp == b.mFrontFaceStencilDepthFailOp
				&& mFrontFaceStencilFailOp == b.mFrontFaceStencilFailOp && mFrontFaceStencilPassOp == b.mFrontFaceStencilPassOp
				&& mFrontFaceStencilFunc == b.mFrontFaceStencilFunc && mBackFaceStencilDepthFailOp == b.mBackFaceStencilDepthFailOp
				&& mBackFaceStencilFailOp == b.mBackFaceStencilFailOp && mBackFaceStencilPassOp == b.mBackFaceStencilPassOp && mBackFaceStencilFunc == b.mBackFaceStencilFunc
			;
		}
		size_t  operator()(const DepthStencilState & d) const
		{
			return hash_val(d.mDepthEnable, (int)d.mDepthFunc, d.mDepthWriteMask, d.mStencilEnable, d.mStencilReadMask, d.mStencilWriteMask, (int)d.mFrontFaceStencilDepthFailOp
			, (int)d.mFrontFaceStencilFailOp, (int)d.mFrontFaceStencilPassOp, (int)d.mFrontFaceStencilFunc, (int)d.mBackFaceStencilDepthFailOp, (int)d.mBackFaceStencilFailOp
			, (int)d.mBackFaceStencilPassOp, (int)d.mBackFaceStencilFunc);
		}
	};
	enum class FillMode
	{
		WireFrame = 2,
		Solid = 3,
	};
	enum class CullMode
	{
		None = 1,
		Front = 2,
		Back = 3
	};
	struct RasterizerState
	{
		ClassPropertyWithInit(FillMode, FillMode, FillMode::Solid)
		ClassPropertyWithInit(CullMode, CullMode, CullMode::Back)
		ClassPropertyWithInit(AntialiasedLineEnable, bool, false)
		ClassPropertyWithInit(DepthBias, int, 0)
		ClassPropertyWithInit(ForcedSampleCount, int, 0)
		ClassPropertyWithInit(DepthBiasClamp, float, 0.f)
		ClassPropertyWithInit(DepthClipEnable, bool, true)
		ClassPropertyWithInit(FrontCounterClockwise, bool, false)
		ClassPropertyWithInit(MultisampleEnable, bool, false)
		ClassPropertyWithInit(ScissorEnable, bool, false)
		ClassPropertyWithInit(SlopeScaledDepthBias, float, 0.f)
	public:
		bool operator == (const RasterizerState & b) const
		{
			return mFillMode == b.mFillMode && mCullMode == b.mCullMode && mAntialiasedLineEnable == b.mAntialiasedLineEnable && b.mDepthBias == mDepthBias
				&& mForcedSampleCount == b.mForcedSampleCount && mDepthBiasClamp == b.mDepthBiasClamp && mDepthClipEnable == b.mDepthClipEnable && mFrontCounterClockwise == b.mFrontCounterClockwise
				&& mMultisampleEnable == b.mMultisampleEnable && mScissorEnable == b.mScissorEnable && mSlopeScaledDepthBias == b.mSlopeScaledDepthBias;
		}
		size_t operator()(const RasterizerState & r) const 
		{
			return hash_val((int)r.mFillMode, (int)r.mCullMode, r.mAntialiasedLineEnable, r.mDepthBias, r.mForcedSampleCount, r.mDepthBiasClamp,
				r.mDepthClipEnable, r.mFrontCounterClockwise, r.mMultisampleEnable, r.mScissorEnable, r.mSlopeScaledDepthBias);
		}
	};
	struct BlendStateTarget
	{
		ClassPropertyWithInit(BlendEnable, bool, false)
		ClassPropertyWithInit(SrcBlend, Blend, Blend::SRC_ALPHA)
		ClassPropertyWithInit(DestBlend, Blend, Blend::INV_SRC_ALPHA)
		ClassPropertyWithInit(BlendOp, BlendOp, BlendOp::ADD)
		ClassPropertyWithInit(SrcBlendAlpha, Blend, Blend::ONE)
		ClassPropertyWithInit(DestBlendAlpha, Blend, Blend::ZERO)
		ClassPropertyWithInit(BlendOpAlpha, BlendOp, BlendOp::ADD)
		ClassPropertyWithInit(RenderTargetWriteMask, unsigned __int8, 0x0F)
	public:
		bool operator == (const BlendStateTarget & b) const
		{
			return mBlendEnable == b.mBlendEnable && mSrcBlend == b.mSrcBlend && b.mDestBlend == mDestBlend
				&& mBlendOp == b.mBlendOp && mSrcBlendAlpha == b.mSrcBlendAlpha && mDestBlendAlpha == b.mDestBlendAlpha && mBlendOpAlpha == b.mBlendOpAlpha
				&& mRenderTargetWriteMask == b.mRenderTargetWriteMask;
		}
		size_t hash() const 
		{
			return hash_val(mBlendEnable, (int)mSrcBlend, (int)mDestBlend, (int)mBlendOp, (int)mSrcBlendAlpha,
				(int)mDestBlendAlpha, (int)mBlendOpAlpha, mRenderTargetWriteMask);
		}
	};
	struct BlendState
	{
		ClassPropertyWithInit(AlphaToCoverageEnable, bool, false)
		ClassPropertyWithInit(IndependentBlendEnable, bool, false)
		ClassArrayProperty(RenderTarget, BlendStateTarget, 8)
	private:
		bool SameAs(int index, const BlendState & b) const
		{
			return mRenderTarget[index] == b.mRenderTarget[index];
		}
	public:
		bool operator == (const BlendState & b) const
		{
			if(mIndependentBlendEnable && b.mIndependentBlendEnable)
			{
				if(mAlphaToCoverageEnable == b.mAlphaToCoverageEnable)
				{
					for(int i = 0; i < GetRenderTargetCount(); i++)
					{
						if (!SameAs(i, b))
							return false;
					}
					return true;
				}
				return false;
			}
			else
			{
				return mAlphaToCoverageEnable == b.mAlphaToCoverageEnable  && SameAs(0, b);
			}
		}
		size_t operator()(const BlendState & b) const
		{
			if (b.mIndependentBlendEnable)
			{
				size_t hs = hash_val(b.mAlphaToCoverageEnable, b.mIndependentBlendEnable);
				for (int i = 0; i < GetRenderTargetCount(); i++)
				{
					hash_combine_value(hs, b.mRenderTarget[i].hash());
				}
				return hs;
			}
			else
			{
				return hash_val(b.mAlphaToCoverageEnable, b.mIndependentBlendEnable, b.mRenderTarget[0].hash());
			}
		}
	};
	struct GpuBuffer
	{
		ID3D11Buffer * buffer;
		unsigned int size;
		GpuBuffer(ID3D11Buffer * b, unsigned int s) : buffer(b), size(s) {  };
	};
	enum class GpuBufferType
	{
		Static,
		Dynamic,
	};
	class GDI
	{
	public:
		explicit GDI() {}
		~GDI() {}
		GDI(GDI &) = delete;
		GDI & operator=(GDI &) = delete;

		void Create();
		void Destroy();
		void Present(bool isVsync);
		void Initialize(HINSTANCE instance, HWND WndHwnd, HWND TopHwnd, UINT ClientWidth, UINT ClientHeight);

		void ResizeTarget(UINT x, UINT y);
		void SetFullScreen(bool isFullscreen, int pos);

		ID3D11Device * GetDevice() { return mD3dDevice; }
		ID3D11DeviceContext * GetDeviceContext() { return mDeviceContext; }

		GpuBuffer CreateIndexBuffer(unsigned len, GpuBufferType type, void * dataAddress = nullptr);
		GpuBuffer CreateVertexBuffer(unsigned len, GpuBufferType type, void * dataAddress = nullptr);
		GpuBuffer CreateConstantBuffer(unsigned size);

		void * Map(GpuBuffer & gpuBuffer);
		void UnMap(GpuBuffer & gpuBuffer);
		void CopyToBuffer(GpuBuffer& buffer, void * src);

		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }
		HWND GetHwnd();
		HWND GetTopHwnd();
		HINSTANCE GetInstance();
		
		bool IsFullScreen() { return mDisplayMode == DisplayMode::FullScreen; }
		bool SetDisplayMode(DisplayMode dm, int left, int top, int cx, int cy, bool move, bool isClientSize = false);
		ID3D11ShaderResourceView* CreateRenderableTexture(unsigned width, unsigned height, DXGI_FORMAT format, char* ptrContent, int pitch, int slicePitch);
		DisplayMode GetDisplayMode() { return  mDisplayMode; }

		int GetFullScreenDisplayModes(DXGI_MODE_DESC ** c) { *c = mScreenMode[0].second; return mScreenMode[0].first; }
		void CheckFullScreenForce(bool isforce);
		RawBlendState  CreateBlendState(const BlendState & bs);
		RawBlendState  GetBlendState(const BlendState & bs);

		RawDepthStencilState CreateDepthStencilState(const DepthStencilState & ds);
		RawDepthStencilState GetDepthStencilState(const DepthStencilState & ds);

		RawRasterizerState CreateRasterizerState(const RasterizerState & rs);
		RawRasterizerState GetRasterizerState(const RasterizerState & rs);

		RawSamplerState CreateSamplerState(const SamplerState & ss);
		RawSamplerState GetSamplerState(const SamplerState & ss);

		D3D_FEATURE_LEVEL CheckFeatureLevel();
		void SetBlendState(RawBlendState bs);
		void SetDepthStencilState(RawDepthStencilState ds);
		void SetRasterizerState(RawRasterizerState rs);
		FrameBuffer * GetDisplayFrameBuffer()
		{
			return &mDisplayFrameBuffer;
		}
		void CreateSwapChain();
		void ReCreateSwapChain();
		void Able4xMsaa();
		void Disable4xMsaa();
		bool IsEnable4xMsaa() const;
		int Query4xMsaaQuality() const;
		bool CanEnable4xMsaa() const;
		bool IsDisplayMode(DisplayMode displayMode) const;
		void SetViewPorts(const std::vector<ViewPort> & viewports);
		void SetScissorRectangle(const std::vector<Rect> & rects);

		void OnReSize(UINT cx, UINT cy);
	protected:
		void SetFullViewPort(int cx = -1, int cy = -1);

		IDXGIFactory2 * mFactory2 = nullptr;
		IDXGIFactory1 * mFactory1 = nullptr;

		ID3D11Device        *mD3dDevice = nullptr;
		ID3D11DeviceContext *mDeviceContext = nullptr;

		IDXGISwapChain1 * mSwapChain1 = nullptr;
		IDXGISwapChain * mSwapChain = nullptr;
		D3D_FEATURE_LEVEL mFeatureLevel;

		UINT m4xMsaaQuality = 0;
		bool mEnable4xMsaa = false;
		bool mIsStandby = false;

		HWND mHwnd;

		HWND mTopHwnd;

		UINT mWidth;
		UINT mHeight;
		HINSTANCE mInstance;
		CacheState<SamplerState, RawSamplerState> mSamplerStates;
		CacheState<RasterizerState, RawRasterizerState> mRasterizerStates;
		CacheState<DepthStencilState, RawDepthStencilState> mDepthStencilStates;
		CacheState<BlendState, RawBlendState> mBlendStates;

		FrameBuffer mDisplayFrameBuffer;
		std::vector<ViewPort> mViewports;
		std::vector<Rect> mScissorRects;

		DisplayMode mDisplayMode;
		std::vector<std::pair<int, DXGI_MODE_DESC *>> mScreenMode;

		std::vector<IDXGIOutput *> mOutputs;

		std::vector<IDXGIAdapter1 *> mAdapters;
		void SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput);
		DXGI_FORMAT mDisplayFormat;
		RECT mLastWinRc;
		DisplayMode mLastMode;

		SIZE mFullScreenSize;
		//DXGI_ADAPTER_DESC * mAdapters;
	private:
#ifdef _DEBUG
		void QueryDebugInterface();
#endif
		
	};
#ifdef _DEBUG
#define PutDebugString(de) de->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(XGF_FUNCTIONNAME), XGF_FUNCTIONNAME);
#else
#define PutDebugString(de) ;
#endif
}

