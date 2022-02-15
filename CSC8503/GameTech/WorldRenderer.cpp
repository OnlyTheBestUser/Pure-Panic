#include "WorldRenderer.h"
#include "OGLGameRenderer.h"
#include "PS4GameRenderer.h"
#include "../../Common/TextureLoader.h"
using namespace NCL;
using namespace Rendering;

WorldRenderer::WorldRenderer(GameWorld& world) : gameWorld(world) {
#ifdef _WIN64
	renderer = new OGLRenderer(*Window::GetWindow());
#endif
#ifdef _ORBIS
	renderer = new PS4::PS4GameRenderer(*world);
#endif
	defaultMesh = new OGLMesh();
	defaultTex = (OGLTexture*)NCL::TextureLoader::LoadAPITexture("checkerboard.png");
	defaultShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
}

WorldRenderer::~WorldRenderer() {
	delete renderer;
}

void WorldRenderer::Update(float dt) {
	renderer->Update(dt);
}

void WorldRenderer::Render() {
	renderer->BeginFrame();
	RenderScene();
	renderer->EndFrame();
	renderer->SwapBuffers();
}

void WorldRenderer::RenderScene() {
	renderer->RenderFrame();
}

void WorldRenderer::BuildObjectList() {
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

void WorldRenderer::SortObjectList() {

}