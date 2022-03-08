#pragma once
#include"PowerUp.h"
#include"Player.h"

namespace NCL {
	namespace CSC8503 {
		class SpeedBoost : public PowerUp
		{
		public:
			SpeedBoost(GameWorld& gw, float speedIncreasedBy = 20.0f) : PowerUp(PowerUpType::SpeedBoost, gw), increaseInSpeed(speedIncreasedBy) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (otherObject->GetName() == "Player") {
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseSpeed(increaseInSpeed, powerupDuration);
				}
			}
		private:
			float increaseInSpeed;
		};
	}
}