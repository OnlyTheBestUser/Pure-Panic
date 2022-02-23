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

			void OnCollisionEnd(GameObject* otherObject) override {
				gameWorld.RemoveGameObject(this, true);
			}
		private:
			//Vector3 velocity;
			GameWorld& gameWorld;
			float radius;
			float height;
		};
	}
}
