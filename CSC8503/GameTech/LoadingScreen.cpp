#include "LoadingScreen.h"
#include <Windows.h>

using namespace NCL;
using namespace CSC8503;

LoadingScreen::LoadingScreen()
{
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
	//wglMakeCurrent(wglGetCurrentDC(), wglGetCurrentContext());
	Debug::SetRenderer(renderer);
	progression += dt * 10;
	renderer->DrawString("Loading: " + std::to_string(progression) + "%", Vector2(30, 50), Debug::WHITE, 30.0f);
	std::cout << "LOADING: " << progression << "%" << std::endl;

	renderer->Update(dt);
	Debug::FlushRenderables(dt);
	renderer->Render();
}
