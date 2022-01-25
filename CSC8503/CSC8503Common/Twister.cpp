#include "Twister.h"

using namespace NCL;
using namespace CSC8503;

void Twister::Interact(float dt)
{
	physicsObject->ApplyAngularImpulse(transform.GetOrientation() * Vector3(0, 1, 0) * 500.0f * dt);
}
