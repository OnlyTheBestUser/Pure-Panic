#include "SpinningConstraint.h"

using namespace NCL;
using namespace CSC8503;

void SpinningConstraint::UpdateConstraint(float dt)
{
	Vector3 oldRot = object->GetTransform().GetOrientation().ToEuler();
	Vector3 newRot = oldRot + angle;
	Quaternion newQuat = Quaternion::EulerAnglesToQuaternion(newRot.x, newRot.y, newRot.z);
	object->GetTransform().SetOrientation(newQuat);
}
