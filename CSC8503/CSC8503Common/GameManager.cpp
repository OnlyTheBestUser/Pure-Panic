#include "GameManager.h"
#include "../GameTech/TutorialGame.h"

using namespace NCL::Maths;
Vector4 GameManager::team1Colour = Vector4(1, 0.039, 0.941, 1);
Vector4 GameManager::team2Colour = Vector4(0.011, 0.988, 0.941, 1);

void GameManager::Update(const float& dt) {
	timer->Update(dt);

	if (printResults) PrintResult(Teams[A].score, Teams[B].score);
	if (printScores)  PrintScores(Teams[A].score, Teams[B].score);
}

void GameManager::PrintScores(float scoreA, float scoreB) {
	Debug::Print(std::to_string((int)scoreA), { 30.f, 80.f }, 15, Vector4(0.3, 0, 0.5, 1));
	Debug::Print(std::to_string((int)scoreB), { 50.f, 80.f }, 15, Vector4(0.25, 0.878, 0.815, 1));

	Vector2 ratio = CalcCurrentScoreRatio();

	Debug::Print(std::to_string(ratio.x), { 30.f, 85.f }, 15, Vector4(1, 1, 1, 1));
	Debug::Print(std::to_string(ratio.y), { 50.f, 85.f }, 15, Vector4(1, 1, 1, 1));
}

void GameManager::PrintResult(float scoreA, float scoreB) {
	if (scoreA > scoreB)
		Debug::Print("Purple Wins", { 30.f, 70.f });
	else if (scoreA < scoreB)
		Debug::Print("Blue Wins", { 30.f, 70.f });
	else
		Debug::Print("Tie", { 30.f, 70.f });
}

void GameManager::PlacePlayersToSpawnPositions() {
	for (int i = 0; i < NO_OF_TEAMS; ++i) {
		auto teamPlayers = Teams[i].TeamPlayers;
		for (auto playerPtrIt = teamPlayers.begin(); playerPtrIt != teamPlayers.end(); ++playerPtrIt) {
			(*playerPtrIt)->Respawn();
		}
	}
}

Vector4 GameManager::GetColourForID(int playerID) {
	return playerID % 2 == 0 ? GameManager::team1Colour : GameManager::team2Colour;
}

void GameManager::UpdateScores(Vector2 scores) {
	Teams[0].score += scores.x;
	Teams[1].score += scores.y;
}

void GameManager::SetScores(Vector2 scores) {
	Teams[0].score = scores.x;
	Teams[1].score = scores.y;
}

Vector2 GameManager::CalcCurrentScoreRatio() {
	float tempTotal = Teams[0].score + Teams[1].score;
	return (tempTotal == 0) ? Vector2(0,0) : Vector2(Teams[0].score / tempTotal, Teams[1].score / tempTotal);
}

void GameManager::HandleScoresAfterRound() {
	Teams[0].score += 0.1f;
	Teams[1].score += 0.1f;
}

void GameManager::StartRound() {
	PlacePlayersToSpawnPositions();
	HandleScoresAfterRound();
	timer->StartTimer();
}