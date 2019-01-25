#pragma once
#include "Defines.hpp"

namespace XGF
{
	class ShaderStage;
	namespace Shape 
	{
		class Light;

		class Mesh;
		
		class MeshRenderer
		{
		public:
			void Initialize();
			void Shutdown();
			MeshRenderer();
			~MeshRenderer();
			void Add(Mesh * mesh);
			void Remove(Mesh * mesh);
			void Add(Light * light);
			void Remove(Light * light);

			void Begin(const WVPMatrix & wvp);
			void Draw(const Point & cameraPosition);
			void End();

			void RefreshLightMesh();
		private:
			void CastLight(Mesh* mesh);;
			std::vector<std::pair<std::vector<Mesh *>, ::XGF::ShaderStage *>> mRendererGroup;
			std::vector<Mesh *> mNullRendererGroup;
			std::vector<Light *> mLights;

		};

	}
}
