#ifdef _ORBIS
#include "ExampleRenderer.h"
#include "../CSC8503Common/RenderObject.h"
#include "../CSC8503Common/Transform.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
#include "../../Plugins/PlayStation4/PS4Shader.h"
#include "../../Plugins/PlayStation4/PS4Texture.h"
#include "../../Common/Window.h"

using namespace NCL;
using namespace NCL::PS4;

ExampleRenderer::ExampleRenderer(GameWorld& world) : PS4RendererBase(*Window::GetWindow()), gameWorld(world) {
	rotation	= 0.0f;
	translation = 0.0f;

	defaultShader = PS4Shader::GenerateShader(
		"/app0/Assets/Shaders/PS4/VertexShader.sb",
		"/app0/Assets/Shaders/PS4/PixelShader.sb"
	);

	defaultMesh	= PS4Mesh::GenerateTriangle();
	defaultMesh->UploadToGPU(this);
	defaultTexture	= PS4Texture::LoadTextureFromFile("/app0/Assets/Textures/doge.gnf");

	CSC8503::Transform* newTransform = new CSC8503::Transform();
	CSC8503::Transform* newTransform2 = new CSC8503::Transform();
	defaultObject[0] = new CSC8503::RenderObject(newTransform,(MeshGeometry*)defaultMesh, (TextureBase*)defaultTexture, (ShaderBase*)defaultShader);
	defaultObject[1] = new CSC8503::RenderObject(newTransform2, (MeshGeometry*)defaultMesh, (TextureBase*)defaultTexture, (ShaderBase*)defaultShader);

	viewProjMat = (Matrix4*)onionAllocator->allocate(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*viewProjMat = Matrix4();

	cameraBuffer.initAsConstantBuffer(viewProjMat, sizeof(Matrix4));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK
}

ExampleRenderer::~ExampleRenderer()
{
	delete defaultMesh;
	delete defaultTexture;
	delete defaultShader;

	delete defaultObject[0];
	delete defaultObject[1];
}

void ExampleRenderer::Update(float dt)	{
	rotation	+= 30.0f * dt;
	translation += dt;

	(defaultObject[0])->GetTransform()
		->SetPosition(Vector3(-0.4, sin(translation), 0));


	defaultObject[1]->GetTransform()
		->SetPosition(Vector3(0.4, 0, sin(translation)));
}

void ExampleRenderer::RenderFrame() {
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	defaultShader->SubmitShaderSwitch(*currentGFXContext);

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

	currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	*viewProjMat = Matrix4();
	*viewProjMat = Matrix4::Perspective(1.0f, 1000.0f, (float)currentWidth / (float)currentHeight, 45.0f) * Matrix4::Translation(Vector3(0, 0, -2));

	BuildObjectList();
	RenderCamera();

	currentFrame->EndFrame();
}


void ExampleRenderer::DrawRenderObject(const CSC8503::RenderObject* o) {
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

void ExampleRenderer::BuildObjectList() {
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

void ExampleRenderer::RenderCamera() {
	for (const auto& i : activeObjects) {
		DrawRenderObject((i));
	}
}
#endif
