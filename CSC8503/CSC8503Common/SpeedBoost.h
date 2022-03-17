#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class SpeedBoost : public PowerUp
		{
		public:
			SpeedBoost(GameWorld& gw, float speedIncreasedBy = 1.5f) : PowerUp(PowerUpType::SpeedBoost, gw), increaseInSpeed(speedIncreasedBy) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (otherObject->GetName() == "Player") {
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseSpeed(increaseInSpeed, powerupDuration);
					pickedUp = true;
					gameWorld.RemoveGameObject(this);
				}
			}
		private:
			float increaseInSpeed;
		};
	}
}