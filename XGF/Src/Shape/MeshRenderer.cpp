#include "..\..\Include\MeshRenderer.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Mesh.hpp"
#include "../../Include/Batch.hpp"
#include <algorithm>
namespace XGF
{
	using XGF::Batch;
	using XGF::GDI;
	using XGF::BindingBridge;
	namespace Shape
	{
		void MeshRenderer::Initialize(GDI * gdi)
		{
			mGDI = gdi;
		}
		void MeshRenderer::Shutdown()
		{
			for (auto & it : mRendererGroup)
			{
				it.second->Shutdown();
				delete it.second;
			}
		}
		MeshRenderer::MeshRenderer()
		{
		}


		MeshRenderer::~MeshRenderer()
		{
		}

		void MeshRenderer::Add(Mesh * mesh)
		{
			auto ele = std::find_if(mRendererGroup.begin(), mRendererGroup.end(), [mesh](const std::pair<std::vector<Mesh *>, Batch *> & data)
			{
				return data.second->GetShaderStage()->EqualsWithShaders(mesh->GetMaterial()->GetShaders());
			});
			if (ele == mRendererGroup.end())
			{
				vector<Mesh *> vec;
				vec.push_back(mesh);
				Batch * batch = new Batch();
				batch->Initialize(mGDI, mesh->GetMaterial()->GetShaders(), 2048, 2048);
				mRendererGroup.push_back(std::make_pair(vec, batch));
			}
			else
			{
				ele->first.push_back(mesh);
			}
		}

		void MeshRenderer::Remove(Mesh * mesh)
		{
			//TDOO: Remove from array
		}

		void MeshRenderer::Begin(const WVPMatrix & wvp)
		{
			for (auto & it : mRendererGroup)
			{
				it.second->GetShaderStage()->SetVSConstantBuffer(0, &wvp);
				it.second->Begin();
			}
		}

		void MeshRenderer::End()
		{
			for (auto & it : mRendererGroup)
			{
				it.second->End();
			}
		}
		void MeshRenderer::Draw()
		{
			
			for (auto & it : mRendererGroup)
			{
				for (auto & element : it.first)
				{
					//TODO: insert more
					it.second->GetShaderStage()->SetRasterizerState(element->GetMaterial()->GetRasterizerState());
					it.second->DrawPolygon(element->GetGeometry()->mPolygonPleIndex, element->GetBindingBridge());
				}
			}
		}
	}
}

