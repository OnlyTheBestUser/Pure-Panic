#include "SpringConstraint.h"
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

void SpringConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePos = object->GetTransform().GetPosition() - *target;

	if (abs(relativePos.Length() - spring->GetLength()) > 0.01f) {
		Vector3 offsetDir = relativePos.Normalised();

		float offset = relativePos.Length();

		Vector3 impulse = offsetDir * -(spring->GetK() * (offset - spring->GetLength()));

		object->GetPhysicsObject()->ApplyLinearImpulse(impulse * dt);
	}
}
