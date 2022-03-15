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
				if (!IsPicked && otherObject->GetName() == "Player") {
					
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->IncreaseHealth(healthTake);

					IsPicked = true;// gameWorld.RemoveGameObject(this, true);
					reappearAfter = REAPPEAR_AFTER_DURATION;

					GetRenderObject()->SetVisibility(0.f);
				}
			}

		private:
			float healthTake;
		};
	}
}