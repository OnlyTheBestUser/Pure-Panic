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

Projectile* Player::AddProjectileToWorld(const Vector3& position, const float& radius, const float& height, const float& initialSpeed) {
	float meshSize = 0.5f;
	float inverseMass = 5.0f;
	Vector3 offsetFromPlayer = Vector3(5.0f, 2.5f, 0);

	Projectile* projectile = new Projectile(gameWorld, radius, height);

	SphereVolume* volume = new SphereVolume(height / 2.0f * meshSize * 1.3f);
	projectile->SetBoundingVolume((CollisionVolume*)volume);

	projectile->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetOrientation(Quaternion(Matrix3::Rotation(90, Vector3(0, 0, 1))))
		.SetPosition(this->GetTransform().GetPosition() + offsetFromPlayer);

	projectile->SetRenderObject(new RenderObject(&projectile->GetTransform(), projectileMesh, nullptr, basicShader));
	projectile->SetPhysicsObject(new PhysicsObject(&projectile->GetTransform(), projectile->GetBoundingVolume()));

	projectile->GetPhysicsObject()->SetInverseMass(inverseMass);
	//projectile->GetPhysicsObject()->SetFriction(1.0f);
	//projectile->GetPhysicsObject()->SetLinearDamping(10.0f);
	projectile->GetPhysicsObject()->InitSphereInertia();
	projectile->GetPhysicsObject()->AddAcceleration(Vector3(1, 0, 0) * initialSpeed);//player1->GetForwardVector()
	Debug::DrawAxisLines(projectile->GetTransform().GetMatrix(), 2.0f, 1000.0f);
	projectile->SetDynamic(true);
	projectile->SetCollisionLayers(CollisionLayer::LAYER_ONE);

	gameWorld.AddGameObject(projectile);

	return projectile;
}

void Player::Fire() {
	AddProjectileToWorld(Vector3(5, 5, 0), 0.3f, 1.0f);
}

void Player::Reset() 
{

}