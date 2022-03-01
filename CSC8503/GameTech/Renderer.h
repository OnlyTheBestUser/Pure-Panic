#pragma once
#include "RendererBase.h"
#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
	namespace Rendering {
		class SimpleFont;

		class Renderer : public RendererBase
		{
		public:
			Renderer(CSC8503::GameWorld& world);
			~Renderer();

			void Update(float dt) override;
			void Render() override;

			void BuildObjectList();
			void SortObjectList();
			void RenderScene();

			void RenderShadows();
			void RenderSkybox();
			void RenderObjects();


			// Debug
			Maths::Matrix4 SetupDebugLineMatrix() const override;
			Maths::Matrix4 SetupDebugStringMatrix() const override;
		protected:
			CSC8503::GameWorld& gameWorld;
			vector<const RenderObject*> activeObjects;

			FrameBufferBase* shadowFBO;
			ShaderBase* shadowShader;

			ShaderBase* skyboxShader;
			MeshGeometry* skyboxMesh;
			TextureBase* skyboxTex;

			Vector4 lightColour;
			float lightRadius;
			Vector3 lightPos;
			Matrix4 shadowMatrix;

		};
	}
}

