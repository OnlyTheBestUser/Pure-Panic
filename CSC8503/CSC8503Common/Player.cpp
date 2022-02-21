#include "Player.h"
#include "PhysicsSystem.cpp"

using namespace NCL;
using namespace CSC8503;

void Player::OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal)
{

}

void Player::Update(float dt)
{
	if (camLocked)
	{
		camera->SetPosition(GetTransform().GetPosition() + Vector3(0, 3, 0));
		GetTransform().SetOrientation(Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)));
	}
	GetPhysicsObject()->AddAcceleration(force.Normalised() * speed);
	force = Vector3(0, 0, 0);

	// Check if grounded, if so don't apply more gravity
	if (CheckDistToGround() < 0.01f)
	{
		Vector3 currentVel = GetPhysicsObject()->GetLinearVelocity();
		GetPhysicsObject()->SetLinearVelocity(Vector3(currentVel.x, 0.0f, currentVel.z));

		physicsObject->SetGravity(false);
	}
	else {
		physicsObject->SetGravity(true);
	}
}

float Player::CheckDistToGround()
{
	Ray ray(GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision closestCollision;
	gameWorld.Raycast(ray, closestCollision, true);
	float distToGround = GetTransform().GetPosition().y - closestCollision.collidedAt.y;

	const CollisionVolume* volume = GetBoundingVolume();
	switch (GetBoundingVolume()->type)
	{
	case VolumeType::AABB: distToGround -= ((const AABBVolume&)* volume).GetHalfDimensions().y; break;
	case VolumeType::OBB: distToGround -= ((const OBBVolume&)* volume).GetHalfDimensions().y; break;
	case VolumeType::Sphere: distToGround -= ((const SphereVolume&)* volume).GetRadius(); break;
	case VolumeType::Capsule: distToGround -= ((const CapsuleVolume&)* volume).GetHalfHeight(); break;
	}

	return distToGround;
}

void Player::Reset() 
{

}