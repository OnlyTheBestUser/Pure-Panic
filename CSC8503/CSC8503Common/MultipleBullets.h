#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class MultipleBullets : public PowerUp {
		public:
			MultipleBullets(GameWorld& gw, const float& bps = 10.0f) : PowerUp(PowerUpType::MultipleBullets, gw), bulletsPerShot(bps) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				std::cout << "Weapons Freeeee" << std::endl;
				if (otherObject->GetName() == "Player") {
					Player* player = ((Player*)otherObject);
					if (player->GetCurrentPowerup() != PowerUpType::None) return;
					
					gameWorld.RemoveGameObject(this, true);
				}
			}

		private:
			float bulletsPerShot;
		};
	}
}