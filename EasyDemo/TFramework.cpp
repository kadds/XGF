#include "TFramework.h"



TFramework::TFramework()
{
}


TFramework::~TFramework()
{
}

void TFramework::OnCreate()
{
	mShapeRenderer.Initialize(GetGDI());
	char buffer[MAX_PATH];
	Tools::GetInstance()->GetFontPath("Dengb.ttf", buffer, MAX_PATH);
	mFont.Initialize(GetGDI(), buffer, 35);
	mTextRenderer.Initialize(GetGDI(), &mFont, 100);
	InputType inputtype[2]{SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_TEXTURE};
	mTextureShader.Initialize(GetGDI(), ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, inputtype, 2);
	mTextureBatch.Initialize(GetGDI(), &mTextureShader, 200, 400, RealTime, false);
	mRenderToTexture.Initialize(GetGDI(), GetGDI()->GetWidth(), GetGDI()->GetHeight());
	
	mRc.SetPositionAndSize(0, GetGDI()->GetHeight() - 100, 130, 100);
	mRc.SetZ(0.0f);

}

void TFramework::OnDestory()
{
	mRenderToTexture.Shutdown();
	mTextureBatch.Shutdown();
	mTextureShader.Shutdown();
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mShapeRenderer.Shutdown();
}

void TFramework::OnSize(int ClientX, int ClientY)
{
	mCamera.UpdataSize(ClientX, ClientY);
	mRenderToTexture.Shutdown();
	mRenderToTexture.Initialize(GetGDI(), GetGDI()->GetWidth(), GetGDI()->GetHeight());
	mRc.SetPositionAndSize(0, GetGDI()->GetHeight() - 100, 130, 100);
}

void TFramework::OnMessage(const Event & ev)
{
}

void TFramework::OnActivate(bool isActivate)
{
}

bool TFramework::OnClose()
{
	return true;
}

void TFramework::Render()
{
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);
	mGDI->Clean(Color(0.2f, 0.2f, 0.2f, 1.f));

	mTextRenderer.Begin(wvp);
	mShapeRenderer.Begin(wvp);
	//RTT start
	mRenderToTexture.SetRenderTarget();
	mRenderToTexture.Clean(Color(0.f,0.f,0.f,1.f));
	
	MyRender(wvp, false);

	//RTT end
	mRenderToTexture.SetDefaultRenderTarget();
	MyRender(wvp, true);
	
	

	PolygonPleTextureBinder textureBinder(4);
	textureBinder.SetPosition(0.f, 1.f, 0.f, 1.f);
	BindingBridge bb;
	bb.AddBinder(textureBinder);
	mTextRenderer.End();
	mTextureBatch.Begin(wvp);
	mRc.Render(mTextureBatch, nullptr, bb, mRenderToTexture.GetShaderResourceView());
	mTextureBatch.End();
	mShapeRenderer.End();
	//auto c = mRenderToTexture.GetShaderResourceView();
	//ID3D11Resource *res;
	//c->GetResource(&res);
	//DirectX::SaveDDSTextureToFile(GetGDI()->GetDeviceContext(),res ,L"scene.dds");
	//Exit(0);
	mGDI->Present(false);
}
void TFramework::MyRender(WVPMatrix & wvp, bool end)
{
	mShapeRenderer.DrawCircle(240, 240, 40, GetPrecision(40, 5), Color(1.f, 1.f, 0.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mShapeRenderer.DrawRectangleB(50, 50, 100, 100, Color(1.f, 0.f, 0.f, 1.f), 8.f, Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mTextRenderer.DrawString(L"It is made by RenderToTexture", 35, 35);
	mFPS.RenderFPS(mTextRenderer, Color(1.f, 0.f, 0.f, 1.f), 3, 4);
	if (!end)
	{
		mTextRenderer.Flush();
		mShapeRenderer.Flush();
	}
}
void TFramework::Update(float time)
{
	mFPS.Tick(time);
}
