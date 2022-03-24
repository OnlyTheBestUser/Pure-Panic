#include "LoadingScreen.h"
using namespace NCL;
using namespace CSC8503;

LoadingScreen* LoadingScreen::singleton = nullptr;

LoadingScreen::LoadingScreen()
{
	singleton = this;
	instancesToLoad = 1;
	world = new GameWorld();
	completed = false;
	renderer = new Renderer(*world);
	Debug::SetRenderer(renderer);
}
LoadingScreen::~LoadingScreen()
{
	delete renderer;
	delete world;
}

void LoadingScreen::UpdateGame(float dt)
{
	Debug::SetRenderer(singleton->renderer);

	singleton->renderer->DrawString("Loading: " + std::to_string(singleton->progression / singleton->instancesToLoad) + "%", Vector2(30, 50), Debug::WHITE, 30.0f);

	Debug::FlushRenderables(dt);
	singleton->renderer->Render();
}
