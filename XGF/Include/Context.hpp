#pragma once
#include <unordered_map>

namespace XGF
{
	class GDI;
	class Asyn;
	class ShaderManager;
	class XGFramework;
	class Context
	{
	static int ContextTLSSlot;
	static bool HasInit;
	public:
		
		~Context();

		GDI& QueryGraphicsDeviceInterface() const;

		ShaderManager& QueryShaderManager() const;

		Asyn& QueryGameThread() const;

		XGFramework& QueryFramework() const;
		static Context& Current();
		static Context & MakeContext(GDI* gdi, XGFramework * framework, Asyn * gameThread, ShaderManager* shaderManager);
		static void ClearContext(Context & context);
		static void JoinContext(Context & mainContext);
		static void DetachContext();
		static void Initialize();
		static void Shutdown();
		Context(const Context & context) = delete;
		Context & operator =(const Context & context) = delete;
		Context & operator = (Context && context) = delete;
	private:
		Context(GDI* gdi, XGFramework* framework, Asyn* gameThread, ShaderManager* shaderManager);
		Context() = default;
		GDI * mGDI;
		XGFramework * mFramework;
		ShaderManager * mShaderManager;
		Asyn * mGameThread;
		void * mLPVOID;
		bool mIsJoin;

	};
}
