#pragma once
#include "GameObject.h"
#include "../../Common/Vector3.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class Paddle : public GameObject {
		public:
			Paddle(Vector3 normal, float force, string name = "") : normal(normal), maxForce(force), GameObject(name) {
			};
			~Paddle() {};

			void Interact(float dt) override;
		protected:
			Vector3 normal;
			float maxForce;
		};
	}
}