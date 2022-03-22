#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class SimpleAI : public GameObject {
		public:
			SimpleAI() {
				speed = 5.0f;
			};
			virtual ~SimpleAI() {};

			void Update(float dt) override;

			void SetTarget(GameObject* t) {
				target = t;
			}

		protected:
			GameObject* target;
			float speed;

		};
	}
}