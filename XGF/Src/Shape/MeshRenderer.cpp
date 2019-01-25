#include "..\..\Include\MeshRenderer.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Mesh.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Texture.hpp"
#include <algorithm>
#include "../../Include/Material/LightMaterial.hpp"
#include "../../Include/Light/Light.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	using XGF::Batch;
	using XGF::BindingBridge;
	namespace Shape
	{
		void MeshRenderer::Initialize()
		{
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
			if(mesh->GetMaterial()->GetShaders().IsNullShaders())
			{
				mNullRendererGroup.push_back(mesh);
				return;
			}
			auto ele = std::find_if(mRendererGroup.begin(), mRendererGroup.end(), [mesh](const std::pair<std::vector<Mesh *>, ShaderStage *> & data)
			{
				return data.second->EqualsWithShaders(mesh->GetMaterial()->GetShaders());
			});
			if (ele == mRendererGroup.end())
			{
				std::vector<Mesh *> vec;
				vec.push_back(mesh);
				ShaderStage * stage = new ShaderStage();
				stage->Initialize(mesh->GetMaterial()->GetShaders());
				mRendererGroup.push_back(std::make_pair<>(vec, stage));
			}
			else
			{
				ele->first.push_back(mesh);
			}
			mesh->GetMaterial()->RemoveChangeShadersFlag();
		}

		void MeshRenderer::Remove(Mesh * mesh)
		{
			if (mesh->GetMaterial()->GetShaders().IsNullShaders())
			{
				auto it = std::find(mNullRendererGroup.begin(), mNullRendererGroup.end(), mesh);
				if(it != mNullRendererGroup.end())
				{
					mNullRendererGroup.erase(it);
				}
				return;
			}
			auto ele = std::find_if(mRendererGroup.begin(), mRendererGroup.end(), [mesh](const std::pair<std::vector<Mesh *>, ShaderStage *> & data)
			{
				return data.second->EqualsWithShaders(mesh->GetMaterial()->GetShaders());
			});
			if (ele != mRendererGroup.end())
			{
				auto et = std::find(ele->first.begin(), ele->first.end(), mesh);
				if(et != ele->first.end())
					ele->first.erase(et);
				if (ele->first.empty())
				{
					ele->second->Shutdown();
					delete ele->second;
					mRendererGroup.erase(ele);
				}
			}
			else // 不在 RendererGroup 中
			{
				for (auto it = mRendererGroup.begin(); it != mRendererGroup.end(); ++it)
				{
					for (auto ot = it->first.begin(); ot != it->first.end(); ++ot)
					{
						if (*ot == mesh)
						{
							it->first.erase(ot);
							goto over;
						}
					}
					continue;
					over:
					if(it->first.empty())
					{
						it->second->Shutdown();
						delete it->second;
						mRendererGroup.erase(it);
					}
					return;
				}
				auto it = std::find(mNullRendererGroup.begin(), mNullRendererGroup.end(), mesh);
				if (it != mNullRendererGroup.end())
				{
					mNullRendererGroup.erase(it);
				}
			}
		}

		void MeshRenderer::Add(Light * light)
		{
			mLights.push_back(light);
		}

		void MeshRenderer::Remove(Light * light)
		{
			auto it = std::find(mLights.begin(), mLights.end(), light);
			if(it != mLights.end())
			{
				mLights.erase(it);
			}
		}

		void MeshRenderer::Begin(const WVPMatrix & wvp)
		{
			std::vector<Mesh *> moveElements;
			for (auto & it : mRendererGroup)
			{
				for(auto & ot : it.first)
				{
					if(ot->GetMaterial()->IsChangeShaders())
						moveElements.push_back(ot);
				}
			}
			for(auto it : mNullRendererGroup)
			{
				if (it->GetMaterial()->IsChangeShaders())
					moveElements.push_back(it);
			}
			for(auto & it : moveElements)
			{
				Remove(it);
				Add(it);
				it->GenerateBindingBridge(it->GetMaterial()->GetShaders().vs->HasSemanticNormal(),
					it->GetMaterial()->GetShaders().vs->HasSemanticTexcoord(), false);
				it->GetMaterial()->RemoveChangeShadersFlag();
			}
			for (auto & it : mRendererGroup)
			{
				if (it.second == nullptr)
					continue;
				it.second->SetVSConstantBuffer(0, &wvp);
			}
		}

		void MeshRenderer::End()
		{
		}

		void MeshRenderer::RefreshLightMesh()
		{
			for (auto & it : mRendererGroup)
			{
				for(auto ot : it.first)
				{
					if(ot->GetMaterial()->CanLinkWithLight())
					{
						CastLight(ot);
					}
				}
			}
			for(auto it : mNullRendererGroup)
			{
				if (it->GetMaterial()->CanLinkWithLight())
				{
					CastLight(it);
				}
			}
		}

		void MeshRenderer::CastLight(Mesh* mesh)
		{
			LightMaterial* material = static_cast<LightMaterial *>(mesh->GetMaterial());
			material->BeginSetLights();
			int group = material->GetLightGroup();
			for(auto it : mLights)
			{
				if(it->GetGroup() & group)
					material->AddLight(it);
			}
			material->EndSetLights();
			material->CompileShader();
		}

		void MeshRenderer::Draw(const Point & cameraPosition)
		{
			for (auto & it : mRendererGroup)
			{
				for (auto & element : it.first)
				{
					auto * stage = it.second;
					stage->SetRasterizerState(element->GetMaterial()->GetRasterizerState());
					stage->SetDepthStencilState(element->GetMaterial()->GetDepthStencilState());
					stage->SetBlendState(element->GetMaterial()->GetBlendState());

					auto r = element->GetMaterial()->GetPSBindingTextures();
					for(unsigned i = 0; i < r.size(); i++)
					{
						stage->SetPSTexture(i, r[i]);
					}
					r = element->GetMaterial()->GetVSBindingTextures();
					for (unsigned i = 0; i < r.size(); i++)
					{
						stage->SetVSTexture(i, r[i]);
					}
					auto cb = element->GetMaterial()->GetVSBindingConstantBuffers();
					for(int i = 0; i < cb.size(); i++)
					{
						stage->SetVSConstantBuffer(i + 1, cb[i]);
					}
					cb = element->GetMaterial()->GetPSBindingConstantBuffers();
					int i = 0;
					for (; i < cb.size(); i++)
					{
						stage->SetPSConstantBuffer(i, cb[i]);
					}
					if(element->GetMaterial()->CanLinkWithLight())
					{
						bool usePosition = ((LightMaterial *)(element->GetMaterial()))->IsUseViewPosition();
						
						auto cpFunc = [element](void * des, int size)
						{
							char * dest = (char *)des;
							auto lights = ((LightMaterial *)(element->GetMaterial()))->GetLights();
							Color ambient = Color(0, 0, 0, 1);
							for(auto it : lights[0])
							{
								ambient += it->GetLightColor();
							}
							*((Color *)dest) = ambient;
							dest += sizeof(Color);
							// 索引从1开始 因为 索引0 环境光 已经计算
							for (int i = 1; i < (int)LightType::InvalidValue; i++)
							{
								for (auto it : lights[i])
								{
									dest += it->CopyConstantBuffer(dest);
								}
							}
							if(size != static_cast<int>(dest - static_cast<char *>(des)))
							{
								XGF_Warn(Render, "Write content is larger than buffer size, content:", static_cast<int>(dest - static_cast<char *>(des)), ", buffer size:", size);
							}
						};
						if(usePosition)
							stage->SetPSConstantBuffer(i++, &cameraPosition);
						//if(((LightMaterial *)(element->GetMaterial()))->ShouldWriteBufferLight())
						stage->SetPSConstantBuffer(i, cpFunc);
					}
					
					BindingBridge bbr = element->GetBindingBridge();
					auto ppe = std::make_shared<PolygonPlePointBinder>(bbr.GetBinder(0)->GetActualCount());
					auto matrix = element->GetGeometry()->GetTransform().GetMatrix();
					element->GetGeometry()->mPolygon->ExpandAllTo(*ppe.get(), Operator::Multiply(matrix));
					bbr.SetBinder(ppe, 0);
					Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *element->GetGeometry()->mPolygonPleIndex.get(), *stage));
				}
			}
		}
	}
}

