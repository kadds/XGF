#include "..\..\Include\Mesh.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/MeshRenderer.hpp"
#include "../../Include/Material/LightMaterial.hpp"
#include "../../Include/Material/BasicMaterial.hpp"
#include "../../Include/Camera.hpp"
#include "../../Include/Material/PhongMaterial.hpp"
#include "../../Include/Material/LambertMaterial.hpp"
#include "../../Include/SystemShaders.hpp"
#include "../../Include/Logger.hpp"

namespace XGF::Shape
{
	bool MeshRenderPath::AddShaders(int key, Shaders shaders)
	{
		if (auto it = std::find_if(mShaders.begin(), mShaders.end(), [key](const std::pair<int, Shaders>& pair)
		{
			return pair.first == key;
		}); it != mShaders.end())
		{
			return false;
		}
		mShaders.emplace_back(key, shaders);
		return true;
	}

	Shaders MeshRenderPath::FindShaders(int key)
	{
		if (auto it = std::find_if(mShaders.begin(), mShaders.end(), [key](const std::pair<int, Shaders>& pair)
		{
			return pair.first == key;
		}); it != mShaders.end())
		{
			return it->second;
		}
		XGF_Warn(Shader, "Unable to find a specific shader based on Key ", key);
		return Shaders();
	}

