#include "./../../Include/Context.hpp"
#include <Windows.h>
#include "../../Include/Logger.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Asyn.hpp"

#ifdef new
#undef  new
#endif
namespace XGF
{
	int Context::ContextTLSSlot = 0;
	bool Context::HasInit = false;


	Context::~Context()
	{
	}

	GDI& Context::QueryGraphicsDeviceInterface() const
	{
		return *mGDI;
	}

	ShaderManager& Context::QueryShaderManager() const
	{
		return *mShaderManager;
	}

	Asyn& Context::QueryGameThread() const
	{
		return *mGameThread;
	}

	Asyn & Context::QueryRenderThread() const
	{
		return *mRenderThread;
	}

	XGFramework& Context::QueryFramework() const
	{
		return *mFramework;
	}

	Renderer& Context::QueryRenderer() const
	{
		return *mRenderer;
	}

	FrameMemoryAllocator& Context::QueryGameThreadFrameAllocator() const
	{
		return mRenderer->GetAllocator();
	}

	FrameMemoryAllocator& Context::QueryRenderThreadFrameAllocator() const
	{
		return mRenderer->GetRenderAllocator();
	}

	FrameMemoryAllocator& Context::QueryFrameAllocator() const
	{
		if (IsCurrentRenderThread())
		{
			return QueryRenderThreadFrameAllocator();
		}
		return QueryGameThreadFrameAllocator();
	}

	ContextData& Context::QueryContextData() const
	{
		return *mContextData;
	}

	bool Context::IsCurrentRenderThread() const
	{
		return mContextData->IsRenderThread();
	}

	void Context::TagToRenderThread()
	{
		mContextData->SetIsRenderThread(true);
	}

	Context& Context::Current()
	{
		return *(Context *)TlsGetValue(ContextTLSSlot);
	}

	Context& Context::MakeContext(GDI* gdi, XGFramework* framework, Asyn* gameThread, Asyn* renderThread,
		Renderer* renderer, ShaderManager* shaderManager)
	{
		void * context = (void *)LocalAlloc(LPTR, sizeof(Context) + sizeof(int));

		auto realContext = new(context) Context(gdi, framework, gameThread, renderThread, renderer, shaderManager);

		realContext->mLPVOID = context;
		realContext->mIsJoin = false;
		TlsSetValue(ContextTLSSlot, realContext);

		void * contextData = (void*)LocalAlloc(LPTR, sizeof(ContextData) + sizeof(int));
		realContext->mContextData = new (contextData) ContextData();
		realContext->mRawContextData = contextData;

		return *realContext;
	}

	void Context::ClearContext(Context & context)
	{
		void * p = context.mLPVOID;
		void* data = context.mRawContextData;
		context.~Context();
		LocalFree(data);
		LocalFree(p);
	}
	void Context::JoinContext(Context & mainContext)
	{
		void * context = (void *)LocalAlloc(LPTR, sizeof(Context) + sizeof(int));

		auto realContext = new(context) Context(mainContext.mGDI, mainContext.mFramework, mainContext.mGameThread, mainContext.mRenderThread, mainContext.mRenderer, mainContext.mShaderManager);

		realContext->mIsJoin = true;
		realContext->mLPVOID = context;
		TlsSetValue(ContextTLSSlot, realContext);

		void* contextData = (void*)LocalAlloc(LPTR, sizeof(ContextData) + sizeof(int));
		realContext->mContextData = new (contextData) ContextData();
		realContext->mRawContextData = contextData;
	}
	void Context::DetachContext()
	{
		auto & context = Context::Current();
		if(context.mIsJoin)
			ClearContext(context);
	}

	void Context::Initialize()
	{
		if (HasInit) return;
		ContextTLSSlot = TlsAlloc();
		HasInit = true;
	}

	void Context::Shutdown()
	{
		if (!HasInit) return;
		TlsFree(ContextTLSSlot);
		HasInit = false;
		ContextTLSSlot = 0;
	}

	Context::Context(GDI* gdi, XGFramework* framework, Asyn* gameThread, Asyn* renderThread, Renderer* renderer,
		ShaderManager* shaderManager) : mGDI(gdi), mGameThread(gameThread), mShaderManager(shaderManager), 
										mFramework(framework), mRenderThread(renderThread), mRenderer(renderer)

	{
	}
	bool ContextData::IsRenderThread()
	{
		return mIsRenderThread;
	}
	void ContextData::SetIsRenderThread(bool isRenderThread)
	{
		mIsRenderThread = isRenderThread;
	}
	ContextData::ContextData(): mExtraInfo(nullptr), mTag(0), mIsRenderThread(false)
	{
	}
	void* ContextData::GetExtraInfo() const
	{
		return mExtraInfo;
	}
	void ContextData::SetExtraInfo(void* info)
	{
		mExtraInfo = info;
	}
}
