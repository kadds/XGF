#include "..\..\Include\GridRenderer.hpp"


#include "../../Include/ShaderConst.hpp"

GridRenderer::GridRenderer()
{
}


GridRenderer::~GridRenderer()
{
}

void GridRenderer::Initialize(GDI * gdi, unsigned width, unsigned height, unsigned max)
{
	auto p = DirectX::XMMatrixIdentity();
	InputType inputype[2] = { SHADER_INPUTLAYOUT_POSITION, SHADER_INPUTLAYOUT_TEXTURE };
	mShader.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, inputype, 2);
	mBatch.Initialize(gdi, &mShader, (max + 1)*(max + 1), max * max * 2 * 3, RenderMode::RealTime);
	mHeight = height;
	mWidth = width;
	mMeshData = new MeshData(max);

}

void GridRenderer::Shutdown()
{
	mBatch.Shutdown();
	mShader.Shutdown();
	delete mMeshData;
}

void GridRenderer::Begin(const WVPMatrix & matrix)
{
	mBatch.Begin(matrix);
}

void GridRenderer::End()
{
	mBatch.End();
}

void GridRenderer::DrawGrid()
{
	//mMeshData->Render(mBatch, nullptr, );
}

MeshData::MeshData(unsigned max):Shape::Shape((max+1)*(max + 1),max * max * 2 *3),maxLen(max)
{
	
}

MeshData::~MeshData()
{
}

void MeshData::SetMeshPosition(Position center, unsigned width, unsigned height, float baseY)
{
	Position pstart;
	pstart.x = center.x - width * maxLen / 2.0f;//x
	pstart.y = center.y - height * maxLen / 2.0f;//z
	for (int i = 0; i < maxLen; i++)
	{
		for (int j = 0; j < maxLen; j++)
		{
			mPolygon.mPoint[i* (maxLen + 1) + j] = Point(i* (maxLen + 1)* width + pstart.x
				, baseY,  j * height + pstart.y);
		}
	}
}
