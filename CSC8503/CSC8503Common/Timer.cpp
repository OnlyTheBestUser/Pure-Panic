#include "Timer.h"
#include "Debug.h"
#include <algorithm>

using namespace NCL::CSC8503;

void Timer::Update(const float& dt) {
	if (state == TimerStates::Running) {
		timeLeft = std::max(0.0f, timeLeft - dt);
		if (timeLeft == 0.0f) {
			state = TimerStates::Ended;
		}
	}
	else if (state == TimerStates::Ended) {
		Debug::Print("Game Ended", Vector2(30.0f, 50.0f), 50.0f, Debug::RED);
	}
	Debug::Print(std::to_string(int(timeLeft)), Vector2(92.0f, 10.0f), 30.0f, Debug::BLACK);
}