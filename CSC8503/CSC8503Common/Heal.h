#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class Heal : public PowerUp {
		public:
			Heal(GameWorld& gw, const float& healthCure = 10.0f) : PowerUp(PowerUpType::Heal, gw), healthTake(healthCure) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				std::cout << "Ahhh thats good" << std::endl;
				if (otherObject->GetName() == "Player") {
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseHealth(healthTake);
					gameWorld.RemoveGameObject(this, true);
				}
			}

		private:
			float healthTake;
		};
	}
}