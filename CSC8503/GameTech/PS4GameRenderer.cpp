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

	camMatrix = (CameraMatrix*)onionAllocator->allocate(sizeof(CameraMatrix), Gnm::kEmbeddedDataAlignment4);
	camMatrix->projMatrix = Matrix4();
	camMatrix->viewMatrix = Matrix4();
	camMatrix->viewProjMatrix = Matrix4();
	camMatrix->invProjMatrix = Matrix4();

	cameraBuffer.initAsConstantBuffer(camMatrix, sizeof(CameraMatrix));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeSC); // it's a constant buffer, so read-only is OK

	globalLight = (LightInfo*)onionAllocator->allocate(sizeof(LightInfo), Gnm::kEmbeddedDataAlignment4);
	globalLight->lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	globalLight->lightRadius = 1000.0f;
	globalLight->lightPos = Vector3(-200.0f, 60.0f, -200.0f);

	lightBuffer.initAsConstantBuffer(globalLight, sizeof(LightInfo));
	lightBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeSC); // it's a constant buffer, so read-only is OK


	// Skybox
	skyboxMesh = PS4Mesh::GenerateQuad();
	skyboxMesh->UploadToGPU(this);

	skyboxShader = PS4Shader::GenerateShader(
		"/app0/Assets/Shaders/PS4/skyboxVertex.sb",
		"/app0/Assets/Shaders/PS4/skyboxPixel.sb"
	);

	skyboxTexture = PS4Texture::LoadSkyboxFromFile("/app0/Assets/Textures/Cubemap/cubemap.gnf");

}

PS4GameRenderer::~PS4GameRenderer()
{
	delete defaultMesh;
	delete defaultTexture;
	delete defaultShader;

	delete camMatrix;

	delete skyboxTexture;
	delete skyboxShader;
	delete skyboxMesh;
}

void PS4GameRenderer::Update(float dt)	{
	rotation	+= 30.0f * dt;
	translation += dt;
}

void PS4GameRenderer::RenderSkybox() {

	int camIndex = skyboxShader->GetConstantBufferIndex(Gnm::kShaderStageVs, "CameraData");
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndex, 1, &cameraBuffer);

	skyboxShader->SubmitShaderSwitch(*currentGFXContext);
	skyboxMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);

}

void PS4GameRenderer::RenderFrame() {
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	camMatrix->viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	camMatrix->projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	camMatrix->viewProjMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect) * gameWorld.GetMainCamera()->BuildViewMatrix();
	camMatrix->invProjMatrix = camMatrix->projMatrix.Inverse();

	BuildObjectList();

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
	dsc.setDepthEnable(false);
	currentGFXContext->setDepthStencilControl(dsc);

	Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);
	
	skyboxShader->SubmitShaderSwitch(*currentGFXContext);
	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &skyboxTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	RenderSkybox();

	//Primitive Setup State
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	////Screen Access State
	dsc.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	currentGFXContext->setDepthStencilControl(dsc);



	defaultShader->SubmitShaderSwitch(*currentGFXContext);
	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	RenderCamera();

	currentFrame->EndFrame();
}


void PS4GameRenderer::DrawRenderObject(const CSC8503::RenderObject* o) {
	ModelInfo* modelData = (ModelInfo*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	modelData->modelMatrix = o->GetTransform()->GetMatrix();
	modelData->invModelMatrix = modelData->modelMatrix.Inverse();

	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(modelData, sizeof(ModelInfo));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	PS4Shader* realShader = defaultShader;
	PS4Mesh*	realMesh	= (PS4Mesh*)o->GetMesh();

	int objIndex = realShader->GetConstantBufferIndex(Gnm::kShaderStageVs, "RenderObjectData");
	int camIndexVs = realShader->GetConstantBufferIndex(Gnm::kShaderStageVs, "CameraData");
	int camIndexPs = realShader->GetConstantBufferIndex(Gnm::kShaderStagePs, "CameraData");
	int lightIndex = realShader->GetConstantBufferIndex(Gnm::kShaderStagePs, "LightData");

	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &constantBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndexVs, 1, &cameraBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStagePs, camIndexPs, 1, &cameraBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStagePs, lightIndex, 1, &lightBuffer);

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
