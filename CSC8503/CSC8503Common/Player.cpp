#include "Player.h"

using namespace NCL;
using namespace CSC8503;

void Player::OnCollisionBegin(GameObject* other, Vector3 localA, Vector3 localB, Vector3 normal)
{

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

Projectile* Player::spawnProjectile(const float& initialSpeed, const float& meshSize) {
	//float meshSize = 0.5f;
	float inverseMass = 5.0f;
	//Vector3 offset = Vector3(0, 2.5f, 0);
	Vector3 camForwardVector = this->GetCamFrontVec();

	Projectile* projectile = new Projectile(gameWorld);

	SphereVolume* volume = new SphereVolume(meshSize * 1.4);// / 2.0f * meshSize * 1.3f);
	projectile->SetBoundingVolume((CollisionVolume*)volume);

	projectile->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		//.SetOrientation(Quaternion(Matrix3::Rotation(camera->GetPitch() + 90, Vector3(0, 0, 1))) * Quaternion(Matrix3::Rotation(camera->GetYaw() + 90, Vector3(1, 0, 0))))
		.SetPosition(camera->GetPosition() + camForwardVector * 5.0f); //+ offset);

	projectile->SetRenderObject(new RenderObject(&projectile->GetTransform(), projectileMesh, nullptr, basicShader));
	projectile->SetPhysicsObject(new PhysicsObject(&projectile->GetTransform(), projectile->GetBoundingVolume()));

	projectile->GetPhysicsObject()->SetInverseMass(inverseMass);
	//projectile->GetPhysicsObject()->SetFriction(1.0f);
	//projectile->GetPhysicsObject()->SetLinearDamping(10.0f);
	projectile->GetPhysicsObject()->InitSphereInertia();

	float velocityDueToMovement = Vector3::Dot(camForwardVector, this->GetPhysicsObject()->GetLinearVelocity());
	if (velocityDueToMovement < 0.0f) velocityDueToMovement = 0.0f;
	//Debug::DrawArrow(projectile->GetTransform().GetPosition(), projectile->GetTransform().GetPosition() + camForwardVector * Vector3::Dot(camForwardVector, this->GetPhysicsObject()->GetLinearVelocity()), Debug::CYAN, 1000.0f);
	projectile->GetPhysicsObject()->AddAcceleration(camForwardVector * (initialSpeed + velocityDueToMovement));
	projectile->GetTransform().SetOrientation(Quaternion(Matrix3::Rotation(-acos(Vector3::Dot(Vector3(0, 1, 0), camForwardVector)) * 180.0f / 3.14f, Vector3::Cross(camForwardVector, Vector3(0, 1, 0)).Normalised())));

	//Debug::DrawAxisLines(projectile->GetTransform().GetMatrix(), 2.0f, 1000.0f);
	//Debug::DrawArrow(camera->GetPosition(), (camera->GetPosition() + this->GetCamFrontVec() * 100.0f), Debug::RED, 5.0f);

	projectile->GetPhysicsObject()->SetDynamic(true);
	projectile->GetPhysicsObject()->SetLinearDamping(this->GetPhysicsObject()->GetLinearDamping() * 0.4);
	projectile->SetCollisionLayers(CollisionLayer::LAYER_FIVE);

	gameWorld.AddGameObject(projectile);

	return projectile;
}

void Player::Fire() {
	if (timeSincePrevShot > fireRate)
	{
		spawnProjectile();
		timeSincePrevShot = 0.0f;
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