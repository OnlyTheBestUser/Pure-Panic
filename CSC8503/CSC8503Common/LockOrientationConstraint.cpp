#include "LockOrientationConstraint.h"
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

void LockOrientationConstraint::UpdateConstraint(float dt)
{

	object->GetTransform().SetOrientation(orientation);

}
