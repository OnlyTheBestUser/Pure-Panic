#pragma once
#include "Constraint.h"
#include "../../Common/Vector3.h"
#include "../CSC8503Common/GameObject.h"
namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class SpinningConstraint : public Constraint {
		public:
			SpinningConstraint(GameObject* obj, Vector3 angle) : object(obj), angle(angle) {};
			~SpinningConstraint() {};

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;
			Vector3 angle;
		};
	}
}