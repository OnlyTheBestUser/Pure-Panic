#pragma once
#include "../../Common/RendererBase.h"
#include "../CSC8503Common/GameWorld.h"

namespace NCL {
	namespace Rendering {
		class WorldRenderer
		{
		public:
			WorldRenderer(GameWorld& world);
			~WorldRenderer();

			void Update(float dt);
			void Render();
			void RenderScene();

			RendererBase* GetRenderer() { return renderer; }
		protected:
			void BuildObjectList();
			void SortObjectList();

			RendererBase* renderer;

			GameWorld& gameWorld;
			vector<const RenderObject*> activeObjects;

			MeshGeometry* defaultMesh;
			TextureBase* defaultTex;
			ShaderBase* defaultShader;


		};
	}
}

