#pragma once
#include"PowerUp.h"
#include"Player.h"

namespace NCL {
	namespace CSC8503 {
		class FireRate : public PowerUp {
		public:
			FireRate(float increaseFireRateBy = 10.0f) : PowerUp(PowerUpType::FireRate),FireRateIncrease(increaseFireRateBy){}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (otherObject->GetName() == "Player") {
					((Player*)otherObject)->IncreaseFireRate(FireRateIncrease, powerupDuration);
				}
			}

		private:
			float FireRateIncrease;
		};
	}
}