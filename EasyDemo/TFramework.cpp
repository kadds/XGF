#include "TFramework.h"
#include <iomanip>


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
	mFont.Initialize(GetGDI(), buffer, 17);
	mTextRenderer.Initialize(GetGDI(), &mFont, 100);

	InputType inputtype[2]{SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_TEXTURE};
	mTextureShader.Initialize(GetGDI(), ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, inputtype, 2);
	mTextureBatch.Initialize(GetGDI(), &mTextureShader, 200, 400);
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
	mRc.SetPositionAndSize(0, ClientY * 2 / 3, ClientX / 3, ClientY / 3);
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
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);
	//mGDI->Clear(Color(0.2f, 0.2f, 0.2f, 1.f));
	mShapeRenderer.Begin(wvp);
	mTextRenderer.Begin(wvp);
	//RTT start
	//mRenderToTexture.SetRenderTarget();
	//mRenderToTexture.Clear(Color(0.f,0.f,0.f,1.f));
	
	//MyRender(wvp, false);

	//RTT end
	//mRenderToTexture.SetDefaultRenderTarget();

	MyRender(wvp, true);
	
	mShapeRenderer.End();
	

	PolygonPleTextureBinder textureBinder(4);
	textureBinder.SetPosition(0.f, 1.f, 0.f, 1.f);
	BindingBridge bb;
	bb.AddBinder(textureBinder);

	std::wstringstream str;
	auto debug = DebugInscriber::GetInstance();
	str.clear();
	str.str(L"");
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() << "\n" << L"FC:" << std::setprecision(4) << debug->GetFrameCost() << "ms";
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	mTextRenderer.End();
	//mTextureBatch.Begin(wvp);
	//mRc.Render(mTextureBatch, nullptr, bb, mRenderToTexture.GetShaderResourceView());
	//mTextureBatch.End();
	/*
	auto c = mRenderToTexture.GetShaderResourceView();
	ID3D11Resource *res;
	c->GetResource(&res);
	DirectX::SaveDDSTextureToFile(GetGDI()->GetDeviceContext(),res ,L"scene.dds");
	Exit(0);
	*/
	mGDI->Present(false);
}
void TFramework::MyRender(WVPMatrix & wvp, bool end)
{
	mShapeRenderer.DrawCircle(240, 240, 40, GetPrecision(40, 5), Color(1.f, 1.f, 0.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mShapeRenderer.DrawRectangleB(50, 50, 100, 100, Color(1.f, 0.f, 0.f, 1.f), 8.f, Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mShapeRenderer.DrawLine(0, 0, 150, 150, Color(1.f, 1.f, 1.f, 1.f));
	mTextRenderer.DrawString(L"A RenderToTexture(RTT) Demo", 35, 35);
	if (!end)
	{
		mShapeRenderer.Flush();
		mTextRenderer.Flush();
	}
}
void TFramework::Update(float time)
{
}
