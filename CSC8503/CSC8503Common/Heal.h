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
				if (otherObject->GetName() == "Player") {
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseHealth(healthTake);
					pickedUp = true;
					gameWorld.RemoveGameObject(this);
				}
			}

		private:
			float healthTake;
		};
	}
}