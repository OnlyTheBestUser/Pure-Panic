#pragma once
#include"PowerUp.h"
#include"Player.h"

namespace NCL {
	namespace CSC8503 {
		class SpeedBoost : public PowerUp
		{
		public:
			SpeedBoost(float speedIncreasedBy = 20.0f) : PowerUp(PowerUpType::SpeedBoost), increaseInSpeed(speedIncreasedBy) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (otherObject->GetName() == "Player") {
					((Player*)otherObject)->IncreaseSpeed(increaseInSpeed, powerupDuration);
				}
			}
		private:
			float increaseInSpeed;
		};
	}
}