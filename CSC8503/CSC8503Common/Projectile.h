#pragma once
#include "GameWorld.h"
#include "GameObject.h"

namespace NCL {

	namespace CSC8503 {

		class Projectile : public GameObject
		{
		public:
			Projectile(GameWorld& gWorld, const float& damageValue = 10.0f, const int& playerID = 0): GameObject("projectile"), gameWorld(gWorld), damagePerShot(damageValue), ownerPlayerID(playerID) {
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				gameWorld.RemoveGameObject(this, true);
			}

			float GetDamage() const { return damagePerShot; }

			int GetOwnerPlayerID() const { return ownerPlayerID; }

			void Update(float dt) override;

		private:
			//Vector3 velocity;
			GameWorld& gameWorld;
			int ownerPlayerID;
			float damagePerShot;
		};
	}
}
