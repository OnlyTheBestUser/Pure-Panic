#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class MultiBullet : public PowerUp {
		public:
			MultiBullet(GameWorld& gw, const int& NoOfBullets = 5) : PowerUp(PowerUpType::MultiBullet, gw), BulletsPerShot(NoOfBullets) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				if (!IsPicked && otherObject->GetName() == "Player") {

					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					((Player*)otherObject)->ActivateMultiBullet(BulletsPerShot, powerupDuration);

					IsPicked = true;// gameWorld.RemoveGameObject(this, true);
					reappearAfter = REAPPEAR_AFTER_DURATION;

					GetRenderObject()->SetVisibility(0.f);
				}
			}

		private:
			int BulletsPerShot;
		};
	}
}