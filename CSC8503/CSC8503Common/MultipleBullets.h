#pragma once
#include"PowerUp.h"
#include"Player.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class MultipleBullets : public PowerUp {
		public:
			MultipleBullets(GameWorld& gw, const float& increaseBulletsBy = 10.0f) : PowerUp(PowerUpType::MultipleBullets, gw), BulletsPerShot(increaseBulletsBy) {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				std::cout << "Weapons Freeeee" << std::endl;
				/*if (otherObject->GetName() == "Player") {
					((Player*)otherObject)->IncreaseFireRate(BulletsIncrease, powerupDuration);
					gameWorld.RemoveGameObject(this, true);
				}*/
			}

		private:
			float BulletsPerShot;
		};
	}
}