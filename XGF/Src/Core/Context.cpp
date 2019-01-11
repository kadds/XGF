#include "./../../Include/Context.hpp"
#include <Windows.h>
#include "../../Include/Logger.hpp"
#ifdef new
#undef  new
#endif
namespace XGF
{
	int Context::ContextTLSSlot = 0;
	bool Context::HasInit = false;

	Context::Context(GDI* gdi, XGFramework * framework, Asyn * gameThread, ShaderManager* shaderManager)
		: mGDI(gdi), mGameThread(gameThread), mShaderManager(shaderManager), mFramework(framework)
	{
		
	}

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

	XGFramework& Context::QueryFramework() const
	{
		return *mFramework;
	}

	Context& Context::Current()
	{
		return *(Context *)TlsGetValue(ContextTLSSlot);
	}

	Context & Context::MakeContext(GDI* gdi, XGFramework* framework, Asyn* gameThread, ShaderManager* shaderManager)
	{
		void * context =(void *) LocalAlloc(LPTR, sizeof(Context) + sizeof(int));

		auto realContext = new(context) Context(gdi, framework, gameThread, shaderManager);

		realContext->mLPVOID = context;
		realContext->mIsJoin = false;
		TlsSetValue(ContextTLSSlot, realContext);
		return *realContext;
	}
	void Context::ClearContext(Context & context)
	{
		void * p = context.mLPVOID;
		context.~Context();
		LocalFree(p);
	}
	void Context::JoinContext(Context & mainContext)
	{
		void * context = (void *)LocalAlloc(LPTR, sizeof(Context) + sizeof(int));

		auto realContext = new(context) Context(mainContext.mGDI, mainContext.mFramework, mainContext.mGameThread, mainContext.mShaderManager);

		realContext->mIsJoin = true;
		realContext->mLPVOID = context;
		TlsSetValue(ContextTLSSlot, realContext);
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
}
