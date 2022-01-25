#pragma once
#include "GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class SpeedUp : public GameObject
		{
		public:
			SpeedUp(float sp, float time = 5.0f) : speed(sp), time(time), GameObject("speed") {};
			~SpeedUp() {};

			void OnCollisionBegin(GameObject* otherObject) override;

			float GetSpeed() const { return speed; }

		protected:
			float speed;
			float time;
		};
	}
}

