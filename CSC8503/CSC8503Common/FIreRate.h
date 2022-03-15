#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class FireRate : public PowerUp {
		public:
			FireRate(GameWorld& gw, const float& increaseFireRateFactor = 0.5f) : PowerUp(PowerUpType::FireRate, gw),FireRateIncrease(increaseFireRateFactor){}
			~FireRate() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (!IsPicked && otherObject->GetName() == "Player") {
					
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseFireRate(FireRateIncrease, powerupDuration);
					
					IsPicked = true;// gameWorld.RemoveGameObject(this, true);
					reappearAfter = REAPPEAR_AFTER_DURATION;
					
					GetRenderObject()->SetVisibility(0.f);
				}
			}

		private:
			float FireRateIncrease;
		};
	}
}