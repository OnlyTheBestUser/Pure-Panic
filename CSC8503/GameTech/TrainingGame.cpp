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

void TrainingGame::UpdateWinScreen(float dt)
{
	won = true;

	if (gameManager->GetScores().x > gameManager->GetScores().y) {
		renderer->DrawString("YOU WIN", Vector2(5, 80), Debug::MAGENTA, 30.0f);
		if (!finalSoundPlayed)
			NCL::AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + "win.ogg");
		finalSoundPlayed = true;
	}
	else {
		renderer->DrawString("YOU LOSE", Vector2(5, 80), Debug::MAGENTA, 30.0f);
		if (!finalSoundPlayed)
			NCL::AudioManager::GetInstance()->StartPlayingSound(Assets::AUDIODIR + "lose.ogg");
		finalSoundPlayed = true;
	}
	renderer->DrawString("Press F1 to Restart.", Vector2(5, 90), Debug::WHITE, 20.0f);
	renderer->DrawString("Press Esc to return to Main Menu.", Vector2(5, 95), Debug::WHITE, 20.0f);
}