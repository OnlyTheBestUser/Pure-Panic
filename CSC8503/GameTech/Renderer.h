#pragma once
#include "../../Common/RendererAPI.h"

#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
	namespace Rendering {
		class Renderer
		{
		public:
			Renderer(GameWorld& world);
			~Renderer();

			void Update(float dt);
			void Render();

			RendererAPI* GetRendererAPI() { return rendererAPI;}
		protected:
			RendererAPI* rendererAPI;
		};
	}
}

