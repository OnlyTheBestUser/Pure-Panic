#pragma once
#include "Constraint.h"
#include "Spring.h"
#include "../../Common/Vector3.h"
namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameObject;

		class SpringConstraint : public Constraint {
		public:
			SpringConstraint(GameObject* a, Vector3* t, Spring* s) {
				object = a;
				target = t;
				spring = s;
			}
			~SpringConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;
			Vector3* target;
			Spring* spring;
		};
	}
}