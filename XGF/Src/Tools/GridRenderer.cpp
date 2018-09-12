#include "..\..\Include\GridRenderer.hpp"

#include "../../Include/ConstantData.hpp"
#include "../../Include/ShaderConst.hpp"
namespace XGF
{
	GridRenderer::GridRenderer()
	{
	}


	GridRenderer::~GridRenderer()
	{
	}

	void GridRenderer::Initialize(GDI * gdi, float width, float height, unsigned xcount, unsigned zcount, const Point & origin)
	{
		XGF_ASSERT(width > 0 && height > 0);
		mBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), (xcount * 2 + zcount * 2) + 4, xcount * 2 + zcount * 2 + 4, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mHeight = height;
		mWidth = width;
		mPolygon = std::make_shared<PolygonPlePoint3>(xcount * 2 + zcount * 2 + 4);
		mPolygonPleIndex = std::make_shared<PolygonPleIndex> (xcount * 2 + zcount * 2 + 4);
		int c[2] = { (int)xcount * 2 + 2, (int)zcount * 2 + 2 };
		mColorBinder = std::make_shared<PolygonPleConstantExColorBinder>(c, 2);
		mColorBinder->SetLayerColor(0, SM::Color(1.0f, 0.2f, 0.2f, 1.0f));
		mColorBinder->SetLayerColor(1, SM::Color(0.2f, 1.0f, 0.2f, 1.0f));
		mBindingBridge.AddBinder(mPolygon);
		mBindingBridge.AddBinder(mColorBinder);
		Point pstart = origin;
		pstart.x = 0 - mWidth * xcount / 2.0f;//x
		pstart.z = 0 - mHeight * (zcount - 1.f);//z
		for (unsigned int i = 0; i < xcount * 2 + 2; i += 2)
		{
			mPolygon->mPoint[i].x = pstart.x;
			mPolygon->mPoint[i].y = origin.y;
			mPolygon->mPoint[i].z = pstart.z;

			mPolygon->mPoint[i + 1].x = pstart.x;
			mPolygon->mPoint[i + 1].y = origin.y;
			mPolygon->mPoint[i + 1].z = -pstart.z;
			mPolygonPleIndex->mIndex[i] = i;
			mPolygonPleIndex->mIndex[i + 1] = i + 1;
			pstart.x += mWidth;
		}
		pstart.x = 0 - mWidth * (xcount - 1.f);//x
		pstart.z = 0 - mHeight * zcount / 2.0f;//z
		for (unsigned int j = xcount * 2 + 2; j < xcount * 2 + zcount * 2 + 4; j += 2)
		{
			mPolygon->mPoint[j].x = pstart.x;
			mPolygon->mPoint[j].y = origin.y;
			mPolygon->mPoint[j].z = pstart.z;

			mPolygon->mPoint[j + 1].x = -pstart.x;
			mPolygon->mPoint[j + 1].y = origin.y;
			mPolygon->mPoint[j + 1].z = pstart.z;
			mPolygonPleIndex->mIndex[j] = j;
			mPolygonPleIndex->mIndex[j + 1] = j + 1;
			pstart.z += mWidth;
		}

		//mMeshData = new MeshData(max);

	}

	void GridRenderer::Shutdown()
	{
		mBatch.Shutdown();
		//delete mMeshData;
	}

	void GridRenderer::Begin(const WVPMatrix & matrix)
	{
		mBatch.GetShaderStage()->SetVSConstantBuffer(0, &matrix);
		mBatch.Begin();
	}

	void GridRenderer::End()
	{
		mBatch.End();
	}

	void GridRenderer::DrawGrid(Point & center)
	{
		mBatch.DrawPolygon(mPolygonPleIndex, mBindingBridge);
	}

	void GridRenderer::SetColor(SM::Color & cx, SM::Color & cz)
	{
		mColorBinder->SetLayerColor(0, cx);
		mColorBinder->SetLayerColor(1, cz);
	}

	void GridRenderer::SetXColor(SM::Color & cx)
	{
		mColorBinder->SetLayerColor(0, cx);
	}

	void GridRenderer::SetZColor(SM::Color & cz)
	{
		mColorBinder->SetLayerColor(1, cz);
	}


	MeshData::MeshData(unsigned max) :Shape::Shape((max + 1)*(max + 1), max * max * 2 * 3), maxLen(max)
	{

	}

	MeshData::~MeshData()
	{
	}

	void MeshData::SetMeshPosition(Position center, unsigned width, unsigned height, float baseY)
	{
		//no finish
		Position pstart;
		pstart.x = center.x - width * maxLen / 2.0f;//x
		pstart.y = center.y - height * maxLen / 2.0f;//z
		for (unsigned int i = 0; i < maxLen; i++)
		{
			for (unsigned int j = 0; j < maxLen; j++)
			{
				mPolygon->mPoint[i* (maxLen + 1) + j] = Point(i* (maxLen + 1)* width + pstart.x
					, baseY, j * height + pstart.y);

			}
		}
	}

}
