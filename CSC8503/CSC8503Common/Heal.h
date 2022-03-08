#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class Heal : public PowerUp {
		public:
			Heal(GameWorld& gw, const float& HealthCure = 10.0f) : PowerUp(PowerUpType::Heal, gw), HealthTake(HealthCure) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				std::cout << "Ahhh thats good" << std::endl;
				/*if (otherObject->GetName() == "Player") {
					((Player*)otherObject)->IncreaseFireRate(FireRateIncrease, powerupDuration);
					gameWorld.RemoveGameObject(this, true);
				}*/
			}

		private:
			float HealthTake;
		};
	}
}