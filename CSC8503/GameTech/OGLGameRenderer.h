#ifdef _WIN64
#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

namespace NCL {
	namespace Maths {
		class Vector3;
		class Vector4;
	}
	namespace CSC8503 {
		class RenderObject;

		class OGLGameRenderer : public OGLRenderer	{
		public:
			OGLGameRenderer();
			~OGLGameRenderer();

		protected:
			void RenderFrame()	override;

			Matrix4 SetupDebugLineMatrix()	const override;
			Matrix4 SetupDebugStringMatrix()const override;

			OGLShader*		defaultShader;

			void RenderShadowMap(Camera* camera);
			void RenderCamera(Camera* camera);
			void RenderSkybox(Camera* camera);
			void RenderPerspective(Camera* camera);

			void LoadSkybox();


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