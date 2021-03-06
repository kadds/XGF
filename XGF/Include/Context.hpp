#pragma once
namespace XGF
{
	class GDI;
	class Asyn;
	class ShaderManager;
	class XGFramework;
	class Renderer;
	class FrameMemoryAllocator;
	class Context;
	class ContextData
	{
	private:
		friend class Context;
		bool mIsRenderThread;
		int mTag;
		void* mExtraInfo;
		bool IsRenderThread();
		void SetIsRenderThread(bool isRenderThread);
	public:
		ContextData();
		void* GetExtraInfo() const;
		void SetExtraInfo(void * info);
	};
	class Context
	{
	static int ContextTLSSlot;
	static bool HasInit;
	public:
		
		~Context();

		GDI& QueryGraphicsDeviceInterface() const;
		ShaderManager& QueryShaderManager() const;

		Asyn& QueryGameThread() const;
		Asyn& QueryRenderThread() const;
		XGFramework& QueryFramework() const;
		Renderer& QueryRenderer() const;
		FrameMemoryAllocator& QueryGameThreadFrameAllocator() const;
		FrameMemoryAllocator& QueryRenderThreadFrameAllocator() const;
		FrameMemoryAllocator& QueryFrameAllocator() const;
		ContextData& QueryContextData() const;
		bool IsCurrentRenderThread() const;
		void TagToRenderThread();
		static Context& Current();
		static Context & MakeContext(GDI* gdi, XGFramework * framework, Asyn * gameThread, Asyn * renderThread, Renderer * renderer, ShaderManager* shaderManager);
		static void ClearContext(Context & context);
		static void JoinContext(Context & mainContext);
		static void DetachContext();
		static void Initialize();
		static void Shutdown();
		Context(const Context & context) = delete;
		Context & operator =(const Context & context) = delete;
		Context & operator = (Context && context) = delete;
	private:
		Context(GDI* gdi, XGFramework* framework, Asyn* gameThread, Asyn * renderThread, Renderer * renderer, ShaderManager* shaderManager);
		Context() = default;
		GDI * mGDI;
		XGFramework * mFramework;
		ShaderManager * mShaderManager;
		Renderer * mRenderer;
		Asyn * mGameThread;
		Asyn * mRenderThread;
		void * mLPVOID;
		bool mIsJoin;

		ContextData* mContextData;
		void* mRawContextData;
	};
}
