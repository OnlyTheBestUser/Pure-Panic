#include "LoadingScreen.h"
using namespace NCL;
using namespace CSC8503;

LoadingScreen* LoadingScreen::singleton = nullptr;

LoadingScreen::LoadingScreen()
{
	singleton = this;
	world = new GameWorld();
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
	
	singleton->renderer->DrawString("Loading...", Vector2(30, 50), Debug::WHITE, 30.0f);
	std::cout << "Loading: " + std::to_string(singleton->progression) + "%" << std::endl;
	singleton->renderer->Update(dt);

	Debug::FlushRenderables(dt);
	singleton->renderer->Render();
}
