#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class SimpleAI : public GameObject {
		public:
			SimpleAI() {
				max_speed = 20.0f;
				max_force = 5.0f;
				max_predict = 20.0f;
				arrive_radius = 30.0f;
				avoid_distance = 10.0f;
			};
			virtual ~SimpleAI() {};

			void Update(float dt) override;

			Vector3 Seek(Vector3 seekTarget);
			Vector3 Arrive(Vector3 arriveTarget);
			Vector3 Avoid();
			Vector3 Pursue();
			void RotateToVelocity();

			void SetTarget(GameObject* t) {
				target = t;
			}

		protected:
			GameObject* target;
			float max_speed;
			float max_force;
			float max_predict;
			float arrive_radius;
			float avoid_distance;
		};
	}
}