#ifdef _ORBIS
#include "PS4GameRenderer.h"
#include "../CSC8503Common/RenderObject.h"
#include "../CSC8503Common/Transform.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
#include "../../Plugins/PlayStation4/PS4Shader.h"
#include "../../Plugins/PlayStation4/PS4Texture.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace NCL::PS4;

PS4GameRenderer::PS4GameRenderer(GameWorld& world) : PS4RendererBase(*Window::GetWindow()), gameWorld(world) {
	rotation	= 0.0f;
	translation = 0.0f;

	defaultShader = PS4Shader::GenerateShader(
		"/app0/Assets/Shaders/PS4/VertexShader.sb",
		"/app0/Assets/Shaders/PS4/PixelShader.sb"
	);

	defaultMesh	= PS4Mesh::GenerateTriangle();
	defaultMesh->UploadToGPU(this);
	defaultTexture	= PS4Texture::LoadTextureFromFile("/app0/Assets/Textures/checkerboard.gnf");

	CSC8503::Transform* newTransform = new CSC8503::Transform();
	CSC8503::Transform* newTransform2 = new CSC8503::Transform();
	defaultObject[0] = new CSC8503::RenderObject(newTransform,(MeshGeometry*)defaultMesh, (TextureBase*)defaultTexture, (ShaderBase*)defaultShader);
	defaultObject[1] = new CSC8503::RenderObject(newTransform2, (MeshGeometry*)defaultMesh, (TextureBase*)defaultTexture, (ShaderBase*)defaultShader);

	camMatrix = (CameraMatrix*)onionAllocator->allocate(sizeof(CameraMatrix), Gnm::kEmbeddedDataAlignment4);
	camMatrix->projMatrix = Matrix4();
	camMatrix->viewMatrix = Matrix4();
	camMatrix->viewProjMatrix = Matrix4();

	cameraBuffer.initAsConstantBuffer(camMatrix, sizeof(CameraMatrix));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeSC); // it's a constant buffer, so read-only is OK


	// Skybox
	skyboxMesh = PS4Mesh::GenerateQuad();
	skyboxMesh->UploadToGPU(this);

	skyboxShader = PS4Shader::GenerateShader(
		"/app0/Assets/Shaders/PS4/skyboxVertex.sb",
		"/app0/Assets/Shaders/PS4/skyboxPixel.sb"
	);

	skyboxTexture = PS4Texture::LoadTextureFromFile("/app0/Assets/Textures/Cubemap/skyrender.gnf");
	Gnm::Texture test = skyboxTexture->GetAPITexture();
	test.setTextureType(Gnm::kTextureTypeCubemap);
}

PS4GameRenderer::~PS4GameRenderer()
{
	delete defaultMesh;
	delete defaultTexture;
	delete defaultShader;


	delete defaultObject[0];
	delete defaultObject[1];

	delete skyboxTexture;
	delete skyboxShader;
	delete skyboxMesh;
}

void PS4GameRenderer::Update(float dt)	{
	rotation	+= 30.0f * dt;
	translation += dt;

	(defaultObject[0])->GetTransform()
		->SetPosition(Vector3(-0.4, sin(translation), 0));


	defaultObject[1]->GetTransform()
		->SetPosition(Vector3(0.4, 0, sin(translation)));
}

void PS4GameRenderer::RenderSkybox() {
	// Disable blend and depth
	/*sce::Gnm::DepthStencilControl depth = sce::Gnm::DepthStencilControl();
	depth.setDepthEnable(false);
	depth.setStencilEnable(false);
	currentGFXContext->setDepthStencilControl(depth);*/


	// Send texture to shader

	// Draw Mesh

	//int camIndex = skyboxShader->GetConstantBufferIndex("CameraData");
	//currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndex, 1, &cameraBuffer);

	int testCamIndex = skyboxShader->GetConstantBufferIndex("CameraData");
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, testCamIndex, 1, &cameraBuffer);

	skyboxShader->SubmitShaderSwitch(*currentGFXContext);
	skyboxMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);

	// Enable blend and depth
	/*depth.setDepthEnable(true);
	depth.setStencilEnable(true);
	currentGFXContext->setDepthStencilControl(depth);*/

}

void PS4GameRenderer::RenderFrame() {
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	//Primitive Setup State
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	////Screen Access State
	Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	currentGFXContext->setDepthStencilControl(dsc);

	Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);


	float screenAspect = (float)currentWidth / (float)currentHeight;
	camMatrix->viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	camMatrix->projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	camMatrix->viewProjMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect) * gameWorld.GetMainCamera()->BuildViewMatrix();

	BuildObjectList();

	/*skyboxShader->SubmitShaderSwitch(*currentGFXContext);
	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &skyboxTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	RenderSkybox();*/

	defaultShader->SubmitShaderSwitch(*currentGFXContext);
	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
	RenderCamera();

	currentFrame->EndFrame();
}


void PS4GameRenderer::DrawRenderObject(const CSC8503::RenderObject* o) {
	Matrix4* modelMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*modelMat = o->GetTransform()->GetMatrix();

	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(modelMat, sizeof(Matrix4));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	//PS4Shader*	realShader = (PS4Shader*)o->GetShader() == NULL ? (PS4Shader*)o->GetShader() : (PS4Shader*)defaultShader;
	//PS4Mesh*	realMesh	= (PS4Mesh*)o->GetMesh() == NULL ? (PS4Mesh*)o->GetMesh() : (PS4Mesh*)defaultMesh;

	PS4Shader* realShader = defaultShader;
	PS4Mesh*	realMesh	= (PS4Mesh*)o->GetMesh();

	int objIndex = realShader->GetConstantBufferIndex("RenderObjectData");
	int camIndex = realShader->GetConstantBufferIndex("CameraData");

	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &constantBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndex, 1, &cameraBuffer);

	realShader->SubmitShaderSwitch(*currentGFXContext);
	realMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void PS4GameRenderer::BuildObjectList() {
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

void PS4GameRenderer::RenderCamera() {
	for (const auto& i : activeObjects) {
		DrawRenderObject((i));
	}
}
#endif
