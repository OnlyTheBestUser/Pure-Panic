#include "Timer.h"
#include "Debug.h"
#include <algorithm>

using namespace NCL::CSC8503;

void Timer::Update(const float& dt) {
	if (state == TimerStates::Running) {
		timeLeft = std::max(0.0f, timeLeft - dt);
		if (timeLeft == 0.0f) state = TimerStates::Ended;
	}
	Debug::Print(std::to_string(int(timeLeft)), Vector2(92.0f, 10.0f), 30.0f, Vector4(0,0,0,1));
}