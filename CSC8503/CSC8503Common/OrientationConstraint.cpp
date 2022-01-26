#include "OrientationConstraint.h"
#include "../CSC8503Common/GameObject.h"
using namespace NCL;
using namespace CSC8503;

OrientationConstraint::OrientationConstraint(GameObject* a, GameObject* b, Vector3 x)
{
	objectA = a;
	objectB = b;
	angle = x;
}

void OrientationConstraint::UpdateConstraint(float dt) {
	Vector3 relativeRot = 
		objectB->GetTransform().GetOrientation().ToEuler() -
	objectA->GetTransform().GetOrientation().ToEuler();

	PhysicsObject* physA = objectA->GetPhysicsObject();
	PhysicsObject* physB = objectB->GetPhysicsObject();

	Vector3 relativeVelocity = physB->GetAngularVelocity() - physA->GetAngularVelocity();
	float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

	Vector3 offset = angle - relativeRot;

	if (abs(offset.x) > 0.01f || abs(offset.y) > 0.01f || abs(offset.z) > 0.01f) {
		Vector3 offsetDir = relativeRot.Normalised();

		float velocityDot = Vector3::Dot(relativeVelocity, offsetDir.Normalised());

		float biasFactor = 0.01f;
		float bias = -(biasFactor / dt) * offset.Length() * 5.0f;

		float lambda = -(velocityDot + bias) / constraintMass;

		Vector3 aImpulse = offsetDir * lambda * dt;
		Vector3 bImpulse = -offsetDir * lambda * dt;

		physA->ApplyAngularImpulse(aImpulse); // multiplied by mass here
		physB->ApplyAngularImpulse(bImpulse);
	}

}