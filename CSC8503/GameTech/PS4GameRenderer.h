#ifdef _ORBIS
#pragma once
#include "../../Plugins/PlayStation4/PS4RendererAPI.h"
#include "../CSC8503Common/GameWorld.h"

namespace NCL {

	namespace PS4 {
		struct CameraMatrix {
			Matrix4 projMatrix;
			Matrix4 viewMatrix;
			Matrix4 viewProjMatrix;
			Matrix4 invProjMatrix;
		};

		struct LightInfo {
			Vector3 lightPos;
			float lightRadius;
			Vector4 lightColour;
		};

		struct ModelInfo {
			Matrix4 modelMatrix;
			Matrix4 invModelMatrix;
		};

		class PS4GameRenderer :
			public PS4RendererAPI
		{
		public:
			PS4GameRenderer(CSC8503::GameWorld& world);
			~PS4GameRenderer();

			void Update(float dt)	 override;

		protected:
			void	RenderFrame()	override;

			void DrawRenderObject(const CSC8503::RenderObject* o);

			float rotation;
			float translation;

			//default data
			PS4Shader*	defaultShader;
			PS4Mesh*	defaultMesh;
			PS4Texture* defaultTexture;

			CSC8503::GameWorld& gameWorld;
			vector<const RenderObject*> activeObjects;

			void BuildObjectList();
			void SortObjectList();

			void RenderCamera();
			void RenderSkybox();

			Gnm::Buffer	cameraBuffer;
			CameraMatrix* camMatrix;

			PS4Texture* skyboxTexture;
			PS4Mesh* skyboxMesh;
			PS4Shader* skyboxShader;

			Gnm::Buffer lightBuffer;
			LightInfo* globalLight;
		};
	}
}
#endif

