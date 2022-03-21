#pragma once
#include "RendererBase.h"
#include "../../CSC8503/CSC8503Common/GameWorld.h"
namespace NCL {
	namespace Rendering {
		class SimpleFont;
		class UniformBuffer;

		struct CameraMatrix {
			Matrix4 projMatrix;
			Matrix4 viewMatrix;
		};

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
			void Paint(const RenderObject* paintable, Vector3& pos, Vector3& colpos, Vector2& texUV_a, Vector2& texUV_b, Vector2& texUV_c, float radius = 1.0f, float hardness = .5f, float strength = 0.5f, Vector4 color = Vector4(0,0,0,0));
			Maths::Vector2 GetUVCoord(const RenderObject* paintable, NCL::Maths::Vector3 pos); // Gets where the uv point on a texture is given the object and collision position
			void ApplyPaintToMasks();
			void DrawGUI();
			NCL::Maths::Vector2 CountPaintMask(TextureBase* paintMask, NCL::Maths::Vector2 prevScores, NCL::Maths::Vector4 team1Colour, NCL::Maths::Vector4 team2Colour);

			void RenderShadows();
			void RenderSkybox();
			void RenderObjects();

			void ClearPaint();


			// Debug
			Maths::Matrix4 SetupDebugLineMatrix() const override;
			Maths::Matrix4 SetupDebugStringMatrix() const override;
		protected:

			struct PaintInstance {
				const RenderObject* object;
				Vector3 colPoint;
				Vector3 barycentric;
				Vector2 texUV_a;
				Vector2 texUV_b;
				Vector2 texUV_c;
				float radius;
				float hardness;
				float strength;
				Vector4 colour;
			};

			CameraMatrix camMatrix;
			UniformBuffer* camBuffer;

			CSC8503::GameWorld& gameWorld;
			vector<const RenderObject*> activeObjects;
			vector<PaintInstance> paintInstances;

			FrameBufferBase* shadowFBO;
			ShaderBase* shadowShader;

			ShaderBase* maskShader;

			ShaderBase* uiShader;

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

