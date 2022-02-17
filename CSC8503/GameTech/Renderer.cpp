#include "PS4GameRenderer.h"
#include "Renderer.h"
#include "../../Plugins/OpenGLRendering/OGLRendererAPI.h"
#include "../../Plugins/OpenGLRendering/OGLFrameBuffer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"

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

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({ Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();

	shadowFBO = new OGLFrameBuffer();
	shadowFBO->AddTexture();

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

	delete defaultShader;

	delete shadowFBO;
	delete shadowShader;

	delete skyboxShader;
	delete skyboxMesh;
	delete skyboxTex;
}

void Renderer::LoadSkybox() {
	GLuint tex;
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6] = { 0 };
	int height[6] = { 0 };
	int channels[6] = { 0 };
	int flags[6] = { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	delete skyboxTex;
	skyboxTex = new OGLTexture(tex);
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->BeginFrame();
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderScene();
	rendererAPI->RenderFrame();
	glDisable(GL_CULL_FACE);

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

	rendererAPI->BindFrameBuffer(shadowFBO);
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

// Render skybox
 	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	rendererAPI->BindShader(skyboxShader);

	rendererAPI->UpdateUniformMatrix4(skyboxShader, "projMatrix", projMatrix);
	rendererAPI->UpdateUniformMatrix4(skyboxShader, "viewMatrix", viewMatrix);

	rendererAPI->BindCubemap(skyboxTex, "cubeTex", 0);

	rendererAPI->DrawMesh(skyboxMesh);


	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
// Render Scene

	/*float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);*/

	for (const auto& i : activeObjects) {
		ShaderBase* shader = (*i).GetShader();
		rendererAPI->BindShader(shader);

		rendererAPI->BindTexture((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);

		rendererAPI->UpdateUniformMatrix4(shader, "projMatrix", projMatrix);
		rendererAPI->UpdateUniformMatrix4(shader, "viewMatrix", viewMatrix);
		rendererAPI->UpdateUniformVector3(shader, "cameraPos", gameWorld.GetMainCamera()->GetPosition());

		rendererAPI->UpdateUniformVector4(shader, "lightColour", lightColour);
		rendererAPI->UpdateUniformVector3(shader, "lightPos", lightPos);
		rendererAPI->UpdateUniformFloat(shader, "lightRadius", lightRadius);

		rendererAPI->BindTexture(shadowFBO->GetTexture(), "shadowTex", 1);

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		rendererAPI->UpdateUniformMatrix4(shader, "modelMatrix", modelMatrix);
		rendererAPI->UpdateUniformMatrix4(shader, "shadowMatrix", fullShadowMat);

		rendererAPI->UpdateUniformVector4(shader, "objectColour", i->GetColour());
		rendererAPI->UpdateUniformInt(shader, "hasVertexColours", !(*i).GetMesh()->GetColourData().empty());
		rendererAPI->UpdateUniformInt(shader, "hasTexture", (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);




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