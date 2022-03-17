#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class MultiBullet : public PowerUp {
		public:
			MultiBullet(const int& NoOfBullets = 5) : PowerUp(PowerUpType::MultiBullet), BulletsPerShot(NoOfBullets) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (!IsPicked && otherObject->GetName() == "Player") {

					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->ActivateMultiBullet(BulletsPerShot, powerupDuration);

					IsPicked = true;
					reappearAfter = REAPPEAR_AFTER_DURATION;

					GetRenderObject()->SetVisibility(false);
				}
			}

		private:
			int BulletsPerShot;
		};
	}
}