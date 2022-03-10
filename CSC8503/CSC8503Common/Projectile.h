#pragma once
#include "GameWorld.h"
#include "GameObject.h"

namespace NCL {

	namespace CSC8503 {

		class Projectile : public GameObject
		{
		public:
			Projectile(GameWorld& gWorld): GameObject("projectile"), gameWorld(gWorld) {
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				gameWorld.RemoveGameObject(this, true);
			}

			void Update(float dt) override;

		private:
			float lifeSpan = 5.0f;
			GameWorld& gameWorld;
		};
	}
}
