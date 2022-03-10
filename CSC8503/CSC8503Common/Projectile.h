#pragma once
#include "GameWorld.h"
#include "GameObject.h"

namespace NCL {
	namespace Rendering {
		class Renderer;
	}
	namespace CSC8503 {

		class Projectile : public GameObject
		{
		public:
			Projectile(GameWorld& gWorld, Renderer* rend, const float& damageValue = 10.0f, const int& playerID = 0): GameObject("projectile"), renderInst(rend), gameWorld(gWorld), damagePerShot(damageValue), ownerPlayerID(playerID) {
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override;

			float GetDamage() const { return damagePerShot; }

			int GetOwnerPlayerID() const { return ownerPlayerID; }

			void Update(float dt) override;

		private:
			float lifeSpan = 5.0f;
			GameWorld& gameWorld;
			Renderer* renderInst;
			int ownerPlayerID;
			float damagePerShot;
		};
	}
}
