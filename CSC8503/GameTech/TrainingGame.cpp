#include "TrainingGame.h"

TrainingGame::TrainingGame()
{
	GameObject* x = LevelLoader::SpawnAIEnemy(Vector3(20, 5, 20), player1);
	x->GetPhysicsObject()->SetGravity(false);
	x->GetPhysicsObject()->SetLinearDamping(0.6f);
	x->GetPhysicsObject()->SetFriction(false);
}