#include "..\..\Include\GridRenderer.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/SystemShaders.hpp"

namespace XGF
{
	GridRenderer::GridRenderer()
	{
	}


	GridRenderer::~GridRenderer()
	{
	}

	void GridRenderer::Initialize(float width, float height, unsigned xcount, unsigned zcount, const Point & origin)
	{
		XGF_ASSERT(width > 0 && height > 0);
		mRenderResource.ReCreate(SystemShaders::GetBasicShaders(SystemShaders::BasicShader_VertexColor));
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mRenderState.GetDepthStencilState().SetDepthEnable(false);
		mHeight = height;
		mWidth = width;
		mPolygon = std::make_shared<PolygonPlePointBinder>(xcount * 2 + zcount * 2 + 4);
		mPolygonPleIndex = std::make_shared<PolygonPleIndex> (xcount * 2 + zcount * 2 + 4);
		int c[2] = { (int)xcount * 2 + 2, (int)zcount * 2 + 2 };
		std::vector<std::pair<int, Color>> vec =
		{
			std::make_pair(0, Color(1.0f, 0.2f, 0.2f, 1.0f)),
			std::make_pair(xcount * 2  + 2, Color(0.2f, 1.0f, 0.2f, 1.0f))
		};
		mColorBinder = std::make_shared<PolygonPleConstantColorBinder>(mPolygon->GetActualCount(), vec);
		mBindingBridge.AddBinder(mPolygon);
		mBindingBridge.AddBinder(mColorBinder);
		Point pstart = origin;
		pstart.x = 0 - mWidth * xcount / 2.0f;//x
		pstart.z = 0 - mHeight * (zcount - 1.f);//z
		auto data = mPolygon->GetData();
		for (unsigned int i = 0; i < xcount * 2 + 2; i += 2)
		{
			data[i].x = pstart.x;
			data[i].y = origin.y;
			data[i].z = pstart.z;

			data[i + 1].x = pstart.x;
			data[i + 1].y = origin.y;
			data[i + 1].z = -pstart.z;
			mPolygonPleIndex->Get(i) = i;
			mPolygonPleIndex->Get(i + 1) = i + 1;
			pstart.x += mWidth;
		}
		pstart.x = 0 - mWidth * (xcount - 1.f);//x
		pstart.z = 0 - mHeight * zcount / 2.0f;//z
		for (unsigned int j = xcount * 2 + 2; j < xcount * 2 + zcount * 2 + 4; j += 2)
		{
			data[j].x = pstart.x;
			data[j].y = origin.y;
			data[j].z = pstart.z;

			data[j + 1].x = -pstart.x;
			data[j + 1].y = origin.y;
			data[j + 1].z = pstart.z;
			mPolygonPleIndex->Get(j) = j;
			mPolygonPleIndex->Get(j + 1) = j + 1;
			pstart.z += mWidth;
		}

		//mMeshData = new MeshData(max);

	}

	void GridRenderer::Shutdown()
	{
		mRenderResource.Clear();
		//delete mMeshData;
	}

	void GridRenderer::Begin(const WVPMatrix & matrix)
	{
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, matrix);
	}

	void GridRenderer::End()
	{
		
	}

	void GridRenderer::DrawGrid(Point & center)
	{
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(mBindingBridge, *mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}

	void GridRenderer::SetColor(SM::Color & cx, SM::Color & cz)
	{
		mColorBinder->Set(0, cx);
		mColorBinder->Set(1, cz);
	}

	void GridRenderer::SetXColor(SM::Color & cx)
	{
		mColorBinder->Set(0, cx);
	}

	void GridRenderer::SetZColor(SM::Color & cz)
	{
		mColorBinder->Set(1, cz);
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
				mPolygon->GetData(i* (maxLen + 1) + j) = Point(i* (maxLen + 1)* width + pstart.x
					, baseY, j * height + pstart.y);

			}
		}
	}

}
