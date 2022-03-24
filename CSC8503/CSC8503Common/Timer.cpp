#include "Timer.h"
#include "Debug.h"
#include <algorithm>

using namespace NCL;
using namespace CSC8503;

void Timer::Update(const float& dt) {
	if (state == TimerStates::Running) {
		timeLeft = std::max(0.0f, timeLeft - dt);
		if (timeLeft == 0.0f) {
			state = TimerStates::Ended;
		}
	}
	else if (state == TimerStates::Ended) {
	}
	Debug::Print(std::to_string(int(timeLeft)), Vector2(92.0f, 10.0f), 30.0f, Debug::BLACK);
}

void Timer::StartTimer() {
	if (state == Ended)
		timeLeft = maxTime;
	state = TimerStates::Running;
}