#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class SimpleAI : public GameObject {
		public:
			SimpleAI() {
				max_speed		= 20.0f;
				max_force		= 5.0f;
				max_predict		= 20.0f;
				arrive_radius	= 30.0f;
				avoid_distance	= 20.0f;
				avoid_magnitude = 50.0f;
				health			= 100.0f;
				shoot_radius	= 50.0f;
				max_shoot_time	= 0.2f;
				shoot_time		= 0.0f;
				spawnPoint = Vector3(10, 3, 10);
			};
			virtual ~SimpleAI() {};

			void Update(float dt) override;

			void OnCollisionBegin(GameObject* otherObject, Vector3 localA, Vector3 localB, Vector3 normal) override;

			Vector3 Seek(Vector3 seekTarget);
			Vector3 Arrive(Vector3 arriveTarget);
			float Avoid();
			void Shoot(float dt);
			Vector3 Pursue();
			void RotateToVelocity();

			void SetTarget(GameObject* t) {
				target = t;
			}

			void Respawn() {
				transform.SetPosition(spawnPoint);
			}

		protected:
			GameObject* target;
			float max_speed;
			float max_force;
			float max_predict;
			float arrive_radius;
			float avoid_distance;
			float avoid_magnitude;
			float health;
			float shoot_radius;
			float shoot_time;
			float max_shoot_time;
			Vector3 spawnPoint;
		};
	}
}