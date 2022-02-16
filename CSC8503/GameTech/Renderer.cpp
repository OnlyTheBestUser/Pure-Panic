#include "PS4GameRenderer.h"
#include "Renderer.h"
#include "OGLGameRenderer.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/MeshGeometry.h"
#include "../../Plugins/PlayStation4/PS4Mesh.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

Renderer::Renderer(GameWorld& world) : RendererBase() {
	gameWorld = world;
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
}

Renderer::~Renderer() {
	delete rendererAPI;
	delete font;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->BeginFrame();
	rendererAPI->RenderFrame();
	rendererAPI->EndFrame();
	DrawDebugData();
	rendererAPI->SwapBuffers();
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