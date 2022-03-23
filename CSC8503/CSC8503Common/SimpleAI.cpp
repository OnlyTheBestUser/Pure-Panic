#include "SimpleAI.h"
#include "Debug.h"
#include "../CSC8503Common/GameWorld.h"
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

		Avoid();

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

Vector3 SimpleAI::Avoid()
{
#if DRAW_DEBUG
	Vector3 test = (Matrix4::Rotation(30, Vector3(0, 1, 0)) * Matrix4::Translation(physicsObject->GetLinearVelocity())).GetPositionVector();
	Vector3 test2 = (Matrix4::Rotation(-30, Vector3(0, 1, 0)) * Matrix4::Translation(physicsObject->GetLinearVelocity())).GetPositionVector();
	Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + (test.Normalised() * 10.0f));
	Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + (test2.Normalised() * 10.0f));
#endif
	// Raycast ahead, if it hits something, raycast to the sides, whichever gets the furthest, go in that direction.
	Ray forwardRay(transform.GetPosition() + (physicsObject->GetLinearVelocity().Normalised() * 1.5f), physicsObject->GetLinearVelocity().Normalised() + Vector3(0,0.0000000000000001f,0));
	forwardRay.SetCollisionLayers(CollisionLayer::LAYER_ONE | CollisionLayer::LAYER_THREE);
	RayCollision forwardCollision;

	Vector4 colour = Debug::BLUE;

	if (GameWorld::Raycast(forwardRay, forwardCollision, true)) {
		colour = Debug::RED;
		//((GameObject*)forwardCollision.node)->GetRenderObject()->SetColour(Debug::RED);
		if ((forwardCollision.collidedAt - transform.GetPosition()).Length() < avoid_distance) {
			std::cout << (forwardCollision.collidedAt - transform.GetPosition()).Length() << std::endl;
		}
	}

	Debug::DrawLine(forwardRay.GetPosition(), forwardRay.GetPosition() + (forwardRay.GetDirection().Normalised() * avoid_distance), colour);

	return Vector3();
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
