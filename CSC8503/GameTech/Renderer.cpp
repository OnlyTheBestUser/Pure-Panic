#ifdef _ORBIS
#include "../../Plugins/PlayStation4/PS4Mesh.h"
#include "../../Plugins/PlayStation4/PS4Shader.h"
#include "../../Plugins/PlayStation4/PS4Texture.h"
#include "../../Plugins/PlayStation4/PS4UniformBuffer.h"
#include "../../Plugins/PlayStation4/PS4FrameBuffer.h"
#endif
#include "Renderer.h"

#include "../../Common/Assets.h"
#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/MeshGeometry.h"


#ifdef _WIN64
#include "../../Plugins/OpenGLRendering/OGLFrameBuffer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLUniformBuffer.h"
#endif
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

	// Uniform block bindings
	camBuffer = new OGLUniformBuffer(sizeof(CameraMatrix), 0);

#endif
#ifdef _ORBIS
	skyboxMesh = PS4::PS4Mesh::GenerateQuad();
	skyboxMesh->UploadToGPU(rendererAPI);

	skyboxShader = PS4::PS4Shader::GenerateShader(
		Assets::SHADERDIR + "PS4/skyboxVertex.sb",
		Assets::SHADERDIR + "PS4/skyboxPixel.sb"
	);

	skyboxTex = PS4::PS4Texture::LoadSkyboxFromFile(NCL::Assets::TEXTUREDIR + "Cubemap/cubemap.gnf");

	maskShader = PS4::PS4Shader::GenerateShader(
		Assets::SHADERDIR + "PS4/maskVertex.sb",
		Assets::SHADERDIR + "PS4/maskPixel.sb"
	);

	camBuffer = new PS4::PS4UniformBuffer(sizeof(CameraMatrix));
#endif

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPos = Vector3(-200.0f, 60.0f, -200.0f);
}

Renderer::~Renderer() {
	delete camBuffer;

	delete shadowFBO;
	delete shadowShader;

	delete skyboxShader;
	delete skyboxMesh;
	delete skyboxTex;

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

	ApplyPaintToMasks();
	RenderScene();
	rendererAPI->SetCullFace(false);

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

	bool a = true;
}



void Renderer::SortObjectList() {
	//Who cares!
}

void Renderer::RenderScene() {
#ifdef _WIN64
	RenderShadows();
#endif
	// Set scene uniform buffers
	float screenAspect = (float)rendererAPI->GetCurrentWidth() / (float)rendererAPI->GetCurrentHeight();
	camMatrix.projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);
	camMatrix.viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	camBuffer->SetData(&camMatrix, sizeof(CameraMatrix));

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

	skyboxShader->BindShader();

	skyboxShader->UpdateUniformMatrix4("invProjMatrix", camMatrix.projMatrix.Inverse());

	skyboxTex->Bind(0);

	rendererAPI->DrawMesh(skyboxMesh);

	rendererAPI->SetCullFace(true);
	rendererAPI->SetBlend(true);
	rendererAPI->SetDepth(true);
}

