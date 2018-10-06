#pragma once
#include "Defines.hpp"
namespace XGF
{
	class Batch;
	class GDI;
	namespace Shape 
	{
		
		class Mesh;
		
		class MeshRenderer
		{
		public:
			void Initialize(GDI * gdi);
			void Shutdown();
			MeshRenderer();
			~MeshRenderer();
			void Add(Mesh * mesh);
			void Remove(Mesh * mesh);

			void Begin(const WVPMatrix & wvp);
			void Draw();
			void End();

		private:
			std::vector<std::pair<std::vector<Mesh *>, ::XGF::Batch *>> mRendererGroup;

			GDI * mGDI;

		};

	}
}
