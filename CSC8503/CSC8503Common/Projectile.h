#pragma once
#include "GameWorld.h"
#include "GameObject.h"

namespace NCL {

	namespace CSC8503 {

		class Projectile : public GameObject
		{
		public:
			Projectile(GameWorld& gWorld, float capsuleRadius, float capsuleHeight): GameObject("projectile"), gameWorld(gWorld), radius(capsuleRadius), height(capsuleHeight) {
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override {
				gameWorld.RemoveGameObject(this, true);
			}

			void Update(float dt) override;
		private:
			//Vector3 velocity;
			GameWorld& gameWorld;
			float radius;
			float height;
		};
	}
}
