#pragma once
#include "RendererBase.h"
#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
	namespace Rendering {
		class SimpleFont;

		struct CamMatrix {
			Vector3 viewMatrix;
		}

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
			void Paint(RenderObject paintable, Vector3 pos, float radius = 1.0f, float hardness = .5f, float strength = 0.5f, Vector4 color = Vector4(0,0,0,0));
			void ApplyPaintToMasks();

			void RenderShadows();
			void RenderSkybox();
			void RenderObjects();


			// Debug
			Maths::Matrix4 SetupDebugLineMatrix() const override;
			Maths::Matrix4 SetupDebugStringMatrix() const override;
		protected:

			struct PaintInstance {
				RenderObject* object;
				Maths::Vector3 pos;
				float radius;
				float hardness;
				float strength;
				Vector4 colour;
			};

			CSC8503::GameWorld& gameWorld;
			vector<const RenderObject*> activeObjects;
			vector<PaintInstance> paintInstances;

			FrameBufferBase* shadowFBO;
			ShaderBase* shadowShader;

			FrameBufferBase* maskFBO;
			ShaderBase* maskShader;

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

