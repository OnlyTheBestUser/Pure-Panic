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
			Timer(float totalDuration) : maxTime(totalDuration), timeLeft(totalDuration) {}
			void StartTimer();
			void PauseTimer() { state = TimerStates::Paused; }
			void ResetTimer() { state = TimerStates::NotStarted; timeLeft = maxTime; }
			void Update(const float& dt);
			float GetTime() const {
				return timeLeft;
			}
			TimerStates GetState() const {
				return state;
			}
		private:
			float timeLeft;
			float maxTime;
			TimerStates state = TimerStates::NotStarted;
		};
	}
}