#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class SpeedBoost : public PowerUp
		{
		public:
			SpeedBoost(float speedIncreasedBy = 1.5f) : PowerUp(PowerUpType::SpeedBoost), increaseInSpeed(speedIncreasedBy) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (!IsPicked && otherObject->GetName() == "Player") {
					
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseSpeed(increaseInSpeed, powerupDuration);
					
					PickUp();
				}
			}
		private:
			float increaseInSpeed;
		};
	}
}