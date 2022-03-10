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
			Projectile(GameWorld& gWorld, Renderer* rend): GameObject("projectile"), gameWorld(gWorld), renderInst(rend) {
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override;

			void Update(float dt) override;

		private:
			//Vector3 velocity;
			GameWorld& gameWorld;
			Renderer* renderInst;
		};
	}
}
