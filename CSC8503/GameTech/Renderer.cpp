#include "Renderer.h"
#include "OGLGameRenderer.h"
#include "PS4GameRenderer.h"
using namespace NCL;
using namespace Rendering;

Renderer::Renderer(GameWorld& world) {
#ifdef _WIN64
	rendererAPI = new OGLGameRenderer(world);
#endif
#ifdef _ORBIS
	rendererAPI = new PS4::PS4GameRenderer(world);
#endif
}

Renderer::~Renderer() {
	delete rendererAPI;
}

void Renderer::Update(float dt) {

}

void Renderer::Render() {
	rendererAPI->Render();
}