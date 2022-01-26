#include "Paddle.h"

using namespace NCL;
using namespace CSC8503;

void Paddle::Interact(float dt) {
	physicsObject->AddForce(-normal * maxForce * dt);
}