#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class FireRate : public PowerUp {
		public:
			FireRate(GameWorld& gw, const float& increaseFireRateBy = 0.5f) : PowerUp(PowerUpType::FireRate, gw),FireRateIncrease(increaseFireRateBy){}
			~FireRate() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (otherObject->GetName() == "Player") {
					//std::cout << "so manyyy" << std::endl;
					((Player*)otherObject)->IncreaseFireRate(FireRateIncrease, powerupDuration);
					gameWorld.RemoveGameObject(this, true);
				}
			}

		private:
			float FireRateIncrease;
		};
	}
}