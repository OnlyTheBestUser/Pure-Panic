#include "Player.h"
#include "../GameTech/LevelLoader.h"

using namespace NCL;
using namespace CSC8503;

void Player::OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal)
{
	if (other->GetName() == "projectile") {
		Projectile* projectile = ((Projectile*)other);
		if (projectile->GetOwnerPlayerID() != playerID) {
			DealDamage(projectile->GetDamage());
		}
	}
}

void Player::Update(float dt)
{
	timeSincePrevShot += dt;

	if (currentPowerUp != PowerUpType::None) {
		powerupTime -= dt;
		if (powerupTime <= 0.0f)
		{
			ResetPowerUps();
		}
	}

	if (camLocked)
	{
		camera->SetPosition(GetTransform().GetPosition() + Vector3(0, 3, 0));
		GetTransform().SetOrientation(Matrix4::Rotation(camera->GetYaw(), Vector3(0, 1, 0)));
	}
	
	if (force.y == 0) GetPhysicsObject()->AddAcceleration(force.Normalised() * curSpeed * dt);
	else GetPhysicsObject()->AddAcceleration(force.Normalised() * inAirSpeed * dt);

	// For smooth jump mechanism
	if (CheckDistToGround() < 0.5f) {
		canJump = true;
	}
	else {
		canJump = false;
	}

	// Check if grounded, if so don't apply more gravity
	if (CheckDistToGround() < 0.01f && force.y <= 0.0f)
	{
		Vector3 currentVel = GetPhysicsObject()->GetLinearVelocity();
		GetPhysicsObject()->SetLinearVelocity(Vector3(currentVel.x, 0.0f, currentVel.z));

		physicsObject->SetGravity(false);
	}
	else {
		physicsObject->SetGravity(true);
	}

	
	force = Vector3(0, 0, 0);
	
	if (IsDead()) {
		Respawn();
	}

	Debug::Print("Health: " + std::to_string(health), { 50.0f,90.0f });
}

float Player::CheckDistToGround()
{
	Ray ray(GetTransform().GetPosition(), Vector3(0, -1, 0));
	RayCollision closestCollision;
	world->Raycast(ray, closestCollision, true);
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

void Player::Fire() {
	if (timeSincePrevShot > fireRate)
	{
		levelLoader->SpawnProjectile(this);
		timeSincePrevShot = 0.0f;
		fired = true;
	}
	else {
		fired = false;
	}

}

bool Player::IsDead(){
	if (health <= 0.0f) {
		std::cout << "I'm Dead" << std::endl;
		return true;
	}
	return false;
}

void Player::Respawn(){
	GetTransform().SetPosition(spawnPos);
	health = maxHealth;
}

void Player::Reset() 
{

}