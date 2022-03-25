#include "TrainingGame.h"
#include "../../Common/Assets.h"
#include "../CSC8503Common/InputHandler.h"

using namespace NCL;
using namespace CSC8503;

TrainingGame::TrainingGame()
{
	Command* resetWorld = new ResetWorldCommand(&state);
	inputHandler->BindButton(RESET_WORLD, resetWorld);
	GameObject* x = LevelLoader::SpawnAIEnemy(Vector3(20, 3, 20), player1);
	x->GetPhysicsObject()->SetGravity(false);
	x->GetPhysicsObject()->SetLinearDamping(0.6f);
	x->GetPhysicsObject()->SetFriction(false);
	
	gameManager->GetTimer()->StartTimer();
}

void TrainingGame::InitWorld() {
	TutorialGame::InitWorld();
	GameObject* x = LevelLoader::SpawnAIEnemy(Vector3(20, 3, 20), player1);
	x->GetPhysicsObject()->SetGravity(false);
	x->GetPhysicsObject()->SetLinearDamping(0.6f);
	x->GetPhysicsObject()->SetFriction(false);
	
	gameManager->GetTimer()->StartTimer();
}