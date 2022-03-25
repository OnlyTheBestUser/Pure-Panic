#include "TrainingGame.h"
#include "../../Common/Assets.h"

using namespace NCL;
using namespace CSC8503;

TrainingGame::TrainingGame()
{

	GameObject* x = LevelLoader::SpawnAIEnemy(Vector3(20, 3, 20), player1);
	x->GetPhysicsObject()->SetGravity(false);
	x->GetPhysicsObject()->SetLinearDamping(0.6f);
	x->GetPhysicsObject()->SetFriction(false);

	gameManager->GetTimer()->StartTimer();
}