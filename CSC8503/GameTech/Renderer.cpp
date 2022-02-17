#include "PS4GameRenderer.h"
#include "Renderer.h"
#include "OGLGameRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLFrameBuffer.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/MeshGeometry.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

Renderer::Renderer(GameWorld& world) : RendererBase(), gameWorld(world) {
#ifdef _WIN64
	rendererAPI = new OGLRendererAPI(*Window::GetWindow());
	debugLinesMesh = new OGLMesh();
	debugTextMesh = new OGLMesh();

	debugLinesMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugLinesMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));

	debugTextMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugTextMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));
	debugTextMesh->SetVertexTextureCoords(std::vector<Vector2>(5000, Vector3()));

	debugTextMesh->UploadToGPU();
	debugLinesMesh->UploadToGPU();

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
	debugShader = new OGLShader("debugVert.glsl", "debugFrag.glsl");

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	TextureLoader::RegisterAPILoadFunction(OGLTexture::RGBATextureFromFilename);
#endif
#ifdef _ORBIS
	rendererAPI = new PS4::PS4GameRenderer(world);

	debugLinesMesh = PS4::PS4Mesh::GenerateQuad();
	debugTextMesh = PS4::PS4Mesh::GenerateQuad();

	debugTextMesh->UploadToGPU();
	debugLinesMesh->UploadToGPU();

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
#endif

	font = new SimpleFont("PressStart2P.fnt", "PressStart2P.png");
	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPos = Vector3(-200.0f, 60.0f, -200.0f);
}

Renderer::~Renderer() {
	delete rendererAPI;
	delete font;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->BeginFrame();

	BuildObjectList();
	SortObjectList();
	RenderScene();
	rendererAPI->RenderFrame();

	rendererAPI->EndFrame();
	DrawDebugData();
	rendererAPI->SwapBuffers();
}

void Renderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}

void Renderer::SortObjectList() {
	//Who cares!
}

void Renderer::RenderScene() {
// Render Shadow Map
#ifdef _WIN64
	OGLFrameBuffer shadowFBO;
	shadowFBO.AddTexture();
#endif

	rendererAPI->BindFrameBuffer(&shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, 4096, 4096);
	glCullFace(GL_FRONT);

	rendererAPI->BindShader(shadowShader);

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);
	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;
	Matrix4 biasMat = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

	shadowMatrix = biasMat * mvMatrix;

	for (const auto& i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 mvpMatrix = mvMatrix * modelMatrix;
		rendererAPI->UpdateUniformMatrix4(shadowShader, "mvpMatrix", mvpMatrix);
		rendererAPI->DrawMesh((*i).GetMesh());
	}
	glViewport(0, 0, rendererAPI->GetCurrentWidth(), rendererAPI->GetCurrentHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	rendererAPI->BindFrameBuffer();
	glCullFace(GL_BACK);

// Render Scene

	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	for (const auto& i : activeObjects) {
		ShaderBase* shader = (*i).GetShader();
		rendererAPI->BindShader(shader);

		rendererAPI->BindTexture((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		rendererAPI->UpdateUniformMatrix4(shader, "projMatrix", projMatrix);
		rendererAPI->UpdateUniformMatrix4(shader, "viewMatrix", viewMatrix);
		rendererAPI->UpdateUniformVector3(shader, "cameraPos", gameWorld.GetMainCamera()->GetPosition());

		rendererAPI->BindTexture(shadowFBO.GetTexture(), "shadowTex", 1);

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		rendererAPI->UpdateUniformMatrix4(shader, "modelMatrix", modelMatrix);
		rendererAPI->UpdateUniformMatrix4(shader, "shadowMatrix", fullShadowMat);


		rendererAPI->DrawMesh((*i).GetMesh());
	}



}


Maths::Matrix4 Renderer::SetupDebugLineMatrix()	const {
	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

Maths::Matrix4 Renderer::SetupDebugStringMatrix()	const {
	return Matrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}