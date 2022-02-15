#ifdef _WIN64
#pragma once
#include "../../Plugins/OpenGLRendering/OGLRendererAPI.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

#include "../CSC8503Common/GameWorld.h"

namespace NCL {
	namespace Maths {
		class Vector3;
		class Vector4;
	}
	namespace CSC8503 {
		class RenderObject;

		class OGLGameRenderer : public OGLRendererAPI	{
		public:
			OGLGameRenderer(GameWorld& world);
			~OGLGameRenderer();

		protected:
			void RenderFrame()	override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap(Camera* camera);
			void RenderCamera(Camera* camera);
			void RenderSkybox(Camera* camera);
			void RenderPerspective(Camera* camera);

			void LoadSkybox();

			vector<const RenderObject*> activeObjects;

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

#endif