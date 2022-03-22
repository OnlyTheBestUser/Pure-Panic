#include "SimpleAI.h"
#include "Debug.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define DRAW_DEBUG true

using namespace NCL;
using namespace CSC8503;

void SimpleAI::Update(float dt)
{
	if (target) {
		Vector3 force;
		force += Arrive(target->GetTransform().GetPosition());
		physicsObject->AddForce(force);
		RotateToVelocity();
	}
}

Vector3 SimpleAI::Seek(Vector3 seekTarget)
{
	Vector3 force = (seekTarget - transform.GetPosition()).Normalised() * max_speed;
	force -= physicsObject->GetLinearVelocity();

	if (force.Length() > max_force) {
		force = force.Normalised() * max_force;
	}

	force.y = 0.0f;
	return force;
}

Vector3 SimpleAI::Arrive(Vector3 arriveTarget)
{
	Vector3 force = (arriveTarget - transform.GetPosition()).Normalised() * max_speed;
	force -= physicsObject->GetLinearVelocity();

	if (force.Length() > max_force) {
		force = force.Normalised() * max_force;
	}
	
	force.y = 0.0f;
	
	float distance = (arriveTarget - transform.GetPosition()).Length();
	float multiply = distance / arrive_radius;
	if (multiply > 1.0f)
		multiply = 1.0f;

	force *= multiply;

#if DRAW_DEBUG
	Debug::DrawSphere(arriveTarget, arrive_radius, Debug::RED);
#endif

	return force;
}

Vector3 SimpleAI::Pursue()
{
	Vector3 targetPos = target->GetTransform().GetPosition() + (target->GetPhysicsObject()->GetLinearVelocity().Normalised() * max_predict);
#if DRAW_DEBUG
	Debug::DrawSphere(targetPos, 1.0f, Debug::RED);
#endif

	return Seek(targetPos);
}

void SimpleAI::RotateToVelocity()
{
	float angle = acos(Vector3::Dot(physicsObject->GetLinearVelocity().Normalised(), Vector3(0, 0, -1)));
	angle *= (180.0f / M_PI);
	if (physicsObject->GetLinearVelocity().x > 0.0f)
		angle *= -1;
	transform.SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), angle));
}
