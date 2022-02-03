#include "MainMenu.h"
#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/PhysicsSystem.h"
using namespace NCL;
using namespace CSC8503;

void MainMenu::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();
	world->GetMainCamera()->LockCamera();
	//InitDefaultFloor();
}

void MainMenu::UpdateGame(float dt)
{
	Debug::SetRenderer(renderer);

	renderer->DrawString("Screwball Scramble", Maths::Vector2(22, 30), Debug::MAGENTA, 30.0f);
	renderer->DrawString("Press 1 - Screwball Scramble", Maths::Vector2(30, 50), Debug::WHITE, 20.0f);
	renderer->DrawString("Press 2 - Evil Maze", Maths::Vector2(30, 55), Debug::WHITE, 20.0f);
	renderer->DrawString("Press 3 - Physics Test", Maths::Vector2(30, 60), Debug::WHITE, 20.0f);
	renderer->DrawString("Press Escape to Exit", Maths::Vector2(29, 65), Debug::WHITE, 20.0f);

	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}
