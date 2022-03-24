#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class FireRate : public PowerUp {
		public:
			FireRate(const float& increaseFireRateFactor = 0.5f) : PowerUp(PowerUpType::FireRate),FireRateIncrease(increaseFireRateFactor){}
			~FireRate() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (!IsPicked && otherObject->GetName() == "Player") {
					
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseFireRate(FireRateIncrease, powerupDuration);
					
					IsPicked = true;// gameWorld.RemoveGameObject(this, true);
					reappearAfter = REAPPEAR_AFTER_DURATION;
					
					GetRenderObject()->SetVisibility(false);
				}
			}

		private:
			float FireRateIncrease;
		};
	}
}