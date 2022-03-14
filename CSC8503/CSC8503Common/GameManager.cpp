#include "GameManager.h"

void GameManager::Update(const float& dt) {
	timer->Update(dt);

	if (timer->GetState() == TimerStates::Ended) {
		if (Teams[A].Score > Teams[B].Score)
			Debug::Print("Team A Wins", { 30.f, 70.f });
		else if (Teams[A].Score < Teams[B].Score)
			Debug::Print("Team B Wins", { 30.f, 70.f });
		else
			Debug::Print("Tie", { 30.f, 70.f });
	}
}
