#ifdef _ORBIS
#pragma once
#include "../../Plugins/PlayStation4/PS4RendererBase.h"

namespace NCL {
	namespace PS4 {
		class ExampleRenderer :
			public PS4RendererBase
		{
		public:
			ExampleRenderer(PS4Window* window);
			~ExampleRenderer();

			void Update(float dt)	 override;

		protected:
			void	RenderFrame()	override;

			void DrawRenderObject(CSC8503::RenderObject* o);

			float rotation;
			float translation;
			CSC8503::RenderObject* defaultObject[2];

			//default data
			PS4Shader*	defaultShader;
			PS4Mesh*	defaultMesh;
			PS4Texture* defaultTexture;

			//		
			NCL::Maths::Matrix4*	viewProjMat;
			Gnm::Buffer	cameraBuffer;
		};
	}
}
#endif