void Renderer::RenderObjects() {
	ShaderBase* activeShader = nullptr;

	for (const auto& i : activeObjects) {
		ShaderBase* shader = (*i).GetShader();

		if (activeShader != shader) {
			shader->BindShader();

			shader->UpdateUniformMatrix4("viewProjMatrix", camMatrix.projMatrix * camMatrix.viewMatrix);
			shader->UpdateUniformMatrix4("invProjMatrix", camMatrix.projMatrix.Inverse());
			shader->UpdateUniformVector3("cameraPos", gameWorld.GetMainCamera()->GetPosition());

			shader->UpdateUniformVector4("lightColour", lightColour);
			shader->UpdateUniformVector3("lightPos", lightPos);
			shader->UpdateUniformFloat( "lightRadius", lightRadius);

			activeShader = shader;
		}

		if ((*i).GetDefaultTexture()) (*i).GetDefaultTexture()->Bind(0);
		shader->UpdateUniformInt("hasTexture", (*i).GetDefaultTexture() ? 1 : 0);
#ifdef _WIN64
		if (shadowFBO->GetTexture()) shadowFBO->GetTexture()->Bind(1);
#endif
		if ((*i).GetPaintMask()) {
			(*i).GetPaintMask()->Bind(2);
		}
		shader->UpdateUniformInt("hasPaintMask", (*i).GetPaintMask() ? 1 : 0);


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
}

void Renderer::Paint(const RenderObject* paintable, Maths::Vector3& barycentric, Maths::Vector3& colpos, Maths::Vector2& texUV_a, Maths::Vector2& texUV_b, Maths::Vector2& texUV_c, float radius, float hardness, float strength, NCL::Maths::Vector4 colour)
{
	PaintInstance pi;
	pi.object = paintable;
	pi.barycentric = barycentric;
	pi.colPoint = colpos;
	pi.texUV_a = texUV_a;
	pi.texUV_b = texUV_b;
	pi.texUV_c = texUV_c;
	pi.radius = radius;
	pi.hardness = hardness;
	pi.strength = strength;
	pi.colour = colour;

	paintInstances.push_back(pi);
}

void Renderer::ApplyPaintToMasks() {
	rendererAPI->SetDepth(false);
	rendererAPI->SetBlend(true, RendererAPI::BlendType::ONE, RendererAPI::BlendType::ALPHA);

	maskShader->BindShader();

#ifdef _ORBIS
	PS4::PS4RendererAPI* rend = ((PS4::PS4RendererAPI*)NCL::Rendering::RendererBase::rendererAPI);
#endif
	Vector2 currentSize;
	for (const auto& i : paintInstances) {
		if (i.object->GetPaintMask() == nullptr) continue;
#ifdef _WIN64
		OGLFrameBuffer maskFBO;
		maskFBO.AddTexture((OGLTexture*)(i.object->GetPaintMask()));
#endif
#ifdef _ORBIS
		//PS4::PS4FrameBuffer maskFBO;
		//maskFBO.AddTexture(i.object->GetPaintMask());
		PS4::PS4Texture* ps4Tex = static_cast<PS4::PS4Texture*>(i.object->GetPaintMask());
		ps4Tex->Bind(0);
#endif
		if (Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight()) != currentSize) {
			currentSize = Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight());
			rendererAPI->SetViewportSize(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight());
		}

		// Update uniforms here
		maskShader->UpdateUniformMatrix4("modelMatrix", i.object->GetTransform()->GetMatrix());
		maskShader->UpdateUniformVector3("barycentricCoord", i.barycentric);
		maskShader->UpdateUniformVector3("collisionPoint", i.colPoint);
		maskShader->UpdateUniformVector2("nearTexCoord_a", i.texUV_a);
		maskShader->UpdateUniformVector2("nearTexCoord_b", i.texUV_b);
		maskShader->UpdateUniformVector2("nearTexCoord_c", i.texUV_c);

		Vector2 pos = i.texUV_a * i.barycentric.x 
			+ i.texUV_b * i.barycentric.y 
			+ i.texUV_c * i.barycentric.z;
		maskShader->UpdateUniformVector2("uvHitPoint", pos);

		maskShader->UpdateUniformVector2("viewport", Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight()));

		maskShader->UpdateUniformVector2("textureSize", Vector2(i.object->GetPaintMask()->GetWidth(), i.object->GetPaintMask()->GetHeight()));
		maskShader->UpdateUniformVector3("textureScale", i.object->GetTransform()->GetScale());
		maskShader->UpdateUniformFloat("radius", 5.0f);
		maskShader->UpdateUniformFloat("hardness", i.hardness);
		maskShader->UpdateUniformFloat("strength", i.strength);
		maskShader->UpdateUniformVector4("colour", i.colour);


#ifndef _ORBIS
		rendererAPI->BindFrameBuffer(&maskFBO);
#endif
		rend->SetPaintBuffer(ps4Tex->target);

		uint64_t textureSizeInBytes;
		Gnm::AlignmentType textureAlignment;
		GpuAddress::computeTotalTiledTextureSize(&textureSizeInBytes, &textureAlignment, &ps4Tex->GetAPITexture());

		rendererAPI->SetCullFace(false);
		rendererAPI->DrawMesh(skyboxMesh);

		rend->currentGFXContext->waitForGraphicsWrites(
			ps4Tex->GetAPITexture().getBaseAddress256ByteBlocks(),
			(textureSizeInBytes + 255) / 256,
			Gnm::kWaitTargetSlotCb1,
			Gnm::kCacheActionWriteBackAndInvalidateL1andL2,
			Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
			Gnm::kStallCommandBufferParserDisable
		);

	}
	rendererAPI->SetBlend(false, RendererAPI::BlendType::ONE, RendererAPI::BlendType::NONE);
	rendererAPI->SetDepth(true);
	rendererAPI->SetViewportSize(rendererAPI->GetCurrentWidth(), rendererAPI->GetCurrentHeight());
#ifdef _ORBIS
	rend->SetRenderBuffer(rend->screenBuffers[rend->currentScreenBuffer], true, false, false);
#else
	rendererAPI->BindFrameBuffer();
#endif
	rendererAPI->ClearBuffer(true, true, true);
	
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

