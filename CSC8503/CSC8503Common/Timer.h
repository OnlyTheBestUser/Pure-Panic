#pragma once

namespace NCL {
	namespace CSC8503 {

		enum TimerStates
		{
			NotStarted,
			Running,
			Paused,
			Ended,
		};

		class Timer {
		public:
			Timer(float totalDuration) : timeLeft(totalDuration) {}
			void StartTimer() { state = TimerStates::Running; };
			void PauseTimer() { state = TimerStates::Paused; };
			void Update(const float& dt);
			float GetTime() const {
				return timeLeft;
			}
			TimerStates GetState() const {
				return state;
			}
		private:
			float timeLeft;
			TimerStates state = TimerStates::NotStarted;
		};
	}
}