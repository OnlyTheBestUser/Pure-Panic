#include "GameManager.h"
#include "../GameTech/TutorialGame.h"

void GameManager::Update(const float& dt) {
	timer->Update(dt);

	if (timer->GetState() == TimerStates::Ended) {
		if (Teams[A].RoundScore > Teams[B].RoundScore)
			Debug::Print("Team A Wins", { 30.f, 70.f });
		else if (Teams[A].RoundScore < Teams[B].RoundScore)
			Debug::Print("Team B Wins", { 30.f, 70.f });
		else
			Debug::Print("Tie", { 30.f, 70.f });
	}
}

void GameManager::PlacePlayersToSpawnPositions() {
	for (int i = 0; i < NO_OF_TEAMS; ++i) {
		auto teamPlayers = Teams[i].TeamPlayers;
		for (auto playerPtrIt = teamPlayers.begin(); playerPtrIt != teamPlayers.end(); ++playerPtrIt) {
			(*playerPtrIt)->Respawn();
		}
	}
}

void GameManager::HandleScoresAfterRound() {
	Teams[A].TotalScore += Teams[A].RoundScore;
	Teams[B].TotalScore += Teams[B].RoundScore;

	Teams[A].RoundScore = 0.f;
	Teams[B].RoundScore = 0.f;
}

void GameManager::StartRound() {
	game->ResetGame();
	PlacePlayersToSpawnPositions();
	HandleScoresAfterRound();
	timer->StartTimer();
}