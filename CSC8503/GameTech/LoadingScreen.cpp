#include "LoadingScreen.h"
using namespace NCL;
using namespace CSC8503;

LoadingScreen::LoadingScreen()
{
	world = new GameWorld();
	renderer = new Renderer(*world);
	Debug::SetRenderer(renderer);

	InitCamera();
	InitWorld();
}
LoadingScreen::~LoadingScreen()
{
	delete renderer;
	delete world;
}

void LoadingScreen::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}
void LoadingScreen::InitWorld()
{
	world->ClearAndErase();
	world->GetMainCamera()->LockCamera();
}

void LoadingScreen::UpdateGame(float dt)
{
	Debug::SetRenderer(renderer);
	renderer->DrawString("Loading", Vector2(22, 30), Debug::MAGENTA, 30.0f);
	std::cout << "LOADING" << std::endl;

	renderer->Update(dt);
	Debug::FlushRenderables(dt);
	renderer->Render();
}
