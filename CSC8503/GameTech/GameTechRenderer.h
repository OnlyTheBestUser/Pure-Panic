#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"
#include "../GameTech/GameRenderCommands.h"

namespace NCL {
	class Maths::Vector3;
	class Maths::Vector4;
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer, public GameRenderCommands	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

		protected:
			void RenderFrame()	override;

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader*		defaultShader;

			
			void RenderShadowMap() override;
			void RenderCamera() override;
			void RenderSkybox() override;

			void LoadSkybox() override;
			void RenderScene() override;

			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;
		};
	}
}