	void MeshRenderPath::ClearShaders()
	{
		mShaders.clear();
	}
	void MeshForwardRenderPath::DrawBasicMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh)
	{
		RenderState renderState;
		BindingBridge bbr;
		auto material = mesh->GetMaterial();
		auto curMaterial = static_cast<BasicMaterial *>(material);
		auto & renderResource = cachePool.GetRenderResource(FindShaders(0));
		if (curMaterial->GetBasicTexture() != nullptr)
		{
			renderResource.SetTexture<PixelShader>(0, curMaterial->GetBasicTexture());
			// set sampler here
			auto & samplerState = renderResource.GetSamplerState<PixelShader>()[0];
			samplerState.SetFilter(Filter::MIN_MAG_MIP_LINEAR);
		}

		renderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		renderResource.SetConstantBuffer<PixelShader>(0, 0, curMaterial->GetBasicColor());

		auto vertices = std::make_shared<PolygonPlePointBinder>(mesh->GetGeometry()->mPolygon->GetActualCount());
		auto matrix = mesh->GetGeometry()->GetTransform().GetMatrix();
		mesh->GetGeometry()->mPolygon->ExpandAllTo(*vertices.get(), Operator::Multiply(matrix));
		bbr.AddBinder(vertices);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *mesh->GetGeometry()->mPolygonPleIndex.get(),
			RenderStage(renderState, renderResource)));

	}
	void MeshForwardRenderPath::DrawPhongOrLambertMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh, std::vector<Light*>& lights, Texture * textureMap, std::function<size_t(void * ptr, size_t)> structData, std::function<size_t(void * ptr)> customData)
	{
		RenderState renderState;
		BindingBridge bbr;
		auto material = static_cast<LightMaterial *>(mesh->GetMaterial());
		const auto group = material->GetLightGroup();
		int i = 0;
		Color ambient(0, 0, 0, 0);
		auto & blend = renderState.GetBlendState().GetRenderTarget(0);
		auto & depth = renderState.GetDepthStencilState();
		std::vector<RenderCommand *> commands;
		const auto matrix = mesh->GetGeometry()->GetTransform().GetMatrix();
		auto normalMatrix = XMMatrixRotationRollPitchYawFromVector(mesh->GetGeometry()->GetTransform().GetRotation());
		for (auto light : lights)
		{
			if (!(light->GetGroup() & group)) continue;
			if (light->GetLightType() == LightType::Ambient)
			{
				ambient += light->GetLightColor();
				continue;
			}
			auto castLight = static_cast<CastShadowAbleLight *>(light);
			auto & renderResource = cachePool.GetRenderResource(FindShaders(i));
			if (i == 0) // base pass
			{
				blend.SetBlendEnable(false);
				depth.SetDepthEnable(true);
				depth.SetDepthFunc(ComparisonFunc::LESS_EQUAL);
				depth.SetDepthWriteMask(true);
				renderResource.SetConstantBuffer<PixelShader>(1, [&ambient, &customData, castLight](void * ptr, size_t size)
				{
					char * p = (char *)ptr;
					*((Point4 *)p) = ambient;
					p += sizeof(Point4);
					p += customData(p);
					p += castLight->CopyConstantBuffer(p);
					
					if (size != (p - (char *)ptr))
						XGF_Warn(Render, "Size of constant buffer is incorrect");
				});
			}
			else // add pass
			{
				if(i == 1) // set at first time
				{
					blend.SetBlendEnable(true);
					blend.SetSrcBlend(Blend::ONE);
					blend.SetDestBlend(Blend::ONE);
					blend.SetDestBlendAlpha(Blend::ONE);
					blend.SetSrcBlendAlpha(Blend::ZERO);
					blend.SetBlendOpAlpha(BlendOp::ADD);
					blend.SetBlendOp(BlendOp::ADD);
					depth.SetDepthEnable(true);
					depth.SetDepthFunc(ComparisonFunc::LESS_EQUAL);
					depth.SetDepthWriteMask(false);
				}
				
				renderResource.SetConstantBuffer<PixelShader>(1, [&customData, castLight](void * ptr, size_t size)
				{
					char * p = (char *)ptr;
					p += customData(p);
					p += castLight->CopyConstantBuffer(p);

					if (size != (p - (char *)ptr))
						XGF_Warn(Render, "Size of constant buffer is incorrect");
				});
			}
			int indexOfTextureAtPixel = 0;
			if (textureMap != nullptr)
			{
				renderResource.SetTexture<PixelShader>(indexOfTextureAtPixel++, textureMap);
				// set sampler here
				auto & samplerState = renderResource.GetSamplerState<PixelShader>()[0];
				samplerState.SetFilter(Filter::MIN_MAG_MIP_LINEAR);
			}
			if (castLight->GetCastShadow() && material->GetReceiveShadow())
			{
				// has shadow
				renderResource.SetConstantBuffer<VertexShader>(0, 0, wvp, normalMatrix, castLight->GetLightMatrix());
				renderResource.SetTexture<PixelShader>(indexOfTextureAtPixel, castLight->GetShadowMapTexture());
				auto & samplerState = renderResource.GetSamplerState<PixelShader>()[indexOfTextureAtPixel];
				switch (castLight->GetShadowType())
				{
				case ShadowType::Default:
					samplerState.SetFilter(Filter::MIN_MAG_MIP_LINEAR);
					samplerState.SetComparisonFunc(ComparisonFunc::ALWAYS);
					break;
				case ShadowType::PCF:
					samplerState.SetFilter(Filter::COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
					samplerState.SetComparisonFunc(ComparisonFunc::LESS_EQUAL);
					break;
				case ShadowType::SoftPCCS:
					//TODO:: this function
					break;
				default:;
				}
			}
			else
			{
				renderResource.SetConstantBuffer<VertexShader>(0, 0, wvp, normalMatrix);
			}
			
			if(i == 0)
			{
				auto vertices = std::make_shared<PolygonPlePointBinder>(mesh->GetGeometry()->mPolygon->GetActualCount());
				
				mesh->GetGeometry()->mPolygon->ExpandAllTo(*vertices.get(), Operator::Multiply(matrix));
				bbr.AddBinder(vertices);
				bbr.AddBinder(mesh->GetGeometry()->GetNormalBinder());
				if (textureMap != nullptr)
				{
					bbr.AddBinder(mesh->GetGeometry()->GetUVBinder());
				}
			}
			renderResource.SetConstantBuffer<PixelShader>(0, structData);
			commands.push_back(DefaultRenderCommand::MakeRenderCommand(bbr, *mesh->GetGeometry()->mPolygonPleIndex.get(),
				RenderStage(renderState, renderResource)));
			
			i++;
		}
		if (i == 0) // no cast shadowable light. We just need a base pass
		{
			bbr.Clear();
			auto vertices = std::make_shared<PolygonPlePointBinder>(mesh->GetGeometry()->mPolygon->GetActualCount());
			mesh->GetGeometry()->mPolygon->ExpandAllTo(*vertices.get(), Operator::Multiply(matrix));
			bbr.AddBinder(vertices);
			auto & renderResource = cachePool.GetRenderResource(FindShaders(i));
			if (textureMap != nullptr)
			{
				bbr.AddBinder(mesh->GetGeometry()->GetUVBinder());
				renderResource.SetTexture<PixelShader>(0, textureMap);
			}

			blend.SetBlendEnable(false);
			depth.SetDepthEnable(true);
			depth.SetDepthFunc(ComparisonFunc::LESS_EQUAL);
			depth.SetDepthWriteMask(true);
			
			renderResource.SetConstantBuffer<PixelShader>(0, structData);
			renderResource.SetConstantBuffer<PixelShader>(1, 0, ambient);
			Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *mesh->GetGeometry()->mPolygonPleIndex.get(),
				RenderStage(renderState, renderResource)));
		}
		else
		{
			Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, SubRenderCommand::MakeRenderCommand(std::move(commands)));
		}
	}
	void MeshForwardRenderPath::DrawPhongMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh, std::vector<Light*>& lights, Point viewPosition)
	{
		auto material = static_cast<PhongMaterial *>(mesh->GetMaterial());
		DrawPhongOrLambertMaterial(cachePool, wvp, mesh, lights, material->GetBasicTexture(), [material] (void * ptr, size_t size)
		{
			auto size2 = sizeof(material->GetStructData());
			memcpy(ptr, &material->GetStructData(), size2);
			if(size2 != size)
				XGF_Warn(Render, "Size of constant buffer is incorrect");
			return size2;
		}, [&viewPosition] (void * ptr)
		{
			*((Point4 *)(ptr)) = viewPosition;
			return sizeof(Point4);
		});
	}

	void MeshForwardRenderPath::DrawLambertMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp,
		Mesh* mesh, std::vector<Light*>& lights)
	{
		auto material = static_cast<LambertMaterial *>(mesh->GetMaterial());
		DrawPhongOrLambertMaterial(cachePool, wvp, mesh, lights, material->GetBasicTexture(), [material](void * ptr, size_t size)
		{
			auto size2 = sizeof(material->GetStructData());
			memcpy(ptr, &material->GetStructData(), size2);
			if (size2 != size)
				XGF_Warn(Render, "Size of constant buffer is incorrect");
			return size2;
		}, [](void *ptr) { return 0; });
	}

	void MeshForwardRenderPath::Draw(MeshRenderResourceCachePool& cachePool, Camera& camera, Mesh* mesh,
	                                 std::vector<Light*>& lights)
	{
		auto material = mesh->GetMaterial();
		WVPMatrix wvp;
		camera.GetCameraMatrix(wvp);
		auto viewPosition = camera.GetPosition();
		switch (material->GetMaterialType())
		{
		case MaterialType::BasicMaterial:
				DrawBasicMaterial(cachePool, wvp, mesh);
			break;
		case MaterialType::PhongMaterial:
				DrawPhongMaterial(cachePool, wvp, mesh, lights, viewPosition);
			break;
		case MaterialType::LambertMaterial: 
			DrawLambertMaterial(cachePool, wvp, mesh, lights);
			break;
		default: ;
		}

	}

	void MeshForwardRenderPath::ReCreate(Mesh* mesh, std::vector<Light*>& lights)
	{
		ClearShaders();
		auto material = mesh->GetMaterial();
		
		switch (material->GetMaterialType())
		{
		case MaterialType::BasicMaterial:
		{
			auto basicMaterial = (BasicMaterial *)material;
				if(basicMaterial->GetBasicTexture() != nullptr)
					AddShaders(0, SystemShaders::GetBasicShaders(SystemShaders::BasicShader_Texture));
				else
					AddShaders(0, SystemShaders::GetBasicShaders(SystemShaders::BasicShader_Nothing)); 
				
			break;
		}
		case MaterialType::PhongMaterial:
		{
			auto phongMaterial = (PhongMaterial *)material;
			auto group = phongMaterial->GetLightGroup();
			int i = 0;
			for (auto light : lights)
			{
				if (!(light->GetGroup() & group)) continue;;
				if(light->GetLightType() != LightType::Ambient)
				{
					SystemShaders::ForwardShaderLightType lightType;
					switch (light->GetLightType())
					{
					case LightType::Directional:
						lightType = SystemShaders::ForwardShaderLightType::Directional;
						break;
					case LightType::Point:
						lightType = SystemShaders::ForwardShaderLightType::Point;
						break;
					case LightType::Spot:
						lightType = SystemShaders::ForwardShaderLightType::Spot;
						break;
					default: lightType = SystemShaders::ForwardShaderLightType::None;
					}
					int shaderType = SystemShaders::PhongForwardShader_Nothing;

					if (phongMaterial->GetBasicTexture() != nullptr)
						shaderType = SystemShaders::PhongForwardShader_Texture;
					SystemShaders::ForwardShaderShadowType shadowType = SystemShaders::ForwardShaderShadowType::None;
					if(((CastShadowAbleLight *)light)->GetCastShadow() && phongMaterial->GetReceiveShadow())
					{
						auto type = ((CastShadowAbleLight *)light)->GetShadowType();
						switch (type) { 
							case ShadowType::Default:
								shadowType = SystemShaders::ForwardShaderShadowType::DEF;
							break;
						case ShadowType::PCF:
								shadowType = SystemShaders::ForwardShaderShadowType::PCF;
							break;
						default:;
						}
					}
					if(i == 0)
						AddShaders(i++, SystemShaders::GetForwardBasePhongShaders(shaderType, lightType, shadowType));
					else
						AddShaders(i++, SystemShaders::GetForwardAddPhongShaders(shaderType, lightType, shadowType));
				}
			}
			break;
		}
		case MaterialType::LambertMaterial: 
		{
			auto lambertMaterial = (LambertMaterial *)material;
			int i = 0;
			auto group = lambertMaterial->GetLightGroup();
			for (auto light : lights)
			{
				if (!(light->GetGroup() & group)) continue;;
				if (light->GetLightType() != LightType::Ambient)
				{
					SystemShaders::ForwardShaderLightType lightType;
					switch (light->GetLightType())
					{
					case LightType::Directional:
						lightType = SystemShaders::ForwardShaderLightType::Directional;
						break;
					case LightType::Point:
						lightType = SystemShaders::ForwardShaderLightType::Point;
						break;
					case LightType::Spot:
						lightType = SystemShaders::ForwardShaderLightType::Spot;
						break;
					default:
						lightType = SystemShaders::ForwardShaderLightType::None;
					}
					int shaderType = SystemShaders::LambertForwardShader_Nothing;

					if (lambertMaterial->GetBasicTexture() != nullptr)
						shaderType = SystemShaders::LambertForwardShader_Texture;
					SystemShaders::ForwardShaderShadowType shadowType = SystemShaders::ForwardShaderShadowType::None;
					if (((CastShadowAbleLight *)light)->GetCastShadow() && lambertMaterial->GetReceiveShadow())
					{
						auto type = ((CastShadowAbleLight *)light)->GetShadowType();
						switch (type) {
						case ShadowType::Default:
							shadowType = SystemShaders::ForwardShaderShadowType::DEF;
							break;
						case ShadowType::PCF:
							shadowType = SystemShaders::ForwardShaderShadowType::PCF;
							break;
						default:;
						}
					}
					if (i == 0)
						AddShaders(i++, SystemShaders::GetForwardBaseLambertShaders(shaderType, lightType, shadowType));
					else
						AddShaders(i++, SystemShaders::GetForwardAddLambertShaders(shaderType, lightType, shadowType));
				}
			}
			break;
		}
		default: ;
		}
	}
}


