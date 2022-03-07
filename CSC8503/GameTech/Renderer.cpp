#ifdef _ORBIS
#include "../../Plugins/PlayStation4/PS4Mesh.h"
#include "../../Plugins/PlayStation4/PS4Shader.h"
#include "../../Plugins/PlayStation4/PS4Texture.h"
#include "../../Plugins/PlayStation4/PS4UniformBuffer.h"
#endif
#include "Renderer.h"

#include "../../Common/Assets.h"
#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/MeshGeometry.h"



#include "../../Plugins/OpenGLRendering/OGLFrameBuffer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;



Renderer::Renderer(GameWorld& world) : RendererBase(), gameWorld(world) {
#ifdef _WIN64

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	rendererAPI->SetDepth(true);
	rendererAPI->SetClearColour(1, 1, 1, 1);

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({ Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	ForceValidDebugState(true);

	skyboxTex = OGLTexture::RGBATextureCubemapFromFilename(
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	);

	shadowFBO = new OGLFrameBuffer();
	shadowFBO->AddTexture();

	//maskFBO = new OGLFrameBuffer();
	//maskFBO->AddTexture(2048 / 4, 2048 / 4);
	maskShader = new OGLShader("MaskVertex.glsl", "MaskFragment.glsl");

#endif
#ifdef _ORBIS
	skyboxMesh = PS4::PS4Mesh::GenerateQuad();
	skyboxMesh->UploadToGPU(rendererAPI);

	skyboxShader = PS4::PS4Shader::GenerateShader(
		Assets::SHADERDIR + "PS4/skyboxVertex.sb",
		Assets::SHADERDIR + "PS4/skyboxPixel.sb"
	);

	skyboxTex = PS4::PS4Texture::LoadSkyboxFromFile(NCL::Assets::TEXTUREDIR + "Cubemap/cubemap.gnf");
	PS4::PS4UniformBuffer test((PS4::PS4RendererAPI*)rendererAPI, (uint32_t)sizeof(CamMatrix));
	CamMatrix* cam = new CamMatrix();
	test.SetData(cam, (uint32_t)sizeof(CamMatrix), 0);
#endif

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPos = Vector3(-200.0f, 60.0f, -200.0f);
}

Renderer::~Renderer() {
	delete shadowFBO;
	delete shadowShader;

	delete skyboxShader;
	delete skyboxMesh;
	delete skyboxTex;

	delete maskFBO;
	delete maskShader;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->BeginFrame();


	rendererAPI->SetCullFace(true);
	rendererAPI->SetClearColour(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();

	for (const auto& i : activeObjects) {
		Paint(i, Vector3(0, 0, 0), 1.0f, 1.0f, 1.0f, Vector4(1, 0, 0, 1));
	}
#ifdef _WIN64
	ApplyPaintToMasks();
#endif
	RenderScene();
	rendererAPI->SetCullFace(false);

	rendererAPI->EndFrame();
#ifdef _WIN64
	DrawDebugData();
#endif
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
#ifdef _WIN64
	RenderShadows();
#endif
	RenderSkybox();
	RenderObjects();
}

void Renderer::RenderShadows() {
	rendererAPI->SetCullFace(true);
	rendererAPI->SetBlend(true);
	rendererAPI->SetDepth(true);

	rendererAPI->BindFrameBuffer(shadowFBO);

	rendererAPI->ClearBuffer(false, true, false);
	rendererAPI->SetColourMask(false, false, false, false);
	rendererAPI->SetViewportSize(4096, 4096);
	rendererAPI->SetCullType(NCL::Rendering::RendererAPI::CULL_TYPE::FRONT);

	shadowShader->BindShader();

	Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);
	Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;
	Matrix4 biasMat = Matrix4::Translation(Vector3(0.5, 0.5, 0.5)) * Matrix4::Scale(Vector3(0.5, 0.5, 0.5));

	shadowMatrix = biasMat * mvMatrix;

	for (const auto& i : activeObjects) {
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		Matrix4 mvpMatrix = mvMatrix * modelMatrix;
		shadowShader->UpdateUniformMatrix4("mvpMatrix", mvpMatrix);

		rendererAPI->DrawMeshAndSubMesh((*i).GetMesh());
	}
	rendererAPI->SetViewportSize(rendererAPI->GetCurrentWidth(), rendererAPI->GetCurrentHeight());
	rendererAPI->SetColourMask(true, true, true, true);
	rendererAPI->BindFrameBuffer();
	rendererAPI->SetCullType(NCL::Rendering::RendererAPI::CULL_TYPE::BACK);
}

void Renderer::RenderSkybox() {
	rendererAPI->SetCullFace(false);
	rendererAPI->SetBlend(false);
	rendererAPI->SetDepth(false);

	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	skyboxShader->BindShader();

	skyboxShader->UpdateUniformMatrix4("projMatrix", projMatrix);
	skyboxShader->UpdateUniformMatrix4("viewMatrix", viewMatrix);
	skyboxShader->UpdateUniformMatrix4("invProjMatrix", projMatrix.Inverse());

	skyboxTex->Bind(0);

	rendererAPI->DrawMesh(skyboxMesh);

	rendererAPI->SetCullFace(true);
	rendererAPI->SetBlend(true);
	rendererAPI->SetDepth(true);
}

void Renderer::RenderObjects() {
	ShaderBase* activeShader = nullptr;

	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	for (const auto& i : activeObjects) {
		ShaderBase* shader = (*i).GetShader();

		if (activeShader != shader) {
			shader->BindShader();

			shader->UpdateUniformMatrix4("projMatrix", projMatrix);
			shader->UpdateUniformMatrix4("viewMatrix", viewMatrix);
			shader->UpdateUniformMatrix4("viewProjMatrix", projMatrix * viewMatrix);
			shader->UpdateUniformMatrix4("invProjMatrix", projMatrix.Inverse());
			shader->UpdateUniformVector3("cameraPos", gameWorld.GetMainCamera()->GetPosition());

			shader->UpdateUniformVector4("lightColour", lightColour);
			shader->UpdateUniformVector3("lightPos", lightPos);
			shader->UpdateUniformFloat( "lightRadius", lightRadius);

			activeShader = shader;
		}

		if ((*i).GetDefaultTexture()) (*i).GetDefaultTexture()->Bind(0);
		shader->UpdateUniformInt("hasTexture", (*i).GetDefaultTexture() ? 1 : 0);
#ifdef _WIN64
		if (shadowFBO->GetTexture()) {
			shadowFBO->GetTexture()->Bind(1);
		}
		//rendererAPI->BindTexture(i->GetPaintMask(), "paintMaskTex", 2);
		//rendererAPI->UpdateUniformInt(shader, "hasPaintMask", (*i).GetPaintMask() ? 1 : 0);
#endif

		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
#ifdef _WIN64
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
#endif
		shader->UpdateUniformMatrix4("modelMatrix", modelMatrix);
		shader->UpdateUniformMatrix4("invModelMatrix", modelMatrix.Inverse());
#ifdef _WIN64
		shader->UpdateUniformMatrix4("shadowMatrix", fullShadowMat);

		shader->UpdateUniformVector4("objectColour", i->GetColour());
		shader->UpdateUniformInt("hasVertexColours", !(*i).GetMesh()->GetColourData().empty());
#endif

		rendererAPI->DrawMeshAndSubMesh((*i).GetMesh());
	}

	/* 
	* Go through all paint instances <- struct of all information from paint method
	* bind paint shader 
	* draw each object that is part
	*/
}

void Renderer::Paint(const RenderObject* paintable, NCL::Maths::Vector3 pos, float radius, float hardness, float strength, NCL::Maths::Vector4 colour)
{
	PaintInstance pi;
	pi.object = paintable;
	pi.pos = pos;
	pi.radius = radius;
	pi.hardness = hardness;
	pi.strength = strength;
	pi.colour = colour;

	paintInstances.push_back(pi);
}

void Renderer::ApplyPaintToMasks() {
#ifdef _WIN64
	rendererAPI->SetDepth(false);
	rendererAPI->SetBlend(true);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);

	maskShader->BindShader();

	Vector2 currentSize;
	for (const auto& i : paintInstances) {
		if (i.object->GetPaintMask() == 0) continue;
		maskFBO = new OGLFrameBuffer();
		maskFBO->AddTexture((OGLTexture*)(i.object->GetPaintMask()));

		if (Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight()) != currentSize) {
			currentSize = Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight());
			rendererAPI->SetViewportSize(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight());
		}

		maskShader->UpdateUniformMatrix4("modelMatrix", i.object->GetTransform()->GetMatrix());

		// Update uniforms here

		rendererAPI->BindFrameBuffer(maskFBO);

		rendererAPI->DrawMesh(skyboxMesh);
		delete maskFBO;
	}
	glBlendFunc(GL_ONE, GL_NONE);
	rendererAPI->SetBlend(false);
	rendererAPI->SetDepth(true);
	rendererAPI->SetViewportSize(rendererAPI->GetCurrentWidth(), rendererAPI->GetCurrentHeight());
	rendererAPI->ClearBuffer(true, true, true);
	rendererAPI->BindFrameBuffer();
#endif
	paintInstances.clear();
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

