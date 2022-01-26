#pragma once
#include "Constraint.h"
#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameObject;
		
		class LockOrientationConstraint : public Constraint {
		public:
			LockOrientationConstraint(GameObject* object, Quaternion orientation) : object(object), orientation(orientation) {};
			~LockOrientationConstraint() {};

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;
			Quaternion orientation;
		};
	}
}