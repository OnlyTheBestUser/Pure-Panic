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
			Projectile(Renderer* rend, const int& playerID = 0, const float& damageValue = 10.0f): GameObject("projectile"), renderInst(rend), damagePerShot(damageValue), ownerPlayerID(playerID) {
				this->SetTrigger(true);
			};
			~Projectile() {}

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override;
			void PaintCircle(const Vector3& position, const Quaternion& rotation, const Vector4& colour, int numberOfPoints, float circleRadius, float paintRadiusModifier);

			float GetDamage() const { return damagePerShot; }

			int GetOwnerPlayerID() const { return ownerPlayerID; }

			void Update(float dt) override;

			void SetAsDeathProjectile() {
				IsDeathProjectile = true;
			}

		private:
			float lifeSpan = 5.0f;
			Renderer* renderInst;
			int ownerPlayerID;
			float damagePerShot;
			bool IsDeathProjectile = false;
		};
	}
}
