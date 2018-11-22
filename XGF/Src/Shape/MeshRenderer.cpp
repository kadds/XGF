#include "..\..\Include\MeshRenderer.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Mesh.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Texture.hpp"
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
				mRendererGroup.push_back(std::make_pair<>(vec, batch));
			}
			else
			{
				ele->first.push_back(mesh);
			}
		}

		void MeshRenderer::Remove(Mesh * mesh)
		{
			auto ele = std::find_if(mRendererGroup.begin(), mRendererGroup.end(), [mesh](const std::pair<std::vector<Mesh *>, Batch *> & data)
			{
				return data.second->GetShaderStage()->EqualsWithShaders(mesh->GetMaterial()->GetShaders());
			});
			if (ele != mRendererGroup.end())
			{
				auto et = std::find(ele->first.begin(), ele->first.end(), mesh);
				ele->first.erase(et);
			}
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
					it.second->GetShaderStage()->SetRasterizerState(element->GetMaterial()->GetRasterizerState());
					it.second->GetShaderStage()->SetDepthStencilState(element->GetMaterial()->GetDepthStencilState());
					it.second->GetShaderStage()->SetBlendState(element->GetMaterial()->GetBlendState());

					auto r = element->GetTextureList();
					for(unsigned i = 0; i < r.size(); i++)
					{
						it.second->GetShaderStage()->SetPSSRV(i, r[i]->GetRawTexture());
					}
					auto cb = element->GetMaterial()->GetConstantBuffers();
					for(int i = 0; i < cb.size(); i++)
					{
						it.second->GetShaderStage()->SetPSConstantBuffer(i, cb[i]);
					}
					BindingBridge bbr = element->GetBindingBridge();
					auto ppe = std::make_shared<PolygonPlePoint3>(bbr.GetBinder(0)->mCount);
					auto matirix = element->GetGeometry()->GetTransform().GetMatrix();
					element->GetGeometry()->mPolygon->MulTo(ppe, matirix);
					bbr.SetBinder(ppe, 0);
					it.second->DrawPolygon(element->GetGeometry()->mPolygonPleIndex, bbr);
				}
			}
		}
	}
}

